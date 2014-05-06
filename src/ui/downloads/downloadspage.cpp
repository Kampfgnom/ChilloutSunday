#include "downloadspage.h"
#include "ui_downloadspage.h"

#include "packagesmodel.h"
#include "packagedelegate.h"

#include <model/downloadpackage.h>

#include <QClipboard>
#include <QTimer>

DownloadsPage::DownloadsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadsPage)
{
    ui->setupUi(this);

    PackagesSortFilterModel *m_modelPackages = new PackagesSortFilterModel(this);

    ui->treeViewPackages->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeViewPackages->setModel(m_modelPackages);
    ui->treeViewPackages->setItemDelegate(new PackageDelegate(this));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DownloadsPage::parseClipboard);
    timer->setInterval(1000);
    timer->start();
}

DownloadsPage::~DownloadsPage()
{
    delete ui;
}

void DownloadsPage::parseClipboard()
{
    static QString oldClipboard;
    QString clipboard = QApplication::clipboard()->text();
    if (clipboard.isEmpty() || clipboard == oldClipboard)
        return;

    oldClipboard = clipboard;

    QUrl url(clipboard);
    if (!url.isValid())
        return;

    if(!DownloadPackage::isPackageUrl(url))
        return;

    QSharedPointer<DownloadPackage> package = DownloadPackage::createPackage(url);
    package->decrypt();
}

void DownloadsPage::on_treeViewPackages_clicked(const QModelIndex &index)
{

}
