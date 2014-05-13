#include "downloadpackage.h"

#include "downloadpart.h"

#include <application.h>
#include <controller/downloads.h>

#include <qserienjunkies.h>

#include <QPixmap>
#include <QUrl>

bool DownloadPackage::isPackageUrl(const QUrl &url)
{
    return (url.host() == "download.serienjunkies.org");
}

QSharedPointer<DownloadPackage> DownloadPackage::decrypt(const QUrl &url)
{
    QSharedPointer<DownloadPackage> package = Qp::create<DownloadPackage>();
    package->setSourceUrl(url);
    Qp::update(package);
    package->decrypt();
    return package;
}

DownloadPackage::DownloadPackage(QObject *parent) :
    QObject(parent),
    m_parts(QpRelation(&DownloadPackage::parts)),
    m_serienJunkiesReply(nullptr)
{
}

QUrl DownloadPackage::sourceUrl() const
{
    return m_sourceUrl;
}

QPixmap DownloadPackage::captchaPixmap() const
{
    return m_captcha;
}

QList<QSharedPointer<DownloadPart> > DownloadPackage::parts() const
{
    return m_parts;
}

bool DownloadPackage::isDownloadFinished()
{
    if(parts().isEmpty())
        return false;

    foreach(QSharedPointer<DownloadPart> part, parts()) {
        if(!part->isFinished())
            return false;
    }

    return true;
}

void DownloadPackage::setSourceUrl(QUrl arg)
{
    if (m_sourceUrl != arg) {
        m_sourceUrl = arg;
        emit sourceUrlChanged(arg);
    }
}

void DownloadPackage::setCaptchaPixmap(QPixmap arg)
{
    m_captcha = arg;
    emit captchaPixmapChanged(arg);
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

void DownloadPackage::decrypt()
{
    m_serienJunkiesReply = QSerienJunkies::decrypt(sourceUrl());
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::error, this, &DownloadPackage::handleSerienjunkiesReplyError);
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::requiresCaptcha, this, &DownloadPackage::requestSerienjunkiesCaptcha);
    connect(m_serienJunkiesReply, &QSerienJunkiesReply::finished, this, &DownloadPackage::parseSerienjunkiesDecryptReply);

    Qp::update(Qp::sharedFrom(this));
}

void DownloadPackage::handleSerienjunkiesReplyError()
{
    qWarning() << (m_serienJunkiesReply->errorString());
    Qp::update(Qp::sharedFrom(this));
}

void DownloadPackage::requestSerienjunkiesCaptcha()
{
    QPixmap pm;
    pm.loadFromData(m_serienJunkiesReply->captcha(), "PNG");
    setCaptchaPixmap(pm);
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

    setCaptchaPixmap(QPixmap());
    Qp::update(Qp::sharedFrom(this));
    m_serienJunkiesReply.clear();

    qApp->downloads()->start();
}
