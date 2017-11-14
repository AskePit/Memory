#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <bitset>

namespace Ui {
class MainWindow;
}

class QAction;
class QSystemTrayIcon;
class QMenu;

namespace memory {

class DirModel;
class EventFilter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

signals:
    void listUpdated();
    void fileRenamed(const QString &fileName);

private slots:
    void onDirChanged(const QModelIndex &current, const QModelIndex &previous);
    void onFileChanged(const QModelIndex &current, const QModelIndex &previous);
    void showListContextMenu(const QPoint&);
    void showTreeContextMenu(const QPoint&);
    void updateList();
    void recoverFileAfterListUpdate();

    void on_actionNew_File_triggered();
    void on_actionNew_Child_Folder_triggered();
    void on_actionNew_Sibling_Folder_triggered();
    void on_actionDelete_File_triggered();
    void on_actionDelete_Folder_triggered();
    void on_actionOpen_Folder_triggered();

    void onQuit();

    void on_actionRename_File_triggered();
    void on_actionRename_Folder_triggered();
    void on_actionExpand_Tree_triggered();
    void on_actionCollapse_Tree_triggered();
    void on_actionPrint_triggered();
    void on_actionSave_triggered();

private:
    void makeConnections();

    enum Content {
        Text,
        Picture,
        Binary,

        Count
    };

    Ui::MainWindow *ui;
    DirModel *dirModel;
    EventFilter *listEventFilter;
    QStringList files;
    std::bitset<Content::Count> content;
    QSettings settings;

    QString currFileName;
    bool dirChanged;
    bool fileEdited;

    void changeDir(const QString &path);
    void saveCurrentFile();

    void saveGeometry();
    void loadGeometry();

    // tray
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    void createTrayIcon();

    bool picPressed {false};
    QPoint picPressPoint;
};

} // namespace memory

#endif // MAINWINDOW_H
