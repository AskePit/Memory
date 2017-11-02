#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "dirmodel.h"
#include "eventfilter.h"
#include "highlighters/highlighters.h"
#include "utils.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QTextStream>
#include <QMimeData>
#include <QBitmap>
#include <QPrinter>
#include <QPrintDialog>
#include <QScrollBar>

namespace memory {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dirModel(new DirModel(this)),
    listEventFilter(new EventFilter),
    currentContent(CurrentContent::Text),
    highlighter(nullptr),
    settings(QStringLiteral("PitM"), QStringLiteral("Memory")),
    currFileName(QString::null),
    dirChanged(true),
    fileEdited(false)
{
    ui->setupUi(this);
    addAction(ui->actionSave);
    setWindowIcon(QIcon(QStringLiteral(":/window_icon.png")));

    ui->treeSplitter->setSizes({100, 260});
    ui->contentSplitter->setSizes({100, 260});

    ui->imgArea->hide();

    ui->tree->setModel(dirModel);

    QString path = settings.value(QStringLiteral("dir"), QString()).toString();
    if(!path.isEmpty()) {
        changeDir(path);
    }
    ui->tree->setRootIndex(dirModel->rootIndex());

    makeConnections();

    QString treePos = settings.value(QStringLiteral("treePosition"), QString()).toString();
    if(!treePos.isEmpty()) {
        ui->tree->setCurrentIndex(dirModel->index(treePos));
        connect(this, &MainWindow::listUpdated, this, &MainWindow::recoverFileAfterListUpdate);
    }

    createTrayIcon();
}

void MainWindow::makeConnections()
{
    ui->filesList->installEventFilter(listEventFilter);
    ui->tree->installEventFilter(listEventFilter);
    ui->imgView->installEventFilter(listEventFilter);

    connect(ui->tree->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onDirChanged);
    connect(ui->filesList->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);
    connect(dirModel, &DirModel::fileMoved, [this](){ onDirChanged(ui->tree->currentIndex(), QModelIndex()); });

    connect(ui->filesList, &QTableWidget::customContextMenuRequested, this, &MainWindow::showListContextMenu);
    connect(ui->tree, &QTreeView::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);

    connect(listEventFilter, &EventFilter::listResized, this, &MainWindow::updateList);
    connect(listEventFilter, &EventFilter::deleteFile, this, &MainWindow::on_actionDelete_File_triggered);
    connect(listEventFilter, &EventFilter::deleteDir, this, &MainWindow::on_actionDelete_Folder_triggered);
    connect(listEventFilter, &EventFilter::renameFile, this, &MainWindow::on_actionRename_File_triggered);

    connect(listEventFilter, &EventFilter::picPress, [this](const QPoint &p) {
        picPressed = true;
        picPressPoint = p;
    });

    connect(listEventFilter, &EventFilter::picRelease, [this](const QPoint &p) {
        Q_UNUSED(p);
        picPressed = false;
        picPressPoint = p;
    });

    connect(listEventFilter, &EventFilter::picMove, [this](const QPoint &p) {
        if(!picPressed) {
            return;
        }

        auto hBar { ui->imgArea->horizontalScrollBar() };
        auto vBar { ui->imgArea->verticalScrollBar() };
        bool barsVisible { hBar->isVisible() || vBar->isVisible() };

        if(!barsVisible) {
            return;
        }

        QPoint diff { picPressPoint - p };
        hBar->setValue(hBar->value() + diff.x());
        vBar->setValue(vBar->value() + diff.y());
    });

    connect(ui->textEditor, &QPlainTextEdit::modificationChanged, [=](bool b) {
        Q_UNUSED(b);
        fileEdited = true;
    });

    connect(qApp, &QApplication::aboutToQuit, this, &MainWindow::onQuit);
}

void MainWindow::recoverFileAfterListUpdate()
{
    QString tablePos = settings.value(QStringLiteral("tablePosition"), QString()).toString();
    if(!tablePos.isEmpty()) {
        QPoint pos = getFilePos(ui->filesList, tablePos);
        ui->filesList->setCurrentCell(pos.y(), pos.x());
    }
    loadGeometry();

    // should be performed once at startup
    disconnect(this, &MainWindow::listUpdated, this, &MainWindow::recoverFileAfterListUpdate);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    onQuit();
    QMainWindow::closeEvent(event);
}

void MainWindow::saveGeometry()
{
    settings.beginGroup(QStringLiteral("MainWindow"));
    settings.setValue(QStringLiteral("geometry"), QMainWindow::saveGeometry());

    auto treeSizes = ui->treeSplitter->sizes();
    settings.setValue(QStringLiteral("treeSplit0"), treeSizes[0]);
    settings.setValue(QStringLiteral("treeSplit1"), treeSizes[1]);

    auto contentSplitterSizes = ui->contentSplitter->sizes();
    settings.setValue(QStringLiteral("listSplit0"), contentSplitterSizes[0]);
    settings.setValue(QStringLiteral("listSplit1"), contentSplitterSizes[1]);

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

    settings.setValue(QStringLiteral("expandedList"), expandedList);
    settings.endGroup();
}

void MainWindow::loadGeometry()
{
    settings.beginGroup(QStringLiteral("MainWindow"));
    int treeSplit0 = settings.value(QStringLiteral("treeSplit0"), 100).toInt();
    int treeSplit1 = settings.value(QStringLiteral("treeSplit1"), 260).toInt();
    int contentSplit0 = settings.value(QStringLiteral("listSplit0"), 100).toInt();
    int contentSplit1 = settings.value(QStringLiteral("listSplit1"), 260).toInt();

    if(treeSplit0 != -1 && treeSplit1 != -1) {
        ui->treeSplitter->setSizes({treeSplit0, treeSplit1});
    }

    if(contentSplit0 != -1 && contentSplit1 != -1) {
        ui->contentSplitter->setSizes({contentSplit0, contentSplit1});
    }

    auto g = settings.value(QStringLiteral("geometry")).toByteArray();
    restoreGeometry(g);

    const auto list = settings.value(QStringLiteral("expandedList")).toList();
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

    clearFilesList(ui->filesList);
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

    const auto filesInfo = dir.entryInfoList({}, QDir::Files);

    for(const auto &f : filesInfo) {
        QString path = f.filePath();
        files << path;
    }

    dirChanged = true;
    QString currentPath { dirModel->filePath(current) };
    dirModel->setCurrentPath(currentPath);
    updateList();

    settings.setValue(QStringLiteral("treePosition"), currentPath);

    saveCurrentFile();
}

void MainWindow::saveCurrentFile()
{
    if(!currFileName.isEmpty() && fileEdited && !currentContent.test(CurrentContent::Binary)) {
        QFile file(currFileName);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        QTextStream ss(&file);
        ss.setCodec("UTF-8");
        ss << ui->textEditor->toPlainText();
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
        auto prevItem = ui->filesList->item(previous.row(), previous.column());

        if(prevItem != nullptr) {
            boldenFileItem(prevItem, false);
        }
    }

    if(!current.isValid()) {
        return;
    }

    auto currItem = ui->filesList->item(current.row(), current.column());

    if(currItem == nullptr) {
        return;
    }

    boldenFileItem(currItem, true);

    ui->actionDelete_File->setEnabled(true);
    ui->actionRename_File->setEnabled(true);

    ui->textEditor->clear();
    fileEdited = false;

    int idx = ui->filesList->rowCount()*current.column() + current.row();

    QFileInfo i(files[idx]);
    currFileName = i.filePath();

    // picture
    if(isPicture(currFileName)) {
        currentContent.reset();
        currentContent.set(CurrentContent::Picture);
        currentContent.set(CurrentContent::Binary);

        ui->textEditor->hide();
        ui->imgArea->show();

        QPixmap pixmap(currFileName);
        ui->imgView->setPixmap(pixmap);
    // text
    } else {
        currentContent.reset();
        currentContent.set(CurrentContent::Text);

        ui->textEditor->setEnabled(true);

        ui->imgArea->hide();
        ui->textEditor->show();

        QFile file(currFileName);
        file.open(QIODevice::ReadOnly);

        if(isBinary(file)) {
            currentContent.set(CurrentContent::Binary);

            //ui->textEditor->setPlainText(tr("BINARY FILE"));
            ui->textEditor->setPlainText(binaryToText(file.readAll()));
            ui->textEditor->setReadOnly(true);
        } else {
            ui->textEditor->setPlainText(QString::fromUtf8(file.readAll()));
            ui->textEditor->setReadOnly(false);
        }

        file.close();
        fileEdited = false;
        applyHighlighter();
    }

    settings.setValue(QStringLiteral("tablePosition"), QFileInfo(currFileName).fileName());
}

void MainWindow::applyHighlighter()
{
    QString prefix("memory::");
    QString suffix("Highlighter");
    QString id;
    if(highlighter) {
        id = highlighter->metaObject()->className(); // memory::CppHighlighter
        id = id.mid(prefix.size());                  // CppHighlighter
        id.truncate(id.count() - suffix.count());    // Cpp
    }

    const QString cppId("Cpp");
    const QString jsId("JS");
    const QString tabId("Tab");

    bool doSwitch = false;

    auto c = Qt::CaseInsensitive;

    if(currFileName.endsWith(QLatin1String(".cpp"), c) || currFileName.endsWith(QLatin1String(".h"), c) || currFileName.endsWith(QLatin1String(".c"), c)) {
        doSwitch = (id != cppId);
        id = cppId;
    } else if(currFileName.endsWith(QLatin1String(".js"), c)) {
        doSwitch = (id != jsId);
        id = jsId;
    } else if(currFileName.endsWith(QLatin1String(".tab"), c)) {
        doSwitch = (id != tabId);
        id = tabId;
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

    QTextDocument *doc = ui->textEditor->document();

    if(id == cppId) {
        highlighter = new CppHighlighter(doc);
    } else if(id == jsId) {
        highlighter = new JSHighlighter(doc);
    } else if(id == tabId) {
        highlighter = new TabHighlighter(doc);
    }
}

QString getNameForList(const QString &name)
{
    QString res = name;
    res = QFileInfo(res).fileName();
    if(res.endsWith(QLatin1String(".txt"))) {
        res.truncate(res.count()-4);
    }

    return res;
}

void MainWindow::updateList()
{
    QString remember;
    if(!dirChanged) {
        disconnect(ui->filesList->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);
        if(ui->filesList->currentItem()) {
            remember = ui->filesList->currentItem()->text();
        }
    }

    ui->filesList->clearContents();

    int tableH = ui->filesList->height();
    int rowH = 18;
    int rows = tableH/rowH-3;
    int columns = files.count() / rows;
    columns += files.count() % rows ? 1 : 0;

    rows = std::min(rows, files.count());

    ui->filesList->setRowCount(rows);
    ui->filesList->setColumnCount(columns);

    for(int i = 0; i<files.count(); ++i) {
        QString f = files[i];

        f = getNameForList(f);

        auto item = new QTableWidgetItem(f);
        item->setToolTip(f);

        int row = i%rows;
        int column = i/rows;
        ui->filesList->setItem(row, column, item);

        QModelIndex index = ui->filesList->model()->index(row, column);
        QPixmap icon(QStringLiteral(":/bullet.png"));
        ui->filesList->model()->setData(index, icon, Qt::DecorationRole);

        if(f == remember) {
            boldenFileItem(item, true);
            ui->filesList->setCurrentItem(item);
        }
    }

    resizeFilesColumns(ui->filesList);

    if(dirChanged) {
        dirChanged = false;
    } else {
        connect(ui->filesList->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);
    }

    emit listUpdated();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showListContextMenu(const QPoint& point)
{
    QPoint globalPos = ui->filesList->mapToGlobal(point);
    QMenu menu(ui->filesList);
    menu.addAction(ui->actionRename_File);
    menu.addAction(ui->actionNew_File);
    menu.addAction(ui->actionDelete_File);

    auto item = ui->filesList->itemAt(point);
    ui->actionDelete_File->setEnabled(item);
    ui->actionRename_File->setEnabled(item);

    menu.exec(globalPos);
}

void MainWindow::showTreeContextMenu(const QPoint& point)
{
    QPoint globalPos = ui->tree->mapToGlobal(point);
    QMenu menu(ui->tree);
    menu.addAction(ui->actionRename_Folder);
    menu.addAction(ui->actionNew_Child_Folder);
    menu.addAction(ui->actionNew_Sibling_Folder);
    menu.addAction(ui->actionDelete_Folder);

    auto index = ui->tree->indexAt(point);
    ui->actionDelete_Folder->setEnabled(index.isValid());
    ui->actionRename_Folder->setEnabled(index.isValid());

    menu.exec(globalPos);
}

void MainWindow::on_actionDelete_File_triggered()
{
    if(!ui->filesList->currentIndex().isValid()) {
        return;
    }

    int idx = ui->filesList->rowCount()*ui->filesList->currentColumn() + ui->filesList->currentRow();

    int ret = callQuestionDialog(tr("Delete file \"%1\"?").arg(ui->filesList->currentItem()->text()));
    if(ret != QMessageBox::Ok) {
        return;
    }

    QString filename = QFileInfo(files[idx]).filePath();

    QFile::remove(filename);
    files.removeAt(idx);
    ui->textEditor->clear();

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

    int ret = callQuestionDialog(tr("Delete folder \"%1\"?").arg(dirModel->fileName(index)));
    if(ret != QMessageBox::Ok) {
        return;
    }

    dirModel->remove(index);
}

void MainWindow::on_actionNew_File_triggered()
{
    QString fileName = getFileNameDialog(tr("New file"), tr("File name:"), QStringLiteral(""), this);

    if(fileName.isEmpty()) {
        return;
    }

    if(!fileName.contains('.')) {
        fileName += QLatin1String(".txt");
    }

    QString dirPath = dirModel->filePath(ui->tree->currentIndex());
    QString newFilePath = dirPath + QDir::separator() + fileName;
    createFile(newFilePath);

    files << newFilePath;
    files.sort(Qt::CaseInsensitive);
    updateList();

    QPoint pos = getFilePos(ui->filesList, fileName);
    ui->filesList->setCurrentCell(pos.y(), pos.x());
}

void MainWindow::on_actionNew_Child_Folder_triggered()
{
    QString dirName = getFileNameDialog(tr("New child folder"), tr("Folder name:"), QStringLiteral(""), this);
    if(dirName.isEmpty()) {
        return;
    }

    QModelIndex newDir = dirModel->mkdir(ui->tree->currentIndex(), dirName);
    ui->tree->setCurrentIndex(newDir);
}

void MainWindow::on_actionNew_Sibling_Folder_triggered()
{
    QString dirName = getFileNameDialog(tr("New sibling folder"), tr("Folder name:"), QStringLiteral(""), this);
    if(dirName.isEmpty()) {
        return;
    }

    QModelIndex newDir = dirModel->mkdir(ui->tree->currentIndex().parent(), dirName);
    ui->tree->setCurrentIndex(newDir);
}

void MainWindow::on_actionOpen_Folder_triggered()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose root folder"), settings.value(QStringLiteral("dir"), QLatin1String(".")).toString());
    if(path.isEmpty()) {
        return;
    }

    changeDir(path);
}

void MainWindow::changeDir(const QString &path)
{
    dirChanged = true;
    clearFilesList(ui->filesList);
    files.clear();
    ui->textEditor->clear();
    fileEdited = false; // prevent from clearing file due to previous line
    currFileName = QString::null;

    QString parentPath = QFileInfo(path).dir().path();

    dirModel->setRootPath(parentPath);
    dirModel->setFilterRoot(path);
    ui->tree->setRootIndex(dirModel->rootIndex());

    ui->tree->clearSelection();

    settings.setValue(QStringLiteral("dir"), path);
}

void MainWindow::on_actionRename_File_triggered()
{
    QString newName = getFileNameDialog(tr("Rename File"), tr("New file name:"), getNameForList(currFileName), this);
    if(newName.isEmpty()) {
        return;
    }

    if(!newName.contains('.')) {
        newName += QLatin1String(".txt");
    }

    QFile f(currFileName);
    newName = QFileInfo(currFileName).path() + QDir::separator() + newName;

    f.rename(newName);

    for(auto &f : files) {
        if(f == currFileName) {
            f = newName;
            break;
        }
    }

    currFileName = newName;

    ui->filesList->currentItem()->setText(getNameForList(newName));
    applyHighlighter();
}

void MainWindow::on_actionRename_Folder_triggered()
{
    QString newName = getFileNameDialog(tr("Rename Folder"), tr("New folder name:"), dirModel->fileInfo(ui->tree->currentIndex()).fileName(), this);
    if(newName.isEmpty()) {
        return;
    }

    newName = dirModel->fileInfo(ui->tree->currentIndex()).path() + QDir::separator() + newName;

    QFile f(dirModel->filePath(ui->tree->currentIndex()));
    f.rename(newName);
}

void MainWindow::on_actionExpand_Tree_triggered()
{
    ui->tree->expandAll();
}

void MainWindow::on_actionCollapse_Tree_triggered()
{
    ui->tree->collapseAll();
}

void MainWindow::createTrayIcon()
{
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            showNormal();
        }
    });

    trayIcon->setIcon(QIcon(QStringLiteral(":/tray_icon.png")));
    trayIcon->setToolTip(QStringLiteral("PitM Memory"));
    trayIcon->show();
}

void MainWindow::on_actionPrint_triggered()
{
    QTextDocument *document = ui->textEditor->document()->clone();

    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;

    document->print(&printer);
}

void MainWindow::on_actionSave_triggered()
{
    saveCurrentFile();
}

} // namespace memory
