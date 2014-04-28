#ifndef PACKAGEDELEGATE_H
#define PACKAGEDELEGATE_H

#include <QStyledItemDelegate>

class Package;

class PackageDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PackageDelegate(QWidget *parent = 0);

    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // PACKAGEDELEGATE_H
