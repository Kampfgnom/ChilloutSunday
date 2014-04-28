#ifndef PACKAGE_H
#define PACKAGE_H

#include <QPersistence.h>
#include <QUrl>
#include <QPixmap>

class QSerienJunkiesReply;

class Package : public QObject
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage WRITE setStatusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl WRITE setSourceUrl NOTIFY sourceUrlChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)

public:
    enum Status {
        UnknownStatus,
        NewPackage,
        Decrypting,
        RequiresCaptcha,
        Error
    };

    explicit Package(QObject *parent = 0);

    Status status() const;
    QString statusMessage() const;
    QUrl sourceUrl() const;
    QString title() const;
    QPixmap captchaPixmap() const;

    QString displayString() const;

    void decrypt();
    void solveCaptcha(const QString &captchaString);

    static bool isPackageUrl(const QUrl &url);
    static QSharedPointer<Package> createPackage(const QUrl &url);


signals:
    void sourceUrlChanged(QUrl arg);
    void titleChanged(QString arg);
    void statusChanged(Status arg);
    void statusMessageChanged(QString arg);
    void captchaPixmapChanged(QPixmap arg);

private slots:
    void parseSerienjunkiesDecryptReply();
    void requestSerienjunkiesCaptcha();
    void handleSerienjunkiesReplyError();

    void setSourceUrl(QUrl arg);
    void setTitle(QString arg);
    void setStatus(Status arg);
    void setStatusMessage(QString arg);
    void setCaptchaPixmap(QPixmap arg);

private:
    QUrl m_sourceUrl;
    QString m_title;
    Status m_status;
    QString m_statusMessage;
    QPixmap m_captcha;

    QSerienJunkiesReply *m_serienJunkiesReply;
};

#endif // PACKAGE_H
