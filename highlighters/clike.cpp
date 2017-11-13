#include "clike.h"

namespace memory {

CLikeHighlighter::CLikeHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat KeywordFormat;
    QTextCharFormat ClassnameFormat;
    QTextCharFormat StringFormat;
    QTextCharFormat CommentFormat;
    QTextCharFormat LiteralFormat;
    QTextCharFormat PreprocessorFormat;

    KeywordFormat.setForeground(QColor(0, 0, 255));
    ClassnameFormat.setForeground(QColor(30, 130, 160));
    StringFormat.setForeground(Qt::darkGreen);
    CommentFormat.setForeground(Qt::darkGreen);
    LiteralFormat.setForeground(QColor(0, 0, 170));
    PreprocessorFormat.setForeground(QColor(100, 100, 100));

    m_colors = {
        { HighlightElement::Keyword, KeywordFormat },
        { HighlightElement::Classname, ClassnameFormat },
        { HighlightElement::String, StringFormat },
        { HighlightElement::Comment, CommentFormat },
        { HighlightElement::Literal, LiteralFormat },
        { HighlightElement::Preprocessor, PreprocessorFormat },
    };
}

void CLikeHighlighter::highlightBlock(const QString &text)
{
    // parsing state
    enum {
        Start = 0,
        Number = 1,
        Identifier = 2,
        String = 3,
        Comment = 4,
        Preprocessor = 5,
    };

    int blockState = previousBlockState();
    int state = blockState & 15;
    if (blockState < 0) {
        state = Start;
    }

    int start = 0;
    int i = 0;
    while (i <= text.length()) {
        QChar ch = (i < text.length()) ? text.at(i) : QChar();
        QChar next_ch = (i < text.length() - 1) ? text.at(i + 1) : QChar();

        switch (state) {

        case Start:
            start = i;
            if (ch.isSpace()) {
                ++i;
            } else if (ch.isDigit() || (ch == '-' && !next_ch.isLetter())) {
                ++i;
                state = Number;
            } else if (ch.isLetter() || (ch == '_') || (ch == '$')) {
                ++i;
                state = Identifier;
            } else if (ch == '#') {
                while(next_ch.isSpace() && next_ch != '\n' && i <= text.length()) {
                    ++i;
                    ch = (i < text.length()) ? text.at(i) : QChar();
                    next_ch = (i < text.length() - 1) ? text.at(i + 1) : QChar();
                }
                ++i;
                state = Preprocessor;
            } else if ((ch == '\'') || (ch == '\"')) {
                ++i;
                state = String;
            } else if ((ch == '/') && (next_ch == '*')) {
                ++i;
                ++i;
                state = Comment;
            } else if ((ch == '/') && (next_ch == '/')) {
                i = text.length();
                setFormat(start, text.length(), m_colors[HighlightElement::Comment]);
            } else {
                ++i;
                state = Start;
            }
            break;

        case Number:
            if (ch.isSpace() || !(ch.isDigit() || ch.isLetter())) {
                setFormat(start, i - start, m_colors[HighlightElement::Literal]);
                state = Start;
            } else {
                ++i;
            }
            break;

        case Identifier:
            if (ch.isSpace() ||
                (!(ch.isDigit() || ch.isLetter() || (ch == '_') || (ch == '$')))) {
                QString token = text.mid(start, i - start).trimmed();
                if (m_keywords.contains(token)) {
                    setFormat(start, i - start, m_colors[HighlightElement::Keyword]);
                } else if (m_classnames.contains(token)) {
                    setFormat(start, i - start, m_colors[HighlightElement::Classname]);
                }
                state = Start;
            } else {
                ++i;
            }
            break;

        case Preprocessor:
            if (ch == '\n' || !ch.isLetter()) {
                QString token = text.mid(start + 1, i - start - 1).trimmed();
                if (m_ppDirectives.contains(token)) {
                    setFormat(start, i - start, m_colors[HighlightElement::Preprocessor]);
                }
                state = Start;
            } else {
                ++i;
            }
            break;

        case String:
            if ((ch == '\\') &&
                ((next_ch == '\\') || (next_ch == '\'') || (next_ch == '\"') ||
                 (next_ch == 'b') || (next_ch == 'r') || (next_ch == 'f') ||
                 (next_ch == 't') || (next_ch == 'v'))) {
                // Accept all valid escapes as part of string
                ++i;
                ++i;
            }
            else if (ch == text.at(start)) {
                QChar prev_prev = (i > 1) ? text.at(i - 2) : QChar();
                QChar prev = (i > 0) ? text.at(i - 1) : QChar();
                if ((!(prev == '\\')) || ((prev_prev == '\\') && (prev == '\\'))) {
                    ++i;
                    setFormat(start, i - start, m_colors[HighlightElement::String]);
                    state = Start;
                } else {
                    // If (ch == '\\') and we are here, there's an error.
                    // For example, an invalid escape sequence.
                    ++i;
                }
            } else {
                // If (ch == '\\') and we are here, there's an error
                // For example, an invalid escape sequence.
                ++i;
            }
            break;

        case Comment:
            if ((ch == '*') && (next_ch == '/')) {
                ++i;
                ++i;
                setFormat(start, i - start, m_colors[HighlightElement::Comment]);
                state = Start;
            } else {
                ++i;
            }
            break;

        default:
            state = Start;
            break;
        }
    }

    if (state == Comment) {
        setFormat(start, text.length(), m_colors[HighlightElement::Comment]);
    } else {
        state = Start;
    }

    blockState = (state & 15);
    setCurrentBlockState(blockState);
}

} // memory
