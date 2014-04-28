#include "package.h"

#include <qserienjunkies.h>

#include <QPixmap>
#include <QUrl>

Package::Package(QObject *parent) :
    QObject(parent),
    m_status(UnknownStatus),
    m_serienJunkiesReply(nullptr)
{
}

Package::Status Package::status() const
{
    return m_status;
}

QString Package::statusMessage() const
{
    return m_statusMessage;
}

QUrl Package::sourceUrl() const
{
    return m_sourceUrl;
}

QString Package::title() const
{
    return m_title;
}

QPixmap Package::captchaPixmap() const
{
    return m_captcha;
}

QString Package::displayString() const
{
    if(!title().isEmpty())
        return title();

    return sourceUrl().toString();
}

void Package::decrypt()
{
    if(status() != NewPackage)
        return;

    if(m_serienJunkiesReply)
        delete m_serienJunkiesReply;

    setStatusMessage(tr("Decrypting"));
    setStatus(Decrypting);

    m_serienJunkiesReply = QSerienJunkies::decrypt(sourceUrl());
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::error, this, &Package::handleSerienjunkiesReplyError);
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::requiresCaptcha, this, &Package::requestSerienjunkiesCaptcha);
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::finished, this, &Package::parseSerienjunkiesDecryptReply);
}

void Package::handleSerienjunkiesReplyError()
{
    setStatusMessage(tr("Error: %1").arg(m_serienJunkiesReply->errorString()));
    setStatus(Error);
}

void Package::requestSerienjunkiesCaptcha()
{
    QPixmap pm;
    pm.loadFromData(m_serienJunkiesReply->captcha(), "PNG");
    setCaptchaPixmap(pm);
    setTitle(m_serienJunkiesReply->packageName());
    setStatusMessage(tr("Requires captcha"));
    setStatus(RequiresCaptcha);
}

void Package::solveCaptcha(const QString &captchaString)
{
    m_serienJunkiesReply->solveCaptcha(captchaString);
}

void Package::parseSerienjunkiesDecryptReply()
{
    qDebug() << m_serienJunkiesReply->urls();
}

void Package::setSourceUrl(QUrl arg)
{
    if (m_sourceUrl != arg) {
        m_sourceUrl = arg;
        emit sourceUrlChanged(arg);
    }
}

void Package::setTitle(QString arg)
{
    if (m_title != arg) {
        m_title = arg;
        emit titleChanged(arg);
    }
}

void Package::setStatus(Package::Status arg)
{
    if (m_status != arg) {
        m_status = arg;
        emit statusChanged(arg);
    }
}

void Package::setStatusMessage(QString arg)
{
    if (m_statusMessage != arg) {
        m_statusMessage = arg;
        emit statusMessageChanged(arg);
    }
}

void Package::setCaptchaPixmap(QPixmap arg)
{
    m_captcha = arg;
    emit captchaPixmapChanged(arg);
}

bool Package::isPackageUrl(const QUrl &url)
{
    return (url.host() == "download.serienjunkies.org");
}

QSharedPointer<Package> Package::createPackage(const QUrl &url)
{
    QSharedPointer<Package> package = Qp::create<Package>();
    package->setSourceUrl(url);
    package->setStatus(NewPackage);
    Qp::update(package);
    return package;
}
