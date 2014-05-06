#include "downloadpart.h"

#include "downloadpackage.h"

DownloadPart::DownloadPart(QObject *parent) :
    QObject(parent),
    m_package(QpRelation(&DownloadPart::package))
{
}

QUrl DownloadPart::url() const
{
    return m_url;
}

QSharedPointer<DownloadPackage> DownloadPart::package() const
{
    return m_package;
}

void DownloadPart::setUrl(QUrl arg)
{
    if (m_url != arg) {
        m_url = arg;
        emit urlChanged(arg);
    }
}

void DownloadPart::setPackage(QSharedPointer<DownloadPackage> arg)
{
    m_package = arg;
    emit packageChanged(arg);
}
