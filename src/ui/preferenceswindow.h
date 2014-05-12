#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#include <QMainWindow>

namespace Ui {
class PreferencesWindow;
}

class QItemSelection;

class PreferencesWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PreferencesWindow(QWidget *parent = 0);
    ~PreferencesWindow();

private slots:
    void on_lineEditUserName_editingFinished();
    void on_lineEditPassword_editingFinished();
    void on_lineEditDownloadFolder_editingFinished();
    void on_pushButtonChooseDownloadFolder_clicked();
    void on_spinBoxMaxDownloads_editingFinished();
    void on_actionClose_triggered();

private:
    Ui::PreferencesWindow *ui;
};

#endif // PREFERENCESWINDOW_H
