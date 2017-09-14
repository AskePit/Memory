#ifndef CLIKE_HIGHLIGHTER_H
#define CLIKE_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace memory {

class CLikeHighlighter : public ::QSyntaxHighlighter
{
    Q_OBJECT

public:
    CLikeHighlighter(QTextDocument *parent = 0);

protected:
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

    enum class HighlightElement {
        Keyword,
        Classname,
        String,
        Comment,
        Literal,
        Preprocessor,
    };

    QMap<HighlightElement, QTextCharFormat> m_colors;
    QStringList m_keywords;
    QStringList m_classnames;
    QStringList m_ppDirectives;
};


} // namespace memory

#endif // CLIKE_HIGHLIGHTER_H
