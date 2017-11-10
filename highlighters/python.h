#ifndef PYTHON_HIGHLIGHTER_H
#define PYTHON_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace memory {

class PythonHighlighter : public ::QSyntaxHighlighter
{
    Q_OBJECT

public:
    PythonHighlighter(QTextDocument *parent = 0);

protected:
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

    enum class HighlightElement {
        Keyword,
        Classname,
        String,
        Comment,
        Literal,
    };

    QMap<HighlightElement, QTextCharFormat> m_colors;
    QStringList m_keywords;
    QStringList m_classnames;
};

} // namespace memory

#endif // PYTHON_HIGHLIGHTER_H
