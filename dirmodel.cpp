#include "dirmodel.h"

namespace memory {

DirModel::DirModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , _model(this)
{
    _model.setFilter(QDir::NoDotAndDotDot|QDir::AllDirs);
    _model.setResolveSymlinks(true);
    setSourceModel(&_model);
}

QVariant DirModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DecorationRole) {
        return QVariant();
    }

    return QSortFilterProxyModel::data(index, role);
}

int DirModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool DirModel::hasChildren(const QModelIndex &parent) const
{
    // return false if item cant have children
    if (parent.flags() & Qt::ItemNeverHasChildren) {
        return false;
    }
    // return if at least one child exists
    return QDirIterator(    filePath(parent),
                            filter() | QDir::NoDotAndDotDot,
                            QDirIterator::NoIteratorFlags
                        ).hasNext();
}

QVariant DirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);

    return QVariant();
}

} // namespace memory