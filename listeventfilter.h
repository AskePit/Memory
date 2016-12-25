#ifndef LISTEVENTFILTER_H
#define LISTEVENTFILTER_H

#include <QObject>

class ListEventFilter : public QObject {
    Q_OBJECT
public:
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void resized();
    void wheeled(int delta);
    void deleteFile();
};

#endif // LISTEVENTFILTER_H
