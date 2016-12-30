#include "eventfilter.h"
#include <QTableWidget>
#include <QTreeView>
#include <QEvent>
#include <QWheelEvent>

namespace memory {

bool EventFilter::eventFilter(QObject *watched, QEvent *event)
{
    QTableWidget *list = qobject_cast<QTableWidget *>(watched);
    QTreeView *tree = qobject_cast<QTreeView *>(watched);

    if (list) {
        switch(event->type()) {
            case QEvent::Resize: emit listResized(); break;
            case QEvent::Wheel: {
                QWheelEvent *w = dynamic_cast<QWheelEvent *>(event);
                emit wheeled(w->delta());
            } break;
            case QEvent::KeyPress: {
                QKeyEvent *k = dynamic_cast<QKeyEvent *>(event);
                if(k->key() == Qt::Key_Delete) {
                    emit deleteFile();
                } else if(k->key() == Qt::Key_F2) {
                    emit renameFile();
                }
            } break;
            default: break;
        }
    }

    if(tree) {
        switch(event->type()) {
            case QEvent::KeyPress: {
                QKeyEvent *k = dynamic_cast<QKeyEvent *>(event);
                if(k->key() == Qt::Key_Delete) {
                    emit deleteDir();
                } else if(k->key() == Qt::Key_F2) {
                    emit renameDir();
                }
            } break;
            default: break;
        }
    }

    return QObject::eventFilter(watched, event);
}

} // namespace memory
