#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QApplication>
#include <QSharedPointer>

class QNetworkAccessManager;
class DownloadPart;
class Database;
class Downloads;
class MainWindow;
class Extraction;

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<Application *>(QCoreApplication::instance()))

class Application : public QApplication
{
public:
    Application(int &argc, char **argv, int flags = ApplicationFlags);
    int exec();

    MainWindow *mainWindow() const;
    QNetworkAccessManager *networkAccessManager() const;
    Database *database() const;
    Downloads *downloads() const;
    Extraction *extraction() const;

private:
    MainWindow *m_mainWindow;
    QNetworkAccessManager *m_networkAccessManager;
    Database *m_database;
    Downloads *m_downloads;
    Extraction *m_extraction;
};

#endif // CONTROLLER_H
