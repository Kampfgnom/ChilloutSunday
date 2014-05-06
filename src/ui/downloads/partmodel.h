#ifndef PARTMODEL_H
#define PARTMODEL_H

#include <QPersistence.h>
#include <model/downloadpart.h>

class PartsModel : public QpObjectListModel<DownloadPart>
{
    Q_OBJECT
public:
    enum Columns {
        UrlColumn
    };

    explicit PartsModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;
};

class PartsSortFilterModel : public QpSortFilterProxyObjectModel<DownloadPart>
{
public:
    PartsSortFilterModel(QObject *parent);

    QSharedPointer<DownloadPackage> filterPackage() const;
    void setFilterPackage(const QSharedPointer<DownloadPackage> &filterPackage);

    bool filterAcceptsObject(QSharedPointer<DownloadPart> object) const Q_DECL_OVERRIDE;

private:
    QSharedPointer<DownloadPackage> m_filterPackage;
};

#endif // PARTMODEL_H
