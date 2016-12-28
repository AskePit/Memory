#include "dirmodel.h"

namespace memory {

DirModel::DirModel(QObject *parent)
    : QFileSystemModel(parent)
{
    setFilter(QDir::NoDotAndDotDot|QDir::AllDirs);
    setResolveSymlinks(true);
}

QVariant DirModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DecorationRole) {
        return QVariant();
    }

    return QFileSystemModel::data(index, role);
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
