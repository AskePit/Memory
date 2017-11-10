#include "texteditor.h"
#include "utils.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextStream>

namespace memory {

TextEditor::TextEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(this)
{
    setTypes(Type::Text);
}

TextEditor::TextEditor(Type::mask allowedTypes, QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(this)
{
    setTypes(allowedTypes);
}

void TextEditor::setTypes(Type::mask allowedTypes)
{
    m_allowedTypes = allowedTypes;
    if(m_allowedTypes.hasNo(m_currentType)) {
        m_currentType = Type::No;
    }
    updateLook();
}

void TextEditor::updateLook()
{
    // if there is only one allowed type - switch to it, there is no alternatives
    auto unique = m_allowedTypes.unique();
    if(unique) {
        Type::t t(static_cast<Type::t>(unique.get()));
        switchToType(t);
        return;
    }

    if(m_fileType != Type::No) {
        // no need to change a look
        if(m_currentType == m_fileType) {
            return;
        }

        // if can switch to exact file's type - do it
        if(m_allowedTypes & m_fileType) {
            switchToType(m_fileType);
            return;
        }

        // rest cases

        // Hex | Code vs Text -> Code
        if(m_fileType == Type::Text) {
            switchToType(Type::Code);
        // Hex | Text vs Code -> Text
        } else if(m_fileType == Type::Code) {
            switchToType(Type::Text);
        // Text | Code vs Hex -> Text
        } else if(m_fileType == Type::Hex) {
            switchToType(Type::Text);
        }
    } else {
        if(m_allowedTypes & Type::Text) {
            switchToType(Type::Text);
        } else if(m_allowedTypes & Type::Code) {
            switchToType(Type::Code);
        }
    }
}

void TextEditor::switchToType(Type::t type)
{
    if(m_currentType == type) {
        return;
    }

    QString face(type == Type::Text ? "Arial" : "Consolas");
    QFont f(face, 10);

    if(type == Type::Code) {
        m_lineNumberArea.show();
        connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLineNumberAreaWidth);
        connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLineNumberArea);
        connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);

        const int tabStop = 4;

        QFontMetrics metrics(f);
        setTabStopWidth(tabStop * metrics.width(' '));

        updateLineNumberAreaWidth(0);
        highlightCurrentLine();
    } else {
        m_lineNumberArea.hide();
        setViewportMargins(0, 0, 0, 0);

        disconnect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLineNumberAreaWidth);
        disconnect(this, &TextEditor::updateRequest, this, &TextEditor::updateLineNumberArea);
        disconnect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);
    }

    setFont(f);
    setReadOnly(type == Type::Hex);

    m_currentType = type;
}

void TextEditor::openFile(const QString &fileName)
{
    m_fileName = fileName;

    QFile file(m_fileName);
    file.open(QIODevice::ReadOnly);

    bool binary = ::isBinary(file);
    Syntax::t syntax = Syntax::fromFile(fileName);
    bool code = syntax != Syntax::No;

    m_fileType = binary ?
                     Type::Hex : code ?
                         Type::Code : Type::Text;

    updateLook();

    setReadOnly(binary);

    QString face(binary || code ? "Consolas" : "Arial");
    QFont f(face, 10);
    setFont(f);

    if(binary) {
        if(m_currentType == Type::Hex) {
            setPlainText(binaryToText(file.readAll()));
        } else {
            setPlainText(tr("BINARY FILE"));
        }
        deleteHighlighter();
    } else {
        setPlainText(QString::fromUtf8(file.readAll()));
        applyHighlighter(syntax);
    }

    file.close();
}

void TextEditor::saveFile(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QTextStream ss(&file);
    ss.setCodec("UTF-8");
    ss << toPlainText();
    file.close();
}

void TextEditor::saveFile()
{
    if(!m_fileName.isEmpty()) {
        saveFile(m_fileName);
    }
}

void TextEditor::applyHighlighter()
{
    deleteHighlighter();

    m_highlighter = Syntax::getHighlighter(m_fileName);
    if(m_highlighter) {
        m_highlighter->setDocument(document());
    }
}

void TextEditor::applyHighlighter(Syntax::t syntax)
{
    deleteHighlighter();

    m_highlighter = Syntax::getHighlighter(syntax);
    if(m_highlighter) {
        m_highlighter->setDocument(document());
    }
}

void TextEditor::deleteHighlighter()
{
    if(m_highlighter) {
        m_highlighter->setDocument(nullptr);
    }
    m_highlighter = nullptr;
}

void TextEditor::onFileRenamed(const QString &fileName)
{
    m_fileName = fileName;

    if(m_fileType == Type::Text || m_fileType == Type::Code) {
        m_fileType = isCode(fileName) ? Type::Code : Type::Text;
    }

    updateLook();
    if(m_currentType != Type::Hex) {
        applyHighlighter();
    } else {
        deleteHighlighter();
    }
}

int TextEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits + 15;

    return space;
}

void TextEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void TextEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        m_lineNumberArea.scroll(0, dy);
    else
        m_lineNumberArea.update(0, rect.y(), m_lineNumberArea.width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void TextEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    m_lineNumberArea.setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void TextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(245, 245, 245);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void TextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(&m_lineNumberArea);
    painter.fillRect(event->rect(), Qt::white);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);

            painter.setPen(Qt::lightGray);
            painter.drawText(0, top, m_lineNumberArea.width()-10, fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

} // namespace memory
