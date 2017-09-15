#ifndef TAB_HIGHLIGHTER_H
#define TAB_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

namespace memory {

class TabHighlighter : public ::QSyntaxHighlighter
{
    Q_OBJECT

public:
    TabHighlighter(QTextDocument *parent = 0);

protected:
    virtual void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

    enum class HighlightElement {
        Dash,
        Number,
        Special,
    };

    struct Rule {
        QTextCharFormat format;
        QString regexp;
    };

    QMap<HighlightElement, Rule> m_rules;
};


} // namespace memory

#endif // TAB_HIGHLIGHTER_H
