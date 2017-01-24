#ifndef JS_HIGHLIGHTER_H
#define JS_HIGHLIGHTER_H

#include "clike.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace memory {

class JSHighlighter : public CLikeHighlighter
{
    Q_OBJECT

public:
    JSHighlighter(QTextDocument *parent = 0);
};

} // namespace memory

#endif // JS_HIGHLIGHTER_H
