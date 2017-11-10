#include "python.h"

namespace memory {

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat KeywordFormat;
    QTextCharFormat ClassnameFormat;
    QTextCharFormat StringFormat;
    QTextCharFormat CommentFormat;
    QTextCharFormat LiteralFormat;

    KeywordFormat.setForeground(QColor(0, 0, 255));
    ClassnameFormat.setForeground(QColor(30, 130, 160));
    StringFormat.setForeground(Qt::darkGreen);
    CommentFormat.setForeground(Qt::darkGreen);
    LiteralFormat.setForeground(QColor(0, 0, 170));

    m_colors = {
        { HighlightElement::Keyword, KeywordFormat },
        { HighlightElement::Classname, ClassnameFormat },
        { HighlightElement::String, StringFormat },
        { HighlightElement::Comment, CommentFormat },
        { HighlightElement::Literal, LiteralFormat },
    };

    //////
    /// KEYWORDS
    ////

    m_keywords = QStringList {
        QStringLiteral("and"),
        QStringLiteral("del"),
        QStringLiteral("from"),
        QStringLiteral("not"),
        QStringLiteral("while"),
        QStringLiteral("as"),
        QStringLiteral("elif"),
        QStringLiteral("global"),
        QStringLiteral("or"),
        QStringLiteral("with"),
        QStringLiteral("assert"),
        QStringLiteral("else"),
        QStringLiteral("if"),
        QStringLiteral("pass"),
        QStringLiteral("yeld"),
        QStringLiteral("break"),
        QStringLiteral("except"),
        QStringLiteral("import"),
        QStringLiteral("print"),
        QStringLiteral("class"),
        QStringLiteral("exec"),
        QStringLiteral("in"),
        QStringLiteral("raise"),
        QStringLiteral("continue"),
        QStringLiteral("finally"),
        QStringLiteral("is"),
        QStringLiteral("return"),
        QStringLiteral("def"),
        QStringLiteral("for"),
        QStringLiteral("lambda"),
        QStringLiteral("try"),
        QStringLiteral("exec"),
        QStringLiteral("None"),
        QStringLiteral("True"),
        QStringLiteral("False"),
    };

    //////
    /// CLASSNAMES
    ////

    m_classnames = QStringList {
    };
}

void PythonHighlighter::highlightBlock(const QString &text)
{
    // parsing state
    enum {
        Start = 0,
        Number = 1,
        Identifier = 2,
        String = 3,
        Comment = 4,
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
        QChar next_next_ch = (i < text.length() - 2) ? text.at(i + 2) : QChar();

        switch (state) {

        case Start:
            start = i;
            if (ch.isSpace()) {
                ++i;
            } else if (ch.isDigit() || (ch == '-' && !next_ch.isLetter())) {
                ++i;
                state = Number;
            } else if ((ch == '\'') && (next_ch == '\'') && (next_next_ch == '\'')) {
                i += 3;
                state = Comment;
            } else if ((ch == '\'') || (ch == '\"')) {
                ++i;
                state = String;
            } else if (ch.isLetter() || (ch == '_') || (ch == '$')) {
                ++i;
                state = Identifier;
            } else if (ch == '#') {
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
            if ((ch == '\'') && (next_ch == '\'') && (next_next_ch == '\'')) {
                i += 3;
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
