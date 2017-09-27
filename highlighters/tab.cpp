#include "tab.h"

#include <QRegularExpressionMatchIterator>

namespace memory {

TabHighlighter::TabHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat DashFormat;
    QTextCharFormat NumberFormat;
    QTextCharFormat SpecialFormat;

    DashFormat.setForeground(Qt::lightGray);
    NumberFormat.setForeground(Qt::black);
    SpecialFormat.setForeground(QColor(100, 100, 100));

    m_rules = {
        { HighlightElement::Dash, {DashFormat, QStringLiteral("[-–—|]{1,}")} },
        { HighlightElement::Number, {NumberFormat, QStringLiteral("\\d{1,}")} },
        { HighlightElement::Special, {SpecialFormat, QStringLiteral("\\b[A-Ze]\\b|#|[^\\s\\w\\d-–—|]")} },
    };
}

void TabHighlighter::highlightBlock(const QString &text)
{
    for(auto key : m_rules.keys()) {
        auto rule = m_rules[key];

        QRegularExpressionMatchIterator matchIterator = QRegularExpression(rule.regexp).globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

} // memory
