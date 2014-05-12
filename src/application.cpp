#include "application.h"

#include "database.h"
#include "controller/downloads.h"
#include "model/downloadpart.h"
#include "ui/mainwindow.h"

#include <qserienjunkies.h>

#include <QNetworkAccessManager>
#include <QTimer>

const char *ARG_CLEANDB("--cleanDatabase");

Application::Application(int &argc, char **argv, int flags) :
    QApplication(argc, argv, flags),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_database(new Database(this)),
    m_downloads(new Downloads(this)),
    m_mainWindow(nullptr)
{
    setApplicationName("ChilloutSunday");
    setOrganizationName("LB Productions");
    setOrganizationDomain("github.com/Kampfgnom");
    setApplicationVersion(APP_VERSION);
    setAttribute(Qt::AA_DontShowIconsInMenus, true);

    QSerienJunkies::setNetworkAccessManager(m_networkAccessManager);
}

int Application::exec()
{
    bool cleanSchema = arguments().contains(ARG_CLEANDB);
    if(!m_database->open(cleanSchema))
        return -1;

    m_mainWindow = new MainWindow;
    QTimer::singleShot(0, m_mainWindow, SLOT(show()));
    return QApplication::exec();
}

QNetworkAccessManager *Application::networkAccessManager() const
{
    return m_networkAccessManager;
}

Database *Application::database() const
{
    return m_database;
}

Downloads *Application::downloads() const
{
    return m_downloads;
}

MainWindow *Application::mainWindow() const
{
    return m_mainWindow;
}
