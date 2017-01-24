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

    void addRule(const QTextCharFormat &format, const QString &regexp);
    void applyList(const QStringList &list, const QTextCharFormat &format, const QString &regexp);

    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegExp commentStartExpression;
    QRegExp commentEndExpression;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;

    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat constantFormat;
};

} // namespace memory

#endif // CLIKE_HIGHLIGHTER_H
