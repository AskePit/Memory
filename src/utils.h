#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QPoint>

class QWidget;
class QTableWidget;
class QTableWidgetItem;

int callQuestionDialog(const QString &message);
QString getFileNameDialog(const QString &title, const QString &message, const QString &text, QWidget *parent);
QPoint getFilePos(QTableWidget *w, const QString &str);
void clearFilesList(QTableWidget *w);
void resizeFilesColumns(QTableWidget *w);
void boldenFileItem(QTableWidgetItem *item, bool bold);


#endif // UTILS_H
