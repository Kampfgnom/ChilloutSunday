#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QApplication>
#include <QSharedPointer>

class QNetworkAccessManager;
class DownloadPart;
class Database;
class Downloads;
class MainWindow;

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<Application *>(QCoreApplication::instance()))

class Application : public QApplication
{
public:
    Application(int &argc, char **argv, int flags = ApplicationFlags);
    int exec();

    QNetworkAccessManager *networkAccessManager() const;
    Database *database() const;
    Downloads *downloads() const;
    MainWindow *mainWindow() const;

private:
    QNetworkAccessManager *m_networkAccessManager;
    Database *m_database;
    Downloads *m_downloads;
    MainWindow *m_mainWindow;
};

#endif // CONTROLLER_H
