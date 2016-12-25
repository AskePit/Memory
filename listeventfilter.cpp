#include "listeventfilter.h"
#include <QTableWidget>
#include <QEvent>
#include <QWheelEvent>


bool ListEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    QTableWidget *list = qobject_cast<QTableWidget *>(watched);

    if (list) {
        switch(event->type()) {
            case QEvent::Resize: emit resized(); break;
            case QEvent::Wheel: {
                QWheelEvent *w = dynamic_cast<QWheelEvent *>(event);
                emit wheeled(w->delta());
            } break;
            case QEvent::KeyPress: {
                QKeyEvent *k = dynamic_cast<QKeyEvent *>(event);
                if(k->key() == Qt::Key_Delete) {
                    emit deleteFile();
                }
            } break;
            default: break;
        }
    }

    return QObject::eventFilter(watched, event);
}
