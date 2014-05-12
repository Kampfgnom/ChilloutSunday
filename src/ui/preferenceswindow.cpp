#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

#include <preferences.h>

#include <QFileDialog>
#include <QMessageBox>

PreferencesWindow::PreferencesWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PreferencesWindow)
{
    ui->setupUi(this);

    ui->lineEditUserName->setText(Preferences::premiumizeMeUserName());
    ui->lineEditPassword->setText(Preferences::premiumizeMeUserPassword());
    ui->lineEditDownloadFolder->setText(Preferences::downloadFolder());
    ui->spinBoxMaxDownloads->setValue(Preferences::maxDownloads());

    setAttribute(Qt::WA_DeleteOnClose, true);
}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::on_lineEditUserName_editingFinished()
{
    Preferences::setPremiumizeMeUserName(ui->lineEditUserName->text());
}

void PreferencesWindow::on_lineEditPassword_editingFinished()
{
    Preferences::setPremiumizeMeUserPassword(ui->lineEditPassword->text());
}

void PreferencesWindow::on_lineEditDownloadFolder_editingFinished()
{
    Preferences::setDownloadFolder(ui->lineEditDownloadFolder->text());
}

void PreferencesWindow::on_pushButtonChooseDownloadFolder_clicked()
{
    QString folder = QFileDialog::getExistingDirectory(this);
    if (folder.isEmpty())
        return;

    ui->lineEditDownloadFolder->setText(folder);
    Preferences::setDownloadFolder(folder);
}

void PreferencesWindow::on_spinBoxMaxDownloads_editingFinished()
{
    Preferences::setMaxDownloads(ui->spinBoxMaxDownloads->value());
}

void PreferencesWindow::on_actionClose_triggered()
{
    close();
}
