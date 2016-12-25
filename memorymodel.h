#ifndef MEMORYMODEL_H
#define MEMORYMODEL_H

#include <QFileSystemModel>

class MemoryModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit MemoryModel(const QString &rootPath, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    const QString &rootPath() { return mRootPath; }
    QModelIndex rootIndex() { return index(mRootPath); }

signals:

public slots:

private:
    QString mRootPath;
};

#endif // MEMORYMODEL_H
