#include "downloads.h"

#include <model/downloadpart.h>
#include <preferences.h>
#include <application.h>

Downloads::Downloads(QObject *parent) :
    QObject(parent),
    m_runningCount(0)
{
    connect(qApp, &QApplication::aboutToQuit, this, &Downloads::stop);
}

Downloads::~Downloads()
{
}

Downloads::State Downloads::state() const
{
    return m_state;
}

void Downloads::toggle()
{
    if(m_state == Started)
        stop();
    else
        start();
}

void Downloads::start()
{
    startMoreDownloads();
}

void Downloads::startMoreDownloads()
{
    int max = Preferences::maxDownloads();

    foreach(QSharedPointer<DownloadPart> part, Qp::readAll<DownloadPart>()) {
        if(m_runningCount == max)
            break;

        if(part->isFinished() || part->isRunning())
            continue;

        connect(part.data(), &DownloadPart::finished, this, &Downloads::finishDownload);
        part->startDownload();
        ++m_runningCount;
    }

    if(m_runningCount == 0)
        setState(Stopped);
    else
        setState(Started);
}

void Downloads::finishDownload()
{
    DownloadPart *part = static_cast<DownloadPart *>(sender());
    disconnect(part, 0, this, 0);
    --m_runningCount;

    startMoreDownloads();
}

void Downloads::stop()
{
    foreach(QSharedPointer<DownloadPart> part, Qp::readAll<DownloadPart>()) {
        part->stopDownload();
    }

    m_runningCount = 0;
    setState(Stopped);
}

void Downloads::setState(Downloads::State arg)
{
    if (m_state != arg) {
        m_state = arg;
        emit stateChanged(arg);
    }
}
