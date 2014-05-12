#ifndef DOWNLOADPACKAGE_H
#define DOWNLOADPACKAGE_H

#include <QPersistence.h>
#include <QUrl>
#include <QPixmap>
#include <QPointer>

class QSerienJunkiesReply;
class DownloadPart;

class DownloadPackage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl sourceUrl READ sourceUrl WRITE setSourceUrl NOTIFY sourceUrlChanged)
    Q_PROPERTY(QList<QSharedPointer<DownloadPart> > parts READ parts WRITE setParts NOTIFY partsChanged)

public:
    explicit DownloadPackage(QObject *parent = 0);

    QUrl sourceUrl() const;
    QPixmap captchaPixmap() const;
    QList<QSharedPointer<DownloadPart> > parts() const;

    static bool isPackageUrl(const QUrl &url);
    static QSharedPointer<DownloadPackage> decrypt(const QUrl &url);

public slots:
    void solveCaptcha(const QString &captchaString);

signals:
    void sourceUrlChanged(QUrl arg);
    void captchaPixmapChanged(QPixmap arg);
    void partsChanged(QList<QSharedPointer<DownloadPart> > arg);

private slots:
    void decrypt();
    void parseSerienjunkiesDecryptReply();
    void requestSerienjunkiesCaptcha();
    void handleSerienjunkiesReplyError();

    void setSourceUrl(QUrl arg);
    void setCaptchaPixmap(QPixmap arg);

    void setParts(QList<QSharedPointer<DownloadPart> > arg);
    void addPart(QSharedPointer<DownloadPart> arg);
    void removePart(QSharedPointer<DownloadPart> arg);

private:
    QUrl m_sourceUrl;
    QPixmap m_captcha;

    QpHasMany<DownloadPart> m_parts;

    QPointer<QSerienJunkiesReply> m_serienJunkiesReply;
};

#endif // DOWNLOADPACKAGE_H
