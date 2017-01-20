#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dirmodel.h"
#include "eventfilter.h"
#include "highlighters/cplusplus.h"
#include "utils.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

namespace memory {



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dirModel(new DirModel(this)),
    listEventFilter(new EventFilter),
    highlighter(nullptr),
    settings("PitM", "Memory"),
    currFileName(QString::null),
    dirChanged(true),
    fileEdited(false)
{
    ui->setupUi(this);

    ui->treeSplitter->setSizes({100, 260});
    ui->listSplitter->setSizes({100, 260});

    ui->tree->setModel(dirModel);

    QString path = settings.value("dir", QString()).toString();
    if(!path.isEmpty()) {
        changeDir(path);
    }
    ui->tree->setRootIndex(dirModel->rootIndex());

    ui->list->installEventFilter(listEventFilter);
    ui->tree->installEventFilter(listEventFilter);

    connect(ui->tree->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onDirChanged);
    connect(ui->list->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);

    connect(ui->list, &QTableWidget::customContextMenuRequested, this, &MainWindow::showListContextMenu);
    connect(ui->tree, &QTreeView::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);

    connect(listEventFilter, &EventFilter::listResized, this, &MainWindow::updateList);
    connect(listEventFilter, &EventFilter::deleteFile, this, &MainWindow::on_actionDelete_File_triggered);
    connect(listEventFilter, &EventFilter::deleteDir, this, &MainWindow::on_actionDelete_Folder_triggered);
    connect(listEventFilter, &EventFilter::renameFile, this, &MainWindow::on_actionRename_File_triggered);
    connect(listEventFilter, &EventFilter::renameDir, this, &MainWindow::on_actionRename_Folder_triggered);

    connect(ui->field, &QPlainTextEdit::modificationChanged, [=](bool b) {
        Q_UNUSED(b);
        fileEdited = true;
    });

    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::onQuit);

    QString pos = settings.value("treePosition", QString()).toString();
    if(!pos.isEmpty()) {
        ui->tree->setCurrentIndex(dirModel->index(pos));
    }

    loadGeometry();
}

void MainWindow::saveGeometry()
{
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", QMainWindow::saveGeometry());
    settings.setValue("windowState", saveState());

    auto treeSizes = ui->treeSplitter->sizes();
    settings.setValue("treeSplit0", treeSizes[0]);
    settings.setValue("treeSplit1", treeSizes[1]);

    auto listSizes = ui->listSplitter->sizes();
    settings.setValue("listSplit0", listSizes[0]);
    settings.setValue("listSplit1", listSizes[1]);

    QStringList expandedList;
    dirModel->foreach_index([&](const QModelIndex &index) {
        if(ui->tree->isExpanded(index)) {
            QString path = dirModel->filePath(index);  // C:/Some/Path/to/value
            QString root = dirModel->rootPath();       // C:/Some/Path

            if(path.startsWith(root)) {
                path = path.mid(root.count()+1);       // to/value
            }

            expandedList << path;
        }
    });

    settings.setValue("expandedList", expandedList);
    settings.endGroup();
}

void MainWindow::loadGeometry()
{
    settings.beginGroup("MainWindow");
    int treeSplit0 = settings.value("treeSplit0", 100).toInt();
    int treeSplit1 = settings.value("treeSplit1", 260).toInt();
    int listSplit0 = settings.value("listSplit0", 100).toInt();
    int listSplit1 = settings.value("listSplit1", 260).toInt();

    if(treeSplit0 != -1 && treeSplit1 != -1) {
        ui->treeSplitter->setSizes({treeSplit0, treeSplit1});
    }

    if(listSplit0 != -1 && listSplit1 != -1) {
        ui->listSplitter->setSizes({listSplit0, listSplit1});
    }

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    auto list = settings.value("expandedList").toList();
    QString root = dirModel->rootPath();
    for(const auto &elem : list) {
        QString path = elem.toString();
        path = QString("%1/%2").arg(root, path);
        QModelIndex index = dirModel->index(path);
        ui->tree->setExpanded(index, true);
    }

    settings.endGroup();
}

void MainWindow::onDirChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    ui->actionDelete_Folder->setEnabled(false);

    clearFilesList(ui->list);
    files.clear();

    if(!current.isValid()) {
        return;
    }

    ui->actionNew_Child_Folder->setEnabled(true);
    ui->actionNew_Sibling_Folder->setEnabled(true);
    ui->actionNew_File->setEnabled(true);
    ui->actionDelete_Folder->setEnabled(true);

    QFileInfo i = dirModel->fileInfo(current);
    QDir dir(i.filePath());

    auto filesInfo = dir.entryInfoList({}, QDir::Files);

    for(auto &f : filesInfo) {
        QString path = f.filePath();
        files << path;
    }

    dirChanged = true;
    updateList();

    settings.setValue("treePosition", dirModel->filePath(current));
}

void MainWindow::saveCurrentFile()
{
    if(!currFileName.isEmpty() && fileEdited) {
        QFile file(currFileName);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        QTextStream ss(&file);
        ss.setCodec("UTF-8");
        ss << ui->field->toPlainText();
        file.close();
    }
}

void MainWindow::onQuit()
{
    saveCurrentFile();
    saveGeometry();
}

void MainWindow::onFileChanged(const QModelIndex &current, const QModelIndex &previous)
{
    ui->actionDelete_File->setEnabled(false);
    ui->actionRename_File->setEnabled(false);

    saveCurrentFile();

    if(previous.isValid()) {
        auto prevItem = ui->list->item(previous.row(), previous.column());

        if(prevItem != nullptr) {
            boldenFileItem(prevItem, false);
        }
    }

    if(!current.isValid()) {
        return;
    }

    auto currItem = ui->list->item(current.row(), current.column());

    if(currItem == nullptr) {
        return;
    }

    boldenFileItem(currItem, true);

    ui->actionDelete_File->setEnabled(true);
    ui->actionRename_File->setEnabled(true);

    ui->field->clear();
    fileEdited = false;

    int idx = ui->list->rowCount()*current.column() + current.row();

    QFileInfo i(files[idx]);
    currFileName = i.filePath();
    QFile file(currFileName);
    file.open(QIODevice::ReadOnly);

    if(isBinary(file)) {
        ui->field->setPlainText("BINARY FILE");
        ui->field->setDisabled(true);
    } else {
        ui->field->setPlainText(QString::fromUtf8(file.readAll()));
        ui->field->setEnabled(true);
    }

    file.close();
    fileEdited = false;
    applyHighlighter();
}

void MainWindow::applyHighlighter()
{
    QString prefix = "memory::";
    QString suffix = "Highlighter";
    QString id;
    if(highlighter) {
        id = highlighter->metaObject()->className(); // memory::CppHighlighter
        id = id.mid(prefix.size());                  // CppHighlighter
        id.truncate(id.count() - suffix.count());    // Cpp
    }

    bool doSwitch = false;

    if(currFileName.endsWith(".cpp") || currFileName.endsWith(".h") || currFileName.endsWith(".c")) {
        doSwitch = (id != "Cpp");
        id = "Cpp";
    } else {
        doSwitch = !id.isEmpty();
        id.clear();
    }

    if(!doSwitch) {
        return;
    }

    if(highlighter) {
        delete highlighter;
        highlighter = nullptr;
    }

    if(id == "Cpp") {
        highlighter = new CppHighlighter(ui->field->document());
    }
}

QString getNameForList(const QString &name)
{
    QString res = name;
    res = QFileInfo(res).fileName();
    if(res.endsWith(".txt")) {
        res.truncate(res.count()-4);
    }

    return res;
}

void MainWindow::updateList()
{
    QString remember;
    if(!dirChanged) {
        disconnect(ui->list->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);
        if(ui->list->currentItem()) {
            remember = ui->list->currentItem()->text();
        }
    }

    ui->list->clearContents();

    int tableH = ui->list->height();
    int rowH = 18;
    int rows = tableH/rowH-3;
    int columns = files.count() / rows;
    columns += files.count() % rows ? 1 : 0;

    rows = std::min(rows, files.count());

    ui->list->setRowCount(rows);
    ui->list->setColumnCount(columns);

    for(int i = 0; i<files.count(); ++i) {
        QString f = files[i];

        f = getNameForList(f);

        auto item = new QTableWidgetItem(f);

        int row = i%rows;
        int column = i/rows;
        ui->list->setItem(row, column, item);

        QModelIndex index = ui->list->model()->index(row, column);
        QPixmap icon(":/bullet.png");
        ui->list->model()->setData(index, icon, Qt::DecorationRole);

        if(f == remember) {
            boldenFileItem(item, true);
            ui->list->setCurrentItem(item);
        }
    }

    resizeFilesColumns(ui->list);

    if(dirChanged) {
        dirChanged = false;
    } else {
        connect(ui->list->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showListContextMenu(const QPoint& point)
{
    QPoint globalPos = ui->list->mapToGlobal(point);
    QMenu menu(ui->list);
    menu.addAction(ui->actionRename_File);
    menu.addAction(ui->actionNew_File);
    menu.addAction(ui->actionDelete_File);

    auto item = ui->list->itemAt(point);
    ui->actionDelete_File->setEnabled(item);
    ui->actionRename_File->setEnabled(item);

    menu.exec(globalPos);
}

void MainWindow::showTreeContextMenu(const QPoint& point)
{
    QPoint globalPos = ui->tree->mapToGlobal(point);
    QMenu menu(ui->tree);
    menu.addAction(ui->actionNew_Child_Folder);
    menu.addAction(ui->actionNew_Sibling_Folder);
    menu.addAction(ui->actionDelete_Folder);

    auto index = ui->tree->indexAt(point);
    ui->actionDelete_Folder->setEnabled(index.isValid());

    menu.exec(globalPos);
}

void MainWindow::on_actionDelete_File_triggered()
{
    if(!ui->list->currentIndex().isValid()) {
        return;
    }

    int idx = ui->list->rowCount()*ui->list->currentColumn() + ui->list->currentRow();

    int ret = callQuestionDialog(QString("Delete file \"%1\"?").arg(ui->list->currentItem()->text()));
    if(ret != QMessageBox::Ok) {
        return;
    }

    QString filename = QFileInfo(files[idx]).filePath();

    QFile::remove(filename);
    files.removeAt(idx);
    ui->field->clear();

    dirChanged = false;
    fileEdited = false;
    currFileName = QString::null;

    updateList();
}

void MainWindow::on_actionDelete_Folder_triggered()
{
    auto index = ui->tree->currentIndex();
    if(!index.isValid()) {
        return;
    }

    int ret = callQuestionDialog(QString("Delete folder \"%1\"?").arg(dirModel->data(index).toString()));
    if(ret != QMessageBox::Ok) {
        return;
    }

    dirModel->remove(index);
}

void MainWindow::on_actionNew_File_triggered()
{
    QString fileName = getFileNameDialog("New file", "File name:", "", this);

    if(fileName.isEmpty()) {
        return;
    }

    if(!fileName.contains('.')) {
        fileName += ".txt";
    }

    QString dirPath = dirModel->filePath(ui->tree->currentIndex());
    QString newFilePath = dirPath + "/" + fileName;
    createFile(newFilePath);

    files << newFilePath;
    files.sort(Qt::CaseInsensitive);
    updateList();

    QPoint pos = getFilePos(ui->list, fileName);
    ui->list->setCurrentCell(pos.y(), pos.x());
}

void MainWindow::on_actionNew_Child_Folder_triggered()
{
    QString dirName = getFileNameDialog("New child folder", "Folder name:", "", this);
    if(dirName.isEmpty()) {
        return;
    }

    QModelIndex newDir = dirModel->mkdir(ui->tree->currentIndex(), dirName);
    ui->tree->setCurrentIndex(newDir);
}

void MainWindow::on_actionNew_Sibling_Folder_triggered()
{
    QString dirName = getFileNameDialog("New sibling folder", "Folder name:", "", this);
    if(dirName.isEmpty()) {
        return;
    }

    QModelIndex newDir = dirModel->mkdir(ui->tree->currentIndex().parent(), dirName);
    ui->tree->setCurrentIndex(newDir);
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this, "Choose root folder", settings.value("dir", ".").toString());
    if(path.isEmpty()) {
        return;
    }

    changeDir(path);
}

void MainWindow::changeDir(const QString &path)
{
    dirChanged = true;
    clearFilesList(ui->list);
    files.clear();
    ui->field->clear();
    fileEdited = false; // prevent from clearing file due to previous line
    currFileName = QString::null;

    QString parentPath = QFileInfo(path).dir().path();

    dirModel->setRootPath(parentPath);
    dirModel->setFilterRoot(path);
    ui->tree->setRootIndex(dirModel->rootIndex());

    ui->tree->clearSelection();

    settings.setValue("dir", path);
}

void MainWindow::on_actionRename_File_triggered()
{
    QString newName = getFileNameDialog("Rename File", "New file name:", getNameForList(currFileName), this);
    if(newName.isEmpty()) {
        return;
    }

    if(!newName.contains('.')) {
        newName += ".txt";
    }

    QFile f(currFileName);
    newName = QFileInfo(currFileName).path() + "/" + newName;

    f.rename(newName);

    for(auto &f : files) {
        if(f == currFileName) {
            f = newName;
            break;
        }
    }

    currFileName = newName;

    ui->list->currentItem()->setText(getNameForList(newName));
}

void MainWindow::on_actionRename_Folder_triggered()
{
    QString newName = getFileNameDialog("Rename Folder", "New folder name:", dirModel->fileInfo(ui->tree->currentIndex()).fileName(), this);
    if(newName.isEmpty()) {
        return;
    }

    newName = dirModel->fileInfo(ui->tree->currentIndex()).path() + "/" + newName;

    QFile f(dirModel->filePath(ui->tree->currentIndex()));
    f.rename(newName);
}

} // namespace memory

void memory::MainWindow::on_actionExpand_Tree_triggered()
{
    ui->tree->expandAll();
}

void memory::MainWindow::on_actionCollapse_Tree_triggered()
{
    ui->tree->collapseAll();
}
