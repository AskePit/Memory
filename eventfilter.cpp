#include "eventfilter.h"
#include <QTableWidget>
#include <QTreeView>
#include <QEvent>
#include <QLabel>
#include <QWheelEvent>

namespace memory {

bool EventFilter::eventFilter(QObject *watched, QEvent *event)
{
    QTableWidget *list = qobject_cast<QTableWidget *>(watched);
    QTreeView *tree = qobject_cast<QTreeView *>(watched);
    QLabel *picView = qobject_cast<QLabel *>(watched);

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
                }
            } break;
            default: break;
        }
    }

    if(picView) {
        QMouseEvent *e { dynamic_cast<QMouseEvent *>(event) };
        if(e) {
            QPoint pos = e->pos();

            switch(event->type()) {
                case QEvent::MouseButtonPress:   emit picPress(pos); break;
                case QEvent::MouseMove:          emit picMove(pos); break;
                case QEvent::MouseButtonRelease: emit picRelease(pos); break;
                default: break;
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

} // namespace memory
