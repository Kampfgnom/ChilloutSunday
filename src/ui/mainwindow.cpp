#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow *MainWindow::s_instance = nullptr;

MainWindow *MainWindow::instance()
{
    if(s_instance)
        return s_instance;

    s_instance = new MainWindow;
    return s_instance;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

MainWindow::~MainWindow()
{
    delete ui;
}
