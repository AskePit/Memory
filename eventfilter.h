#ifndef LISTEVENTFILTER_H
#define LISTEVENTFILTER_H

#include <QObject>

namespace memory {

class EventFilter : public QObject {
    Q_OBJECT
public:
    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void listResized();
    void wheeled(int delta);
    void deleteFile();
    void deleteDir();
};

} // namespace memory

#endif // LISTEVENTFILTER_H
