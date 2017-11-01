#include "utils.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QTableWidget>
#include <QCoreApplication>
#include <QImageReader>

bool isBinary(QFile &f)
{
    if(f.size() == 0) {
        return false;
    }

    char c;
    while(1) {
        f.getChar(&c);

        if(c == 0) {
            f.seek(0);
            return true;
        }

        if(f.atEnd()) {
            break;
        }
    }

    f.seek(0);
    return false;
}

int callQuestionDialog(const QString &message)
{
    QMessageBox msgBox;
    msgBox.setText(message);

    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    return msgBox.exec();
}

static QString getTextDialog(const QString &title, const QString &message, const QString &text, QWidget *parent)
{
    bool ok = false;
    QString answer = QInputDialog::getText(parent, title, message, QLineEdit::Normal, text, &ok);
    return ok ? answer : QString::null;
}

static const QString forbiddenSymbols("\":/\\*?<>|");

static bool isCorrectFileName(const QString &fileName)
{
    for(const auto &ch : forbiddenSymbols) {
        if(fileName.contains(ch)) {
            return false;
        }
    }

    return true;
}

QString getFileNameDialog(const QString &title, const QString &message, const QString &text, QWidget *parent)
{
    QString answer = getTextDialog(title, message, text, parent);
    while(!isCorrectFileName(answer) && !answer.isNull()) {
        answer = getTextDialog(title, message + QCoreApplication::translate("GLOBAL", " no any of ") + forbiddenSymbols, text, parent);
    }

    return answer;
}

void createFile(const QString &fileName)
{
    QFile f(fileName);
    f.open(QIODevice::WriteOnly);
    f.close();
}

QPoint getFilePos(QTableWidget *w, const QString &_str)
{
    QStringRef str(&_str);

    if(str.endsWith(QLatin1String(".txt"))) {
        str.truncate(str.length()-4);
    }

    QPoint point;
    bool found = false;
    for(int c = 0; c<w->columnCount(); ++c) {
        for(int r = 0; r<w->rowCount(); ++r) {
            auto item = w->item(r, c);
            if(!item) {
                continue;
            }
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

void resizeFilesColumns(QTableWidget *w)
{
    const int extraSpace = 15;

    for(int c = 0; c<w->columnCount(); ++c) {
        w->resizeColumnToContents(c);
        w->setColumnWidth(c, w->columnWidth(c) + extraSpace);
    }
}

void boldenFileItem(QTableWidgetItem *item, bool bold)
{
    QFont font = item->font();
    font.setBold(bold);
    item->setFont(font);
}

bool isPicture(const QString &fileName)
{
    QImageReader reader(fileName);
    return reader.format() != QByteArray();
}

static inline bool isNth(int val, int n) {
    return val % n == n - 1;
}

QString binaryToText(const QByteArray &data, bool caps)
{
    Q_UNUSED(caps);
    QByteArray raw = data.toHex();
    QString res;
    for(int i = 0; i<raw.size(); ++i) {
        res += raw[i];
        if(isNth(i, 2)) {
            res += ' ';
        }

        if(isNth(i, 32)) {
            res += '\n';
        } else if(isNth(i, 16)) {
            res += "| ";
        }
    }

    return res;
}
