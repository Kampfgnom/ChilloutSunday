#ifndef DOWNLOADSPAGE_H
#define DOWNLOADSPAGE_H

#include <QWidget>

namespace Ui {
class DownloadsPage;
}

class PackagesSortFilterModel;

class DownloadsPage : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadsPage(QWidget *parent = 0);
    ~DownloadsPage();

private slots:
    void parseClipboard();

    void on_treeViewPackages_clicked(const QModelIndex &index);

private:
    Ui::DownloadsPage *ui;
    PackagesSortFilterModel *m_modelPackages;
};

#endif // DOWNLOADSPAGE_H
