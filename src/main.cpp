#include <QApplication>

#include "ui/mainWindow.h"
#include <QTimer>

#include <MPMBaseModel.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("MPMBase");
    a.setOrganizationName("Geenen IT-Systeme");
    a.setOrganizationDomain("de.geenen-it-systeme.mpmbase");
    a.setAttribute(Qt::AA_DontShowIconsInMenus, true);
    a.setApplicationVersion(APP_VERSION);

    // Set this so that we can test on non-german systems (e.g. Niklas')
    QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));

    if(a.arguments().size() != 4) {
        qDebug() << "Usage: MPMBase <databasename> <username> <password>";
        return -1;
    }

    QString database = a.arguments().at(1);
    QString username = a.arguments().at(2);
    QString password = a.arguments().at(3);

    MPMDatabase::instance()->setDatabase(database);
    MPMDatabase::instance()->setLoginName(username);
    MPMDatabase::instance()->setPassword(password);

    Q_INIT_RESOURCE(libGUI);

    QTimer::singleShot(0, MainWindow::instance(), SLOT(show()));

    int ret = a.exec();
    delete MainWindow::instance();
    return ret;
}
