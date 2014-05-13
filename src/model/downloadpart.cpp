#include "downloadpart.h"

#include "downloadpackage.h"

#include <preferences.h>
#include <application.h>
#include <controller/extraction.h>

#include <QUrlQuery>
#include <QDir>
#include <QJsonDocument>

static void (QNetworkReply:: *ERRORSIGNAL)(QNetworkReply::NetworkError) = &QNetworkReply::error;
static const QRegularExpression LOCATION_REGEXP(QLatin1String("location\":\"(http[^\"]+)"));
static const  QRegularExpression FILENAME_REGEXP(QLatin1String("filename=\\\"(.*)\\\""));
static qint64 BUFFERSIZE = (2 << 19);

QString fileNameFromUrl(const QUrl &url)
{
    QString path = url.path();
    int index = 0;
    QString result;
    while (result.isEmpty() && index >= 0) {
        index = path.lastIndexOf('/', index - 1);
        Q_ASSERT(index >= 0);
        result = path.mid(index + 1);
    }
    return result;
}

QString fileNameFromReply(QNetworkReply *reply)
{
    QString contentDisposition = reply->header(QNetworkRequest::ContentDispositionHeader).toString();
    if (!contentDisposition.isEmpty()) {
        QRegularExpressionMatch match = FILENAME_REGEXP.match(contentDisposition);
        if (match.hasMatch()) {
            QString name = match.captured(1);
            if (!name.isEmpty())
                return name;
        }
    }

    return fileNameFromUrl(reply->url());
}

DownloadPart::DownloadPart(QObject *parent) :
    QObject(parent),
    m_package(QpRelation(&DownloadPart::package)),
    m_running(false),
    m_reply(nullptr),
    m_fileSize(0),
    m_file(nullptr),
    m_buffer(nullptr),
    m_bytesWritten(0)
{
}

DownloadPart::~DownloadPart()
{
    stopDownload();
}

void DownloadPart::cleanup()
{
    m_running = false;

    if(m_file && m_file->isOpen()) {
        m_file->flush();
        m_file->close();
        m_file->deleteLater();
        m_file = nullptr;
    }

    if(m_reply) {
        m_reply->deleteLater();
        m_reply = nullptr;
    }

    delete[] m_buffer;
    m_buffer = nullptr;
}


void DownloadPart::stopDownload()
{
    if (m_reply)
        m_reply->abort();

    cleanup();
}

QUrl DownloadPart::url() const
{
    return m_url;
}

QSharedPointer<DownloadPackage> DownloadPart::package() const
{
    return m_package;
}

quint64 DownloadPart::fileSize() const
{
    return m_fileSize;
}

QString DownloadPart::fileName() const
{
    return m_fileName;
}

quint64 DownloadPart::bytesWritten() const
{
    return m_bytesWritten;
}

bool DownloadPart::isRunning() const
{
    return m_running;
}

bool DownloadPart::isFinished() const
{
    QFileInfo info(m_fileName);
    return m_bytesWritten == m_fileSize
            && info.exists()
            && static_cast<quint64>(info.size()) == m_bytesWritten;
}

void DownloadPart::setFileName(QString arg)
{
    QDir downloadsFolder(Preferences::downloadFolder());
    if(QFileInfo(arg).isRelative())
        arg = downloadsFolder.absoluteFilePath(arg);

    if (m_fileName != arg) {
        m_fileName = arg;
        emit fileNameChanged(arg);
    }
}

void DownloadPart::setFileSize(quint64 arg)
{
    if (m_fileSize != arg) {
        m_fileSize = arg;
        emit fileSizeChanged(arg);
    }
}

void DownloadPart::setUrl(QUrl arg)
{
    if (m_url != arg) {
        m_url = arg;
        emit urlChanged(arg);
    }
}

void DownloadPart::setBytesWritten(quint64 arg)
{
    if (m_bytesWritten != arg) {
        m_bytesWritten = arg;
        emit bytesWrittenChanged(arg);
    }
}

void DownloadPart::increaseBytesWritten(quint64 writtenBytes)
{
    setBytesWritten(m_bytesWritten + writtenBytes);
    qDebug() << "RECEIVED " << bytesWritten() << "of" << fileSize() <<
                "=>" << fileName();
}

void DownloadPart::setPackage(QSharedPointer<DownloadPackage> arg)
{
    m_package = arg;
    emit packageChanged(arg);
}

void DownloadPart::handleNetworkError()
{
    if (m_reply->error() == QNetworkReply::OperationCanceledError)
        return;

    qDebug() << (tr("Network error: %1").arg(m_reply->errorString()));
    Qp::update(Qp::sharedFrom(this));
    cleanup();
}

void DownloadPart::startDownload()
{
    if (isRunning())
        return;

    if(isFinished()) {
        emit finished();
        return;
    }

    m_running = true;

    QFileInfo info(m_fileName);
    if(info.exists())
        m_bytesWritten = info.size();
    else
        m_bytesWritten = 0;

    Qp::update(Qp::sharedFrom(this));

    m_reply = qApp->networkAccessManager()->head(QNetworkRequest(url()));
    qDebug() << "HEAD" << url();

    connect(m_reply, ERRORSIGNAL, this, &DownloadPart::handleNetworkError);
    connect(m_reply, &QNetworkReply::finished, this, &DownloadPart::parseFileInfo);
}

void DownloadPart::parseFileInfo()
{
    setFileName(fileNameFromReply(m_reply));
    qDebug() << "Filename: " << fileName();

    QUrlQuery query;
    query.addQueryItem("method", "directdownloadlink");
    query.addQueryItem("params[login]", Preferences::premiumizeMeUserName());
    query.addQueryItem("params[pass]", Preferences::premiumizeMeUserPassword());
    query.addQueryItem("params[link]", url().toEncoded());

    QUrl getLinkUrl(QString::fromLatin1("https://api.premiumize.me/pm-api/v1.php"));
    getLinkUrl.setQuery(query);

    m_reply->deleteLater();
    m_reply = qApp->networkAccessManager()->get(QNetworkRequest(getLinkUrl));
    qDebug() << "GET" << getLinkUrl;

    connect(m_reply, ERRORSIGNAL, this, &DownloadPart::handleNetworkError);
    connect(m_reply, &QNetworkReply::finished, this, &DownloadPart::startPremiumizeMeDownload);

}

void DownloadPart::startPremiumizeMeDownload()
{
    Qp::update(Qp::sharedFrom(this));

    QJsonDocument document = QJsonDocument::fromJson(m_reply->readAll());
    QVariantMap documentVariant = document.toVariant().toMap();
    QVariantMap result = documentVariant.value("result").toMap();

    // Redirection
    QUrl downloadUrl = result.value("location").toUrl();
    if(!downloadUrl.isValid())
        downloadUrl = url();
    else
        qDebug() << "REDIRECT" << downloadUrl;

    QNetworkRequest request = QNetworkRequest(downloadUrl);
    request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    if(m_bytesWritten > 0) {
        QByteArray data;
        QString strData("bytes=" + QString::number(m_bytesWritten) + "-");

        data = strData.toLatin1();
        request.setRawHeader("Range", data);
    }

    m_reply->deleteLater();
    m_reply = qApp->networkAccessManager()->get(request);
    m_reply->setReadBufferSize(0);

    qDebug() << "GET" << downloadUrl;
    if(!m_buffer)
        m_buffer = new char[BUFFERSIZE];

    connect(m_reply, &QNetworkReply::readyRead, this, &DownloadPart::readBytes);
    connect(m_reply, &QNetworkReply::finished, this, &DownloadPart::finishDownload);
    connect(m_reply, ERRORSIGNAL, this, &DownloadPart::handleNetworkError);
}

void DownloadPart::readBytes()
{
    if(fileSize() == 0) {
        setFileSize(bytesWritten() + m_reply->header(QNetworkRequest::ContentLengthHeader).toInt());
    }

    qint64 bytesAvailable = m_reply->bytesAvailable();
    if (bytesAvailable < BUFFERSIZE)
        return;

    if (!m_file) {
        m_file = new QFile(fileName());
        qDebug() << "OPENING FILE" << m_file->fileName();
        if (!m_file->open(QIODevice::Append)) {
            qDebug() << (QString("Could not write to file '%1': %2")
                         .arg(m_file->fileName())
                         .arg(m_file->errorString()));
            stopDownload();
            return;
        }
    }

    qint64 read = m_reply->read(m_buffer, BUFFERSIZE);
    if (read == -1) {
        qDebug() << (QString("Could not read from network: %2")
                     .arg(m_reply->errorString()));
        stopDownload();
        return;
    }

    qint64 write = m_file->write(m_buffer, read);
    if (write == -1) {
        qDebug() << (QString("Could not write to file '%1': %2")
                     .arg(m_file->fileName())
                     .arg(m_file->errorString()));
        stopDownload();
        return;
    }
    Q_ASSERT(read == write);

    increaseBytesWritten(write);
    Qp::update(Qp::sharedFrom(this));
}

void DownloadPart::finishDownload()
{
    int bytesAvailable = m_reply->bytesAvailable();
    if(bytesAvailable > 0 && m_reply->error() == QNetworkReply::NoError) {
        qint64 write = m_file->write(m_reply->readAll());
        Q_ASSERT(bytesAvailable == write);
        increaseBytesWritten(write);
        Q_ASSERT(bytesWritten() == fileSize());
        Qp::update(Qp::sharedFrom(this));
    }

    qDebug() << "FINISHED" << fileName();
    cleanup();

    if(bytesWritten() == fileSize()) {
        extractIfComplete();
        emit finished();
    }
}

void DownloadPart::extractIfComplete()
{
    qApp->extraction()->extractIfComplete(package());
}
