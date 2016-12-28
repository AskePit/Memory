#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QSyntaxHighlighter;

namespace memory {

class DirModel;
class EventFilter;

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

    void on_actionNew_File_triggered();
    void on_actionNew_Child_Folder_triggered();
    void on_actionNew_Sibling_Folder_triggered();
    void on_actionDelete_File_triggered();
    void on_actionDelete_Folder_triggered();
    void on_actionOpen_Folder_triggered();

    void saveCurrentFile();

private:
    Ui::MainWindow *ui;
    DirModel *dirModel;
    EventFilter *listEventFilter;
    QStringList files;
    QSyntaxHighlighter *highlighter;

    QString currFileName;
    bool dirChanged;
    bool fileEdited;

    void applyHighlighter();

};

} // namespace memory

#endif // MAINWINDOW_H
