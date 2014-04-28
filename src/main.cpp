#include <QApplication>

#include <database.h>
#include <ui/mainwindow.h>
#include <QTimer>

const char *ARG_CLEANDB("--cleanDatabase");

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("ChilloutSunday");
    a.setOrganizationName("LB Productions");
    a.setOrganizationDomain("github.com/Kampfgnom");
    a.setAttribute(Qt::AA_DontShowIconsInMenus, true);
    a.setApplicationVersion(APP_VERSION);

    bool cleanSchema = a.arguments().contains(ARG_CLEANDB);
    Database db;
    if(!db.open(cleanSchema))
        return -1;

    QTimer::singleShot(0, MainWindow::instance(), SLOT(show()));
    return a.exec();
}
