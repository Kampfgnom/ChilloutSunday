#ifndef EXTRACTION_H
#define EXTRACTION_H

#include <QObject>

class DownloadPackage;

class Extraction : public QObject
{
    Q_OBJECT
public:
    explicit Extraction(QObject *parent = 0);

    void extractIfComplete(QSharedPointer<DownloadPackage> package);

private slots:
    void handleError();
    void handleProcessedData();
    void handleFinished();
    void handleChangedVolume(const QString &volume);
    void handleMissingVolume(const QString &volume);
    void handleFileChange(const QString &fileName);
};

#endif // EXTRACTION_H
