#include "extraction.h"

#include <model/downloadpackage.h>
#include <model/downloadpart.h>

#include <QuunRar.h>
#include <QuunRarJob.h>

Extraction::Extraction(QObject *parent) :
    QObject(parent)
{
}

void Extraction::extractIfComplete(QSharedPointer<DownloadPackage> package)
{
    if(!package->isDownloadFinished())
        return;

    qDebug() << "EXTRACT: " << package->parts().first()->fileName();

    QuunRar *unrar = new QuunRar(package->parts().first()->fileName(), this);
    QuunRarJob *job = unrar->extract();

    connect(job, &QuunRarJob::error, this, &Extraction::handleError);
    connect(job, &QuunRarJob::dataProcessed, this, &Extraction::handleProcessedData);
    connect(job, &QuunRarJob::finished, this, &Extraction::handleFinished);
    connect(job, &QuunRarJob::volumeChanged, this, &Extraction::handleChangedVolume);
    connect(job, &QuunRarJob::volumeMissing, this, &Extraction::handleMissingVolume);
    connect(job, &QuunRarJob::currentFileChanged, this, &Extraction::handleFileChange);
}

void Extraction::handleError()
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    qWarning() << "UNRAR ERROR:" << job->archive()->errorString() <<
                  "=>" << job->archive()->fileName();
    job->archive()->deleteLater();
}

void Extraction::handleProcessedData()
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    qDebug() << "UNRAR EXTRACTED:" << job->processedData() << "of" << job->totalData() <<
                  "=>" << job->archive()->fileName();
}

void Extraction::handleFinished()
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    qDebug() << "UNRAR ERROR:" << job->archive()->errorString() <<
                  "=>" << job->archive()->fileName();
    job->archive()->deleteLater();
}

void Extraction::handleChangedVolume(const QString &volume)
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    qDebug() << "UNRAR VOLUME CHANGE:" << volume <<
                  "=>" << job->archive()->fileName();
}

void Extraction::handleMissingVolume(const QString &volume)
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    qWarning() << "UNRAR MISSING VOLUME:" << volume <<
                  "=>" << job->archive()->fileName();
    job->archive()->deleteLater();
}

void Extraction::handleFileChange(const QString &fileName)
{
    QuunRarJob *job = static_cast<QuunRarJob *>(sender());
    qDebug() << "UNRAR FILE:" << fileName <<
                  "=>" << job->archive()->fileName();
}
