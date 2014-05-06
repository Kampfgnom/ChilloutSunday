#include "qserienjunkiesreply.h"
#include <QSharedData>

#include "qserienjunkies.h"

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QDebug>
#include <QRegularExpression>
#include <QPointer>

#include <functional>

static QString removeHTMLTags(QString line) {
    return line.replace(QRegularExpression("</?strong>"), "")
            .replace(QRegularExpression("</?p>"), "")
            .replace(QRegularExpression("<br />"), "")
            .replace(QRegularExpression("</?div>"), "");
}

static const QString UPLOADER_KEY      = "Uploader";
static const QString LANGUAGE_KEY      = "Sprache";
static const QString DURATION_KEY      = "Dauer";
static const QString FORMAT_KEY        = "Format";
static const QRegularExpression MIRROR_PATTERN =
        QRegularExpression(".*href=\"([^\"]*)\".*\">(part.*?|hier)</a> \\| ([^< ]+).*",
                           QRegularExpression::CaseInsensitiveOption
                           | QRegularExpression::DotMatchesEverythingOption);

class QSerienJunkiesReplyData : public QSharedData {
public:
    QSerienJunkiesReplyData() : QSharedData(),
        finishedSeasons(0),
        seasonCount(0),
        downloadLinkCount(0),
        finishedDownloadLinks(0)
    {}

    QList<QSerienJunkiesReply::Series> series;
    QList<QSerienJunkiesReply::Season> seasons;
    QList<QSerienJunkiesReply::Format> formats;
    //      formatDescription ->            Mirror  ->  List of downloadLinks
    QHash<  QString,                QHash<  QString,    QList<QSerienJunkiesReply::DownloadLink> > > cryptedDownloadLinks;
    QString errorString;
    int finishedSeasons;
    int seasonCount;
    QString hiddenFormData;
    QByteArray captchaData;
    QUrl captchaPageUrl;
    int downloadLinkCount;
    int finishedDownloadLinks;
    QList<QUrl> downloadLinks;
    QString packageName;

    QPointer<QNetworkReply> reply;

    QSerienJunkiesReply *q;
};

QSerienJunkiesReply::QSerienJunkiesReply(QObject *parent) :
    QObject(parent),
    data(new QSerienJunkiesReplyData)
{
    data->q = this;
}

QSerienJunkiesReply::~QSerienJunkiesReply()
{
}

void (QNetworkReply:: *ERRORSIGNAL)(QNetworkReply::NetworkError) = &QNetworkReply::error;

void QSerienJunkiesReply::searchSeries(const QString &string)
{
    QByteArray postData;
    postData.append("string=");
    postData.append(string);

    QNetworkRequest request = QNetworkRequest(QUrl("http://serienjunkies.org/media/ajax/search/search.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    data->reply = QSerienJunkies::networkAccessManager()->post(request, postData);
    connect(data->reply, &QNetworkReply::finished, this, &QSerienJunkiesReply::seriesSearchReplyFinished);
    connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
}

void QSerienJunkiesReply::seriesSearchReplyFinished()
{
    QJsonParseError jsonError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data->reply->readAll(), &jsonError);
    data->reply.clear();

    if(jsonError.error != QJsonParseError::NoError) {
        data->errorString = "The returned JSON was not valid: "+jsonError.errorString();
        emit error();
        return;
    }

    QVariant v = jsonDocument.toVariant();
    if(static_cast<QMetaType::Type>(v.type()) != QMetaType::QVariantList) {
        data->errorString = "The returned JSON is no list.";
        emit error();
        return;
    }

    QVariantList resultList = v.toList();
    data->seasonCount = resultList.size();

    if(data->seasonCount == 0) {
        emit finished();
        return;
    }

    // Get the given URLs, because the actual URL of a series is in the "Location" header of these URLs.
    foreach(QVariant result, resultList) {
        QVariantList list = result.toList();
        if(list.size() != 2)
            continue;
        Series r;
        r.id = list.at(0).toInt();
        r.name = list.at(1).toString();
        int index = data->series.size();
        data->series.append(r);

        QUrl url(QString("http://serienjunkies.org/?cat=%1").arg(r.id));
        QNetworkReply *reply = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(url));
        connect(reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
        connect(reply, &QNetworkReply::finished, [=] {
            data->series[index].url = reply->header(QNetworkRequest::LocationHeader).toUrl();
            reply->deleteLater();
            ++data->finishedSeasons;

            if(data->finishedSeasons == data->seasonCount)
                emit finished();
        });
    }
}

void QSerienJunkiesReply::searchSeasons(const QUrl &seriesUrl)
{
    data->reply = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(seriesUrl));
    connect(data->reply, &QNetworkReply::finished, this, &QSerienJunkiesReply::seasonSearchReplyFinished);
    connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
}

void QSerienJunkiesReply::seasonSearchReplyFinished()
{
    QString page = QString::fromUtf8(data->reply->readAll());

    QRegularExpression reg("\\&nbsp\\;<a href=\"http://serienjunkies.org/(.*?)/\">(.*?)</a><br");
    QRegularExpressionMatchIterator it = reg.globalMatch(page);

    while(it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        Season result;
        result.title = match.captured(2);
        result.url = QUrl(match.captured(1).prepend("http://serienjunkies.org/"));
        data->seasons.append(result);
    }

    emit finished();
}

void QSerienJunkiesReply::searchDownloads(const QUrl &seasonUrl)
{
    data->reply = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(seasonUrl));
    connect(data->reply, &QNetworkReply::finished, this, &QSerienJunkiesReply::downloadSearchReplyFinished);
    connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
}

void QSerienJunkiesReply::downloadSearchReplyFinished()
{
    QString page = QString::fromUtf8(data->reply->readAll());

    if(page.isEmpty()) {
        QUrl location = data->reply->header(QNetworkRequest::LocationHeader).toUrl();
        if(location.isValid()) {
            data->reply = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(location));

            connect(data->reply, &QNetworkReply::finished, this, &QSerienJunkiesReply::downloadSearchReplyFinished);
            connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
            return;
        }
        else {
            data->errorString = "Episode search reply was empty.";
            emit error();
            return;
        }
    }

    // This code has been taken from JDownloader and adjusted for Qt
    QStringList lines = page.split('\n');

    Format currentFormat;
    QString normalName = "";
    foreach(QString line, lines) {
        if (line.contains(DURATION_KEY)
                || line.contains(FORMAT_KEY)
                || line.contains(LANGUAGE_KEY)) {
            if (!currentFormat.description.isEmpty()
                    && data->cryptedDownloadLinks.contains(currentFormat.description)) {
                data->formats.append(currentFormat);
            }
            currentFormat = Format();
            currentFormat.description = removeHTMLTags(line);
        }
        else if (line.contains("Download:")) {
            QString mirror;
            QString url;
            QRegularExpressionMatch match = MIRROR_PATTERN.match(line);
            if (match.hasMatch()) {
                url = match.captured(1);
                mirror = match.captured(3);
            }
            DownloadLink link;
            link.name = normalName;
            link.url = url;

            if(!currentFormat.mirrors.contains(mirror))
                currentFormat.mirrors.append(mirror);
            data->cryptedDownloadLinks[currentFormat.description][mirror].append(link);
        }
        else if (line.contains("<strong>")) {
            normalName = removeHTMLTags(line);
        }
    }

    if (!currentFormat.description.isEmpty()) {
        data->formats.append(currentFormat);
    }

    emit finished();
}

void QSerienJunkiesReply::decrypt(const QUrl &url)
{
    data->reply = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(url));
    connect(data->reply, &QNetworkReply::finished, this, &QSerienJunkiesReply::decryptLinkReplyFinished);
    connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
}

void QSerienJunkiesReply::decryptLinkReplyFinished()
{
    QString page = QString::fromUtf8(data->reply->readAll());
    data->captchaPageUrl = data->reply->url();
    decryptLinkReplyFinishedHelper(page);
}

void QSerienJunkiesReply::decryptLinkReplyFinishedHelper(const QString &page)
{
    QRegularExpression packageNameReg("<TITLE>.* \\- (.*?)</TITLE>");
    data->packageName = packageNameReg.match(page).captured(1);

    int index = page.indexOf("<FORM ACTION=\"\" METHOD=\"post\" NAME=\"INPF\" ID=\"postit\" STYLE=\"display:inline;\">");
    if(index < 0) {
        data->errorString = "Could not find captcha formular.";
        emit error();
        return;
    }
    int index2 = page.indexOf("</FORM>", index);
    if(index2 < 0
            || index2 < index) {
        data->errorString = "Could not find end of captcha formular.";
        emit error();
        return;
    }

    QString form = page.mid(index, index2 - index);

    QRegularExpression reg("\\<INPUT TYPE=\"HIDDEN\" NAME=\"s\" VALUE=\"(.*)\"\\>");
    QRegularExpressionMatch match = reg.match(form);
    if(!match.hasMatch()) {
        data->errorString = "Could not find hidden form data of captcha.";
        emit error();
        return;
    }
    data->hiddenFormData = match.captured(1);

    reg = QRegularExpression("\\<IMG SRC=\"/secure/(.*?)\"");
    match = reg.match(form);
    if(!match.hasMatch()) {
        data->errorString = "Could not find captcha.";
        emit error();
        return;
    }

    QUrl captchaUrl(QString("http://download.serienjunkies.org/secure/%1").arg(match.captured(1)));

    data->reply = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(captchaUrl));
    connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);

    connect(data->reply, &QNetworkReply::finished, [=]() {
        data->captchaData = data->reply->readAll();
        emit requiresCaptcha();
    });
}

void QSerienJunkiesReply::solveCaptcha(const QString &captcha)
{
    QByteArray postData;
    postData += QString("s=%1").arg(data->hiddenFormData);
    postData += QString("&c=%1").arg(captcha);
    postData += QString("&action=Download");

    QNetworkRequest request(data->captchaPageUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    data->reply = QSerienJunkies::networkAccessManager()->post(request, postData);

    connect(data->reply, &QNetworkReply::finished, this, &QSerienJunkiesReply::decryptedLinkReplyFinished);
    connect(data->reply, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
}

QString QSerienJunkiesReply::packageName() const
{
    return data->packageName;
}

void QSerienJunkiesReply::decryptedLinkReplyFinished()
{
    QString page = QString::fromUtf8(data->reply->readAll());

    QRegularExpression findForms("\\<FORM ACTION=\"(.+)\" STYLE=\"display: inline;\" TARGET=\"_blank\"\\>");
    QRegularExpressionMatchIterator it = findForms.globalMatch(page);

    if(!it.hasNext()) {
        decryptLinkReplyFinishedHelper(page);
        return;
    }

    while(it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString url = match.captured(1);
        if(url.contains("download.serienjunkies.org")
                && !url.contains("firstload")
                && url != QLatin1String("http://mirror.serienjunkies.org")) {
            ++data->downloadLinkCount;
            QNetworkReply *r = QSerienJunkies::networkAccessManager()->get(QNetworkRequest(QUrl(url)));
            connect(r, ERRORSIGNAL ,this, &QSerienJunkiesReply::handleError);
            connect(r, &QNetworkReply::finished, [=]() {
                QUrl urlLocation = r->header(QNetworkRequest::LocationHeader).toUrl();
                data->downloadLinks.append(urlLocation);
                r->deleteLater();

                ++data->finishedDownloadLinks;
                if(data->downloadLinkCount == data->finishedDownloadLinks)
                    emit finished();
            });
        }
    }
}

void QSerienJunkiesReply::handleError()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    data->errorString = reply->errorString();
    reply->deleteLater();
    emit error();
}

QList<QSerienJunkiesReply::Series> QSerienJunkiesReply::series() const
{
    return data->series;
}

QList<QSerienJunkiesReply::Season> QSerienJunkiesReply::seasons() const
{
    return data->seasons;
}

QList<QSerienJunkiesReply::Format> QSerienJunkiesReply::formats() const
{
    return data->formats;
}

QString QSerienJunkiesReply::errorString() const
{
    return data->errorString;
}


QList<QSerienJunkiesReply::DownloadLink> QSerienJunkiesReply::downloadLinks(const QSerienJunkiesReply::Format &format, const QString &mirror) const
{
    if(data->cryptedDownloadLinks.contains(format.description)) {
        auto linksPerMirror = data->cryptedDownloadLinks[format.description];
        if(linksPerMirror.contains(mirror)) {
            return linksPerMirror[mirror];
        }
    }

    return QList<QSerienJunkiesReply::DownloadLink>();
}

QByteArray QSerienJunkiesReply::captcha() const
{
    return data->captchaData;
}

QList<QUrl> QSerienJunkiesReply::urls() const
{
    return data->downloadLinks;
}


QList<QSerienJunkiesReply::DownloadLink> QSerienJunkiesReply::downloadLinks(const QSerienJunkiesReply::Format &format, const QStringList &mirrors) const
{
    QList<QSerienJunkiesReply::DownloadLink> result;
    foreach(const QString mirror, mirrors) {
        if(data->cryptedDownloadLinks.contains(format.description)) {
            auto linksPerMirror = data->cryptedDownloadLinks[format.description];
            if(linksPerMirror.contains(mirror)) {
                result.append(linksPerMirror[mirror]);
            }
        }
    }

    return result;
}
