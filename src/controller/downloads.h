#ifndef DOWNLOADS_H
#define DOWNLOADS_H

#include <QObject>

class Downloads : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_PROPERTY(State state READ state WRITE setState NOTIFY stateChanged)

public:
    enum State {
        Stopped,
        Started
    };

    explicit Downloads(QObject *parent = 0);
    ~Downloads();

    State state() const;

public slots:
    void toggle();
    void start();
    void stop();

private slots:
    void setState(State arg);
    void startMoreDownloads();
    void finishDownload();

signals:
    void stateChanged(State arg);

private:
    State m_state;
    int m_runningCount;
};

#endif // DOWNLOADS_H
