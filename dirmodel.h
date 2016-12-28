#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include <QFileSystemModel>

namespace memory {

class DirModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit DirModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex rootIndex() { return index(rootPath()); }
};

} // namespace memory

#endif // MEMORYMODEL_H
