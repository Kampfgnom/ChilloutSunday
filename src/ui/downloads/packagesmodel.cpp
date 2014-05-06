#include "packagesmodel.h"

/***************************************************************************
 * DownloadsModel
 */
PackagesModel::PackagesModel(QObject *parent) :
    QpObjectListModel<DownloadPackage>(parent)
{
}

Qt::ItemFlags PackagesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QpObjectListModel<DownloadPackage>::flags(index);
    if(index.column() == 1)
        flags |= Qt::ItemIsEditable;

    return flags;
}

int PackagesModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return QpObjectListModel<DownloadPackage>::columnCount(parent) + 2;
}

QVariant PackagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QSharedPointer<DownloadPackage> package = objectByIndex(index);

    if(role == ObjectRole)
        return QVariant::fromValue<QSharedPointer<DownloadPackage> >(package);

    if(index.column() > 1)
        return QpObjectListModel<DownloadPackage>::data(index, role);


    if(role == Qt::DecorationRole) {
        if(index.column() == 0)
            return package->captchaPixmap();
    }

    return QVariant();
}

QVariant PackagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section > 1)
        return QpObjectListModel<DownloadPackage>::headerData(section, orientation, role);

    if(section == 0)
        return tr("Captcha");

    if(section == 1)
        return tr("Solve captcha");

    return QVariant();
}

/***************************************************************************
 * DownloadsSortFilterModel
 */
PackagesSortFilterModel::PackagesSortFilterModel(QObject *parent) :
    QpSortFilterProxyObjectModel(new PackagesModel(parent), parent)
{
}
