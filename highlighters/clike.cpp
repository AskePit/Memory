#include "clike.h"

namespace memory {

CLikeHighlighter::CLikeHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    //////
    /// COMMENTS
    ////

    singleLineCommentFormat.setForeground(Qt::darkGray);
    addRule(singleLineCommentFormat, "//[^\n]*");

    multiLineCommentFormat.setForeground(Qt::darkGray);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");


    //////
    /// FUNCTIONS
    ////

    /*functionFormat.setFontItalic(true);
    addRule(functionFormat, "\\b[A-Za-z0-9_]+(?=\\()");*/


    //////
    /// STRINGS
    ////

    quotationFormat.setForeground(Qt::darkGreen);
    addRule(quotationFormat, "\".*\"");


    //////
    /// LITERALS
    ////

    constantFormat.setForeground(Qt::darkGray);
    addRule(constantFormat, "\\b[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?[uflUFL]{0,3}\\b");
    addRule(constantFormat, "\\b0[x][0-9a-fA-F]+\\b");
    addRule(constantFormat, "\\b0[b][0-1]+\\b");
    addRule(constantFormat, "\\b0[o][0-7]+\\b");
}

void CLikeHighlighter::addRule(const QTextCharFormat &format, const QString &regexp)
{
    HighlightingRule rule;
    rule.pattern = QRegExp(regexp);
    rule.format = format;
    highlightingRules.append(rule);
}

void CLikeHighlighter::applyList(const QStringList &list, const QTextCharFormat &format, const QString &regexp)
{
    for (const QString &str : list) {
        QString pattern = QString(regexp).arg(str);
        addRule(format, pattern);
    }
}

void CLikeHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

} // memory
