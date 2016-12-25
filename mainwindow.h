#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MemoryModel;
class ListEventFilter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void onDirChanged(const QModelIndex &current, const QModelIndex &previous);
    void onFileChanged(const QModelIndex &current, const QModelIndex &previous);
    void showListContextMenu(const QPoint&);
    void showTreeContextMenu(const QPoint&);
    void updateList();
    void deleteFile();
    void deleteDir();

private:
    Ui::MainWindow *ui;
    MemoryModel *model;
    ListEventFilter *listEventFilter;
    QStringList files;

    QString currFileName;
    bool dirChanged;      // directory was changed to another
    bool fileChanged;     // current file's content changed
};

#endif // MAINWINDOW_H
