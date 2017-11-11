#include "shell.h"

#include <QRegularExpressionMatchIterator>

namespace memory {

ShellHighlighter::ShellHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat CommandFormat;
    QTextCharFormat KeyFormat;
    QTextCharFormat CommentFormat;

    CommandFormat.setFontWeight(QFont::Bold);
    CommandFormat.setForeground(Qt::darkMagenta);
    KeyFormat.setForeground(QColor(120, 120, 255));
    CommentFormat.setForeground(QColor(30, 130, 160));

    m_colors = {
        { HighlightElement::Comand, CommandFormat },
        { HighlightElement::Key, KeyFormat },
        { HighlightElement::Comment, CommentFormat },
    };
}

void ShellHighlighter::highlightBlock(const QString &text_)
{
    if(text_.startsWith('#') || text_.startsWith("rem ", Qt::CaseInsensitive)) {
        setFormat(0, text_.size(), m_colors[HighlightElement::Comment]);
        return;
    }

    QStringRef text(&text_);
    auto comands = text.split('|', QString::SkipEmptyParts);

    for(const QStringRef &comand : comands) {
        int i = 0;
        while(i < comand.size() && comand[i].isSpace()) { // eat whitespace
            ++i;
        }
        while(i < comand.size() && !comand[i].isSpace()) { // eat first word
            ++i;
        }

        setFormat(comand.position(), i, m_colors[HighlightElement::Comand]);
    }

    QString regexp = "-{1,2}[\\w\\d_]+";
    QRegularExpressionMatchIterator matchIterator = QRegularExpression(regexp).globalMatch(text_);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_colors[HighlightElement::Key]);
    }
}

} // memory
