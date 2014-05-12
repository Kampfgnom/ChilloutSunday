#ifndef DOWNLOADPART_H
#define DOWNLOADPART_H

#include <QPersistence.h>

#include <QFile>
#include <QNetworkReply>
#include <QUrl>
#include <QPointer>

class DownloadPackage;

class DownloadPart : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QSharedPointer<DownloadPackage> package READ package WRITE setPackage NOTIFY packageChanged)
    Q_PROPERTY(quint64 fileSize READ fileSize WRITE setFileSize NOTIFY fileSizeChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(quint64 bytesWritten READ bytesWritten WRITE setBytesWritten NOTIFY bytesWrittenChanged)

public:
    explicit DownloadPart(QObject *parent = 0);
    ~DownloadPart();

    QUrl url() const;
    QSharedPointer<DownloadPackage> package() const;

    quint64 fileSize() const;
    QString fileName() const;
    quint64 bytesWritten() const;
    bool isRunning() const;
    bool isFinished() const;

public slots:
    void startDownload();
    void stopDownload();

signals:
    void packageChanged(QSharedPointer<DownloadPackage> arg);
    void urlChanged(QUrl arg);
    void fileSizeChanged(quint64 arg);
    void fileNameChanged(QString arg);
    void bytesWrittenChanged(quint64 arg);

    void finished();

private slots:
    void setUrl(QUrl arg);
    void setPackage(QSharedPointer<DownloadPackage> arg);
    void setFileSize(quint64 arg);
    void setFileName(QString arg);
    void setBytesWritten(quint64 arg);
    void increaseBytesWritten(quint64 writtenBytes);

    void handleNetworkError();
    void parseFileInfo();
    void startPremiumizeMeDownload();
    void cleanup();
    void readBytes();
    void finishDownload();

private:
    friend class DownloadPackage;

    QUrl m_url;
    QpBelongsToOne<DownloadPackage> m_package;
    bool m_running;
    QNetworkReply *m_reply;
    quint64 m_fileSize;
    QString m_fileName;
    QFile *m_file;
    char *m_buffer;
    quint64 m_bytesWritten;

    QElapsedTimer m_updateTimer;
};

#endif // DOWNLOADPART_H
