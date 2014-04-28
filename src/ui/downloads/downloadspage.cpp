#include "downloadspage.h"
#include "ui_downloadspage.h"

#include "packagesmodel.h"
#include "packagedelegate.h"

#include <model/package.h>

#include <QClipboard>
#include <QTimer>

DownloadsPage::DownloadsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadsPage)
{
    ui->setupUi(this);

    ui->treeViewDownloads->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->treeViewDownloads->setModel(new PackagesSortFilterModel(ui->treeViewDownloads));
    ui->treeViewDownloads->setItemDelegate(new PackageDelegate(ui->treeViewDownloads));

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

    if(!Package::isPackageUrl(url))
        return;

    QSharedPointer<Package> package = Package::createPackage(url);
    package->decrypt();
}
