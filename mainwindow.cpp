#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dirmodel.h"
#include "eventfilter.h"
#include "highlighters/cplusplus.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QDebug>

namespace memory {

static int callQuestionDialog(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

static QString getTextDialog(const QString &title, const QString &message, QWidget *parent)
{
    bool ok = false;
    QString answer = QInputDialog::getText(parent, title, message, QLineEdit::Normal, "", &ok);
    return ok ? answer : QString::null;
}

static const QString forbiddenSymbols = "\":/\\*?<>|";

static bool isCorrectFileName(const QString &fileName)
{
    for(const auto &ch : forbiddenSymbols) {
        if(fileName.contains(ch)) {
            return false;
        }
    }

    return true;
}

static QString getFileNameDialog(const QString &title, const QString &message, QWidget *parent)
{
    QString answer = getTextDialog(title, message, parent);
    while(!isCorrectFileName(answer) && !answer.isNull()) {
        answer = getTextDialog(title, message + " no any of " + forbiddenSymbols, parent);
    }

    return answer;
}

static void createFile(const QString &fileName)
{
    QFile f(fileName);
    f.open(QIODevice::WriteOnly);
    f.close();
}

static QPoint getFilePos(QTableWidget *w, const QString &str)
{
    QPoint point;
    bool found = false;
    for(int c = 0; c<w->columnCount(); ++c) {
        for(int r = 0; r<w->rowCount(); ++r) {
            auto item = w->item(r, c);
            if(item->text() == str) {
                point.setY(r);
                point.setX(c);
                found = true;
                break;
            }
        }
        if(found) {
            break;
        }
    }

    return point;
}

void clearFilesList(QTableWidget *w)
{
    w->clear();
    w->setRowCount(0);
    w->setColumnCount(0);
}

static void resizeFilesColumns(QTableWidget *w)
{
    const int extraSpace = 15;

    for(int c = 0; c<w->columnCount(); ++c) {
        w->resizeColumnToContents(c);
        w->setColumnWidth(c, w->columnWidth(c) + extraSpace);
    }
}

static void boldenFileItem(QTableWidgetItem *item, bool bold)
{
    QFont font = item->font();
    font.setBold(bold);
    item->setFont(font);
}

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

    connect(ui->field, &QPlainTextEdit::modificationChanged, [=](bool b) {
        Q_UNUSED(b);
        fileEdited = true;
    });

    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::onQuit);

    loadGeometry();
}

void MainWindow::saveGeometry()
{
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());

    auto treeSizes = ui->treeSplitter->sizes();
    settings.setValue("treeSplit0", treeSizes[0]);
    settings.setValue("treeSplit1", treeSizes[1]);

    auto listSizes = ui->listSplitter->sizes();
    settings.setValue("listSplit0", listSizes[0]);
    settings.setValue("listSplit1", listSizes[1]);
    settings.endGroup();
}

void MainWindow::loadGeometry()
{
    settings.beginGroup("MainWindow");
    QSize size = settings.value("size", QSize()).toSize();
    QPoint pos = settings.value("pos", QPoint()).toPoint();
    int treeSplit0 = settings.value("treeSplit0", 100).toInt();
    int treeSplit1 = settings.value("treeSplit1", 260).toInt();
    int listSplit0 = settings.value("listSplit0", 100).toInt();
    int listSplit1 = settings.value("listSplit1", 260).toInt();

    if(size.isValid()) {
        resize(size);
    }

    if(!pos.isNull()) {
        move(pos);
    }

    if(treeSplit0 != -1 && treeSplit1 != -1) {
        ui->treeSplitter->setSizes({treeSplit0, treeSplit1});
    }

    if(listSplit0 != -1 && listSplit1 != -1) {
        ui->listSplitter->setSizes({listSplit0, listSplit1});
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

    ui->field->clear();
    fileEdited = false;

    int idx = ui->list->rowCount()*current.column() + current.row();

    QFileInfo i(files[idx]);
    currFileName = i.filePath();
    QFile file(currFileName);
    file.open(QIODevice::ReadOnly);
    ui->field->setPlainText(QString::fromUtf8(file.readAll()));
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
        auto &f = files[i];
        /*if(f.endsWith(".txt")) {
            f.truncate(f.count()-4);
        }*/

        QFileInfo info(f);
        auto item = new QTableWidgetItem(info.fileName());

        int row = i%rows;
        int column = i/rows;
        ui->list->setItem(row, column, item);

        QModelIndex index = ui->list->model()->index(row, column);
        QPixmap icon(":/bullet.png");
        ui->list->model()->setData(index, icon, Qt::DecorationRole);

        if(info.fileName() == remember) {
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
    menu.addAction(ui->actionNew_File);
    menu.addAction(ui->actionDelete_File);

    auto item = ui->list->itemAt(point);
    ui->actionDelete_File->setEnabled(item);

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
    QString fileName = getFileNameDialog("New file", "File name:", this);

    if(fileName.isEmpty()) {
        return;
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
    QString dirName = getFileNameDialog("New child folder", "Folder name:", this);
    if(dirName.isEmpty()) {
        return;
    }

    QModelIndex newDir = dirModel->mkdir(ui->tree->currentIndex(), dirName);
    ui->tree->setCurrentIndex(newDir);
}

void MainWindow::on_actionNew_Sibling_Folder_triggered()
{
    QString dirName = getFileNameDialog("New sibling folder", "Folder name:", this);
    if(dirName.isEmpty()) {
        return;
    }

    QModelIndex newDir = dirModel->mkdir(ui->tree->currentIndex().parent(), dirName);
    ui->tree->setCurrentIndex(newDir);
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this, "Choose root folder", dirModel->rootPath());
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
    qDebug() << parentPath;

    dirModel->setRootPath(parentPath);
    dirModel->setFilterRoot(path);
    ui->tree->setRootIndex(dirModel->rootIndex());
    ui->tree->clearSelection();

    settings.setValue("dir", path);
}

} // namespace memory
