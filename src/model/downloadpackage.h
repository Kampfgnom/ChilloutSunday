#ifndef DOWNLOADPACKAGE_H
#define DOWNLOADPACKAGE_H

#include <QPersistence.h>
#include <QUrl>
#include <QPixmap>

class QSerienJunkiesReply;
class DownloadPart;

class DownloadPackage : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl WRITE setSourceUrl NOTIFY sourceUrlChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

    Q_PROPERTY(QList<QSharedPointer<DownloadPart> > parts READ parts WRITE setParts NOTIFY partsChanged)

public:
    enum Status {
        UnknownStatus,
        NewPackage,
        Decrypting,
        RequiresCaptcha,
        Error
    };

    explicit DownloadPackage(QObject *parent = 0);

    Status status() const;
    QString statusMessage() const;
    QUrl sourceUrl() const;
    QString title() const;
    QPixmap captchaPixmap() const;

    QString displayString() const;

    QList<QSharedPointer<DownloadPart> > parts() const;

    void decrypt();
    void solveCaptcha(const QString &captchaString);

    static bool isPackageUrl(const QUrl &url);
    static QSharedPointer<DownloadPackage> createPackage(const QUrl &url);

signals:
    void sourceUrlChanged(QUrl arg);
    void titleChanged(QString arg);
    void statusChanged(Status arg);
    void statusMessageChanged(QString arg);
    void captchaPixmapChanged(QPixmap arg);

    void partsChanged(QList<QSharedPointer<DownloadPart> > arg);

private slots:
    void parseSerienjunkiesDecryptReply();
    void requestSerienjunkiesCaptcha();
    void handleSerienjunkiesReplyError();

    void setSourceUrl(QUrl arg);
    void setTitle(QString arg);
    void setStatus(Status arg);
    void setStatusMessage(QString arg);
    void setCaptchaPixmap(QPixmap arg);

    void setParts(QList<QSharedPointer<DownloadPart> > arg);
    void addPart(QSharedPointer<DownloadPart> arg);
    void removePart(QSharedPointer<DownloadPart> arg);

private:
    QUrl m_sourceUrl;
    QString m_title;
    Status m_status;
    QString m_statusMessage;
    QPixmap m_captcha;

    QSerienJunkiesReply *m_serienJunkiesReply;
    QpHasMany<DownloadPart> m_parts;
};

#endif // DOWNLOADPACKAGE_H
