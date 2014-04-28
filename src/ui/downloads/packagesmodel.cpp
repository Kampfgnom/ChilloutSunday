#include "packagesmodel.h"

/***************************************************************************
 * DownloadsModel
 */
PackagesModel::PackagesModel(QObject *parent) :
    QpObjectListModel<Package>(parent)
{
}

Qt::ItemFlags PackagesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QpObjectListModel<Package>::flags(index);
    if(index.column() == 1)
        flags |= Qt::ItemIsEditable;

    return flags;
}

int PackagesModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return QpObjectListModel<Package>::columnCount(parent) + 2;
}

QVariant PackagesModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QSharedPointer<Package> package = objectByIndex(index);

    if(role == ObjectRole)
        return QVariant::fromValue<QSharedPointer<Package> >(package);

    if(index.column() > 1)
        return QpObjectListModel<Package>::data(index, role);


    if(role == Qt::DecorationRole) {
        if(index.column() == 0)
            return package->captchaPixmap();
    }

    return QVariant();
}

QVariant PackagesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section > 1)
        return QpObjectListModel<Package>::headerData(section, orientation, role);

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
