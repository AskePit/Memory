#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include <QSortFilterProxyModel>
#include <QFileSystemModel>

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

    QModelIndex rootIndex() { return index(rootPath()); }

    // QFileSystemModel forwards
    QString rootPath() const { return _model.rootPath(); }
    virtual QModelIndex index(const QString &path, int column = 0) const { return mapFromSource(_model.index(path, column)); }
    QDir::Filters filter() const { return _model.filter(); }
    QFileInfo fileInfo(const QModelIndex &index) const { return _model.fileInfo(mapToSource(index)); }
    bool remove(const QModelIndex &index) { return _model.remove(mapToSource(index)); }
    QString filePath(const QModelIndex &index) const { return _model.filePath(mapToSource(index)); }
    QModelIndex mkdir(const QModelIndex &parent, const QString &name) { return mapFromSource(_model.mkdir(mapToSource(parent), name)); }
    QModelIndex setRootPath(const QString &newPath) { return mapFromSource(_model.setRootPath(newPath)); }

private:
    QFileSystemModel _model;
};

} // namespace memory

#endif // MEMORYMODEL_H
