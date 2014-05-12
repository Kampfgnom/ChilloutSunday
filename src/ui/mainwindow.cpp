#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "preferenceswindow.h"

#include <application.h>
#include <controller/downloads.h>
#include <model/downloadpackage.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(qApp->downloads(), &Downloads::stateChanged, this, &MainWindow::reflectDownloadsState);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesWindow *w = nullptr;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        if((w = qobject_cast<PreferencesWindow *>(widget)))
            break;
    }
    if(!w)
        w = new PreferencesWindow;

    w->show();
}

void MainWindow::on_actionClose_triggered()
{
    close();
}

void MainWindow::on_actionToggleDownloads_triggered()
{
    qApp->downloads()->toggle();
}

void MainWindow::reflectDownloadsState()
{
    if(qApp->downloads()->state() == Downloads::Started)
        ui->actionToggleDownloads->setText(tr("Stop"));
    else
        ui->actionToggleDownloads->setText(tr("Start"));
}
