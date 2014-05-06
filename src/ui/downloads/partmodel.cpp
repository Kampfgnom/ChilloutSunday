#include "partmodel.h"

PartsModel::PartsModel(QObject *parent) :
    QpObjectListModel<DownloadPart>(parent)
{
}

QVariant PartsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    QSharedPointer<DownloadPart> part = objectByIndex(index);

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case UrlColumn:
            return part->url();
        }
    }

    return QVariant();
}

QVariant PartsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation != Qt::Horizontal)
        return QVariant();

    if(role == Qt::DisplayRole) {
        switch(section) {
        case UrlColumn:
            return tr("URL");
        }
    }

    return QVariant();
}

/***************************************************************************
 * DownloadsSortFilterModel
 */
PartsSortFilterModel::PartsSortFilterModel(QObject *parent) :
    QpSortFilterProxyObjectModel<DownloadPart>(parent)
{
}

QSharedPointer<DownloadPackage> PartsSortFilterModel::filterPackage() const
{
    return m_filterPackage;
}

void PartsSortFilterModel::setFilterPackage(const QSharedPointer<DownloadPackage> &filterPackage)
{
    m_filterPackage = filterPackage;
    invalidateFilter();
}

bool PartsSortFilterModel::filterAcceptsObject(QSharedPointer<DownloadPart> object) const
{
    if(!object)
        return false;

    return (object->package() == m_filterPackage);
}

