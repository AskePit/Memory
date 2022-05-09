#include "dirmodel.h"
#include <QMimeData>
#include <QUrl>

namespace memory {

DirModel::DirModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , _model(this)
{
    _model.setFilter(QDir::NoDotAndDotDot|QDir::AllDirs);
    _model.setResolveSymlinks(true);
    setSourceModel(&_model);
    _model.setReadOnly(false);
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

bool DirModel::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    QModelIndex rootParent { index(_filterRoot).parent() };
    if(parent == rootParent) {
        return false;
    }

    if(data->hasFormat(QStringLiteral("application/x-qabstractitemmodeldatalist"))) {
        return true;
    }
    return QSortFilterProxyModel::canDropMimeData(data, action, row, column, parent);
}

bool DirModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if(data->hasFormat(QStringLiteral("application/x-qabstractitemmodeldatalist"))) {
        QByteArray encoded = data->data(QStringLiteral("application/x-qabstractitemmodeldatalist"));
        QDataStream stream(&encoded, QIODevice::ReadOnly);

        QList<QUrl> urls;
        while (!stream.atEnd())
        {
            int row, col;
            QMap<int,  QVariant> roleDataMap;
            stream >> row >> col >> roleDataMap;

            QString fileName { roleDataMap[Qt::DisplayRole].toString() };
            QString filePath { QString("%1/%2").arg(_currentPath, fileName) };
            urls.append(QUrl::fromLocalFile(filePath));
        }
        QMimeData *newData = new QMimeData;
        newData->setUrls(urls);
        bool res = QSortFilterProxyModel::dropMimeData(newData, Qt::MoveAction, row, column, parent);
        if(res) {
            emit fileMoved();
        }
        return res;
    }

    return QSortFilterProxyModel::dropMimeData(data, action, row, column, parent);
}

bool DirModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QString path = _model.filePath(sourceParent);

    if(path == rootPath() && _model.filePath(_model.index(sourceRow, 0, sourceParent)) != _filterRoot) {
        return false;
    }

    return true;
}

void DirModel::foreach_index(const std::function<void(const QModelIndex&)> &f, QModelIndex &&parent) {
    if (!parent.isValid())
        parent = QSortFilterProxyModel::index(0,0,QModelIndex());

    int numRows = rowCount(parent);

    for (int i=0; i<numRows; i++) {
        foreach_index(f, QSortFilterProxyModel::index(i,0,parent));
    }

    f(parent);
}

} // namespace memory
