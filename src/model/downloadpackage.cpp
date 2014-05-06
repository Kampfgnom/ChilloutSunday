#include "downloadpackage.h"

#include "downloadpart.h"

#include <qserienjunkies.h>

#include <QPixmap>
#include <QUrl>

DownloadPackage::DownloadPackage(QObject *parent) :
    QObject(parent),
    m_status(UnknownStatus),
    m_parts(QpRelation(&DownloadPackage::parts)),
    m_serienJunkiesReply(nullptr)
{
}

DownloadPackage::Status DownloadPackage::status() const
{
    return m_status;
}

QString DownloadPackage::statusMessage() const
{
    return m_statusMessage;
}

QUrl DownloadPackage::sourceUrl() const
{
    return m_sourceUrl;
}

QString DownloadPackage::title() const
{
    return m_title;
}

QPixmap DownloadPackage::captchaPixmap() const
{
    return m_captcha;
}

QString DownloadPackage::displayString() const
{
    if(!title().isEmpty())
        return title();

    return sourceUrl().toString();
}

QList<QSharedPointer<DownloadPart> > DownloadPackage::parts() const
{
    return m_parts;
}

void DownloadPackage::decrypt()
{
    m_serienJunkiesReply = QSerienJunkies::decrypt(sourceUrl());
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::error, this, &DownloadPackage::handleSerienjunkiesReplyError);
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::requiresCaptcha, this, &DownloadPackage::requestSerienjunkiesCaptcha);
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::finished, this, &DownloadPackage::parseSerienjunkiesDecryptReply);

    setStatusMessage(tr("Decrypting"));
    setStatus(Decrypting);
    Qp::update(Qp::sharedFrom(this));
}

void DownloadPackage::handleSerienjunkiesReplyError()
{
    setStatusMessage(tr("Error: %1").arg(m_serienJunkiesReply->errorString()));
    setStatus(Error);
    Qp::update(Qp::sharedFrom(this));
}

void DownloadPackage::requestSerienjunkiesCaptcha()
{
    QPixmap pm;
    pm.loadFromData(m_serienJunkiesReply->captcha(), "PNG");
    setCaptchaPixmap(pm);
    setTitle(m_serienJunkiesReply->packageName());
    setStatusMessage(tr("Requires captcha"));
    setStatus(RequiresCaptcha);
    Qp::update(Qp::sharedFrom(this));
}

void DownloadPackage::solveCaptcha(const QString &captchaString)
{
    m_serienJunkiesReply->solveCaptcha(captchaString);
}

void DownloadPackage::parseSerienjunkiesDecryptReply()
{
    foreach(QUrl url, m_serienJunkiesReply->urls()) {
        QSharedPointer<DownloadPart> part = Qp::create<DownloadPart>();
        part->setUrl(url);
        addPart(part);
        Qp::update(part);
    }

    setStatus(Decrypted);
    setCaptchaPixmap(QPixmap());
    setStatusMessage(tr("Decrypted"));
    Qp::update(Qp::sharedFrom(this));
    m_serienJunkiesReply.clear();
}

void DownloadPackage::setSourceUrl(QUrl arg)
{
    if (m_sourceUrl != arg) {
        m_sourceUrl = arg;
        emit sourceUrlChanged(arg);
    }
}

void DownloadPackage::setTitle(QString arg)
{
    if (m_title != arg) {
        m_title = arg;
        emit titleChanged(arg);
    }
}

void DownloadPackage::setStatus(DownloadPackage::Status arg)
{
    if (m_status != arg) {
        m_status = arg;
        emit statusChanged(arg);
    }
}

void DownloadPackage::setStatusMessage(QString arg)
{
    if (m_statusMessage != arg) {
        m_statusMessage = arg;
        emit statusMessageChanged(arg);
    }
}

void DownloadPackage::setCaptchaPixmap(QPixmap arg)
{
    m_captcha = arg;
    emit captchaPixmapChanged(arg);
}

bool DownloadPackage::isPackageUrl(const QUrl &url)
{
    return (url.host() == "download.serienjunkies.org");
}

QSharedPointer<DownloadPackage> DownloadPackage::decrypt(const QUrl &url)
{
    QSharedPointer<DownloadPackage> package = Qp::create<DownloadPackage>();
    package->setSourceUrl(url);
    package->setStatus(NewPackage);
    Qp::update(package);
    package->decrypt();
    return package;
}

void DownloadPackage::setParts(QList<QSharedPointer<DownloadPart> > arg)
{
    m_parts = arg;
    emit partsChanged(arg);
}

void DownloadPackage::addPart(QSharedPointer<DownloadPart> arg)
{
    m_parts.add(arg);
    emit partsChanged(parts());
}

void DownloadPackage::removePart(QSharedPointer<DownloadPart> arg)
{
    m_parts.remove(arg);
    emit partsChanged(parts());
}
