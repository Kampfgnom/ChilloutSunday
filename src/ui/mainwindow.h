#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    static MainWindow *instance();
    ~MainWindow();

private:
    explicit MainWindow(QWidget *parent = 0);

    static MainWindow *s_instance;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
