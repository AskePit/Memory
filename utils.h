#ifndef UTILS_H
#define UTILS_H

#include <QFile>

class QTableWidget;
class QTableWidgetItem;

bool isBinary(QFile &f);
int callQuestionDialog(const QString &message);
QString getFileNameDialog(const QString &title, const QString &message, const QString &text, QWidget *parent);
void createFile(const QString &fileName);
QPoint getFilePos(QTableWidget *w, const QString &str);
void clearFilesList(QTableWidget *w);
void resizeFilesColumns(QTableWidget *w);
void boldenFileItem(QTableWidgetItem *item, bool bold);
bool isPicture(const QString &fileName);
QString binaryToText(const QByteArray &data, bool caps = true);

#endif // UTILS_H
