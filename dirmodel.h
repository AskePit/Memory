#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <functional>

namespace memory {

class DirModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DirModel(QObject *parent = 0);

    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual bool hasChildren(const QModelIndex &parent) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

    QModelIndex rootIndex() const { return index(rootPath()); }
    QString currentPath() const { return _currentPath; }
    void setCurrentPath(const QString &path) { _currentPath = path; }

    // QFileSystemModel forwards
    QString rootPath() const { return _model.rootPath(); }
    virtual QModelIndex index(const QString &path, int column = 0) const { return mapFromSource(_model.index(path, column)); }
    virtual QModelIndex index(int row, int column) const { return QSortFilterProxyModel::index(row, column); }
    QDir::Filters filter() const { return _model.filter(); }
    QFileInfo fileInfo(const QModelIndex &index) const { return _model.fileInfo(mapToSource(index)); }
    bool remove(const QModelIndex &index) { return _model.remove(mapToSource(index)); }
    QString filePath(const QModelIndex &index) const { return _model.filePath(mapToSource(index)); }
    QString fileName(const QModelIndex &index) const { return _model.fileName(mapToSource(index)); }
    QModelIndex mkdir(const QModelIndex &parent, const QString &name) { return mapFromSource(_model.mkdir(mapToSource(parent), name)); }
    QModelIndex setRootPath(const QString &newPath) { return mapFromSource(_model.setRootPath(newPath)); }
    void setFilterRoot(const QString &root) { beginResetModel(); _filterRoot = root; endResetModel(); }
    QString filterRoot() { return _filterRoot; }

    void foreach_index(const std::function<void(const QModelIndex&)> &f, QModelIndex &&parent = QModelIndex());

signals:
    void fileMoved();

private:
    QFileSystemModel _model;
    QString _filterRoot;
    QString _currentPath;
};

} // namespace memory

#endif // MEMORYMODEL_H
