#ifndef PACKAGESMODEL_H
#define PACKAGESMODEL_H

#include <QPersistence.h>
#include <model/downloadpackage.h>

class PackagesModel : public QpObjectListModel<DownloadPackage>
{
public:
    enum Roles {
        ObjectRole = Qt::UserRole + 1
    };

    PackagesModel(QObject *parent);

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
};

class PackagesSortFilterModel : public QpSortFilterProxyObjectModel<DownloadPackage>
{
public:
    PackagesSortFilterModel(QObject *parent);
};

#endif // PACKAGESMODEL_H
