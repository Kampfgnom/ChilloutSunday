#include "packagedelegate.h"

#include "packagesmodel.h"

#include <QPainter>
#include <QLineEdit>

PackageDelegate::PackageDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{
}

void PackageDelegate::setModelData(QWidget *editor, QAbstractItemModel *, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit *>(editor);
    QString text = lineEdit->text();

    QSharedPointer<DownloadPackage> package = index.data(PackagesModel::ObjectRole).value<QSharedPointer<DownloadPackage> >();
    package->solveCaptcha(text);
}

QSize PackageDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(70);
    return size;
}

void PackageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();

    QStyledItemDelegate::paint(painter, option, index);

    //    QRect rect = option.rect.adjusted(MARGIN, MARGIN, -MARGIN, -MARGIN);

    //    QSharedPointer<Package> package = index.data(PackagesModel::ObjectRole).value<QSharedPointer<Package> >();

    //    painter->drawText(rect, package->displayString());
    //    painter->drawRoundedRect(rect, RECT_RADIUS, RECT_RADIUS);

    painter->restore();
}
