#ifndef DOWNLOADPART_H
#define DOWNLOADPART_H

#include <QPersistence.h>

#include <QUrl>

class DownloadPackage;

class DownloadPart : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QSharedPointer<DownloadPackage> package READ package WRITE setPackage NOTIFY packageChanged)

public:
    explicit DownloadPart(QObject *parent = 0);

    QUrl url() const;
    QSharedPointer<DownloadPackage> package() const;

private slots:
    void setUrl(QUrl arg);
    void setPackage(QSharedPointer<DownloadPackage> arg);

signals:
    void packageChanged(QSharedPointer<DownloadPackage> arg);
    void urlChanged(QUrl arg);

private:
    friend class DownloadPackage;

    QUrl m_url;
    QpBelongsToOne<DownloadPackage> m_package;
};

#endif // DOWNLOADPART_H
