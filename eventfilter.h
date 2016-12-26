#ifndef LISTEVENTFILTER_H
#define LISTEVENTFILTER_H

#include <QObject>

class ListEventFilter : public QObject {
    Q_OBJECT
public:
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void listResized();
    void wheeled(int delta);
    void deleteFile();
    void deleteDir();
};

#endif // LISTEVENTFILTER_H
