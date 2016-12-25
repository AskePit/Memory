#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "memorymodel.h"
#include "listeventfilter.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new MemoryModel("../notes", this)),
    listEventFilter(new ListEventFilter),
    currFileName(QString::null),
    dirChanged(true),
    fileChanged(false)
{
    ui->setupUi(this);

    ui->treeSplitter->setSizes({100, 260});
    ui->listSplitter->setSizes({100, 260});

    ui->tree->setModel(model);
    ui->tree->setRootIndex(model->rootIndex());

    connect(ui->tree->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onDirChanged);
    connect(ui->list->selectionModel(), &QItemSelectionModel::currentChanged, this, &MainWindow::onFileChanged);
    connect(ui->list, &QTableWidget::customContextMenuRequested, this, &MainWindow::showListContextMenu);
    connect(ui->tree, &QTreeView::customContextMenuRequested, this, &MainWindow::showTreeContextMenu);
    ui->list->installEventFilter(listEventFilter);
    connect(listEventFilter, &ListEventFilter::resized, this, &MainWindow::updateList);
    connect(listEventFilter, &ListEventFilter::deleteFile, this, &MainWindow::deleteFile);
    connect(ui->field, &QPlainTextEdit::textChanged, [=]() {
        fileChanged = true;
    });
}

void MainWindow::onDirChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    files.clear();

    QFileInfo i = model->fileInfo(current);
    QDir dir(i.filePath());

    auto filesInfo = dir.entryInfoList({}, QDir::Files);

    for(auto &f : filesInfo) {
        QString path = f.filePath();
        files << path;
    }

    dirChanged = true;
    updateList();
}

void MainWindow::onFileChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);

    if(!currFileName.isEmpty() && fileChanged) {
        qDebug() << "write";
        QFile file(currFileName);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate);
        QTextStream ss(&file);
        ss.setCodec("UTF-8");
        ss << ui->field->toPlainText();
        file.close();
    }

    if(!current.isValid()) {
        return;
    }

    if(ui->list->item(current.row(), current.column()) == nullptr) {
        return;
    }

    ui->field->clear();

    int idx = ui->list->rowCount()*current.column() + current.row();

    QFileInfo i(files[idx]);
    currFileName = i.filePath();
    QFile file(currFileName);
    file.open(QIODevice::ReadOnly);
    ui->field->setPlainText(QString::fromUtf8(file.readAll()));
    file.close();

    fileChanged = false;
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
            ui->list->setCurrentItem(item);
        }
    }

    ui->list->resizeColumnsToContents();

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
    auto item = ui->list->itemAt(point);
    if(!item) {
        return;
    }

    QPoint globalPos = ui->list->mapToGlobal(point);

    QMenu menu(ui->list);
    QAction remove(tr("Delete"), this);
    connect(&remove, &QAction::triggered, [&](bool checked){
        Q_UNUSED(checked);
        deleteFile();
    });

    menu.addAction(&remove);
    menu.exec(globalPos);
}

void MainWindow::showTreeContextMenu(const QPoint& point)
{
    auto index = ui->tree->indexAt(point);
    if(!index.isValid()) {
        return;
    }

    QPoint globalPos = ui->tree->mapToGlobal(point);

    QMenu menu(ui->tree);
    QAction remove(tr("Delete"), this);
    connect(&remove, &QAction::triggered, [&](bool checked){
        Q_UNUSED(checked);
        deleteDir();
    });

    menu.addAction(&remove);
    menu.exec(globalPos);
}

void MainWindow::deleteFile()
{
    int idx = ui->list->rowCount()*ui->list->currentColumn() + ui->list->currentRow();
    QString filename = QFileInfo(files[idx]).filePath();

    QFile::remove(filename);
    files.removeAt(idx);
    ui->field->clear();

    dirChanged = false;
    fileChanged = false;
    currFileName = QString::null;

    updateList();
}

void MainWindow::deleteDir()
{
    auto index = ui->tree->currentIndex();
    model->remove(index);
}

