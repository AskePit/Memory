#include "texteditor.h"
#include "highlighters/highlighters.h"
#include "utils.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextStream>

namespace memory {

TextEditor::TextEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_lineNumberArea(this)
{
    connect(this, &TextEditor::blockCountChanged, this, &TextEditor::updateLineNumberAreaWidth);
    connect(this, &TextEditor::updateRequest, this, &TextEditor::updateLineNumberArea);
    connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::highlightCurrentLine);

    QFont font = QFont(QStringLiteral("Consolas"), 10);
    setFont(font);

    const int tabStop = 4;

    QFontMetrics metrics(font);
    setTabStopWidth(tabStop * metrics.width(' '));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

bool TextEditor::openFile(const QString &fileName)
{
    m_fileName = fileName;

    QFile file(m_fileName);
    file.open(QIODevice::ReadOnly);

    bool binary = isBinary(file);
    if(binary) {
        //ui->textEditor->setPlainText(tr("BINARY FILE"));
        setPlainText(binaryToText(file.readAll()));
        deleteHighlighter();
    } else {
        setPlainText(QString::fromUtf8(file.readAll()));
        applyHighlighter();
    }

    setReadOnly(binary);
    file.close();

    return !binary;
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
    QString prefix("memory::");
    QString suffix("Highlighter");
    QString id;
    if(m_highlighter) {
        id = m_highlighter->metaObject()->className(); // memory::CppHighlighter
        id = id.mid(prefix.size());                  // CppHighlighter
        id.truncate(id.count() - suffix.count());    // Cpp
    }

    const QString cppId("Cpp");
    const QString jsId("JS");
    const QString tabId("Tab");

    bool doSwitch = false;

    auto c = Qt::CaseInsensitive;

    if(m_fileName.endsWith(QLatin1String(".cpp"), c) || m_fileName.endsWith(QLatin1String(".h"), c) || m_fileName.endsWith(QLatin1String(".c"), c)) {
        doSwitch = (id != cppId);
        id = cppId;
    } else if(m_fileName.endsWith(QLatin1String(".js"), c)) {
        doSwitch = (id != jsId);
        id = jsId;
    } else if(m_fileName.endsWith(QLatin1String(".tab"), c)) {
        doSwitch = (id != tabId);
        id = tabId;
    } else {
        doSwitch = !id.isEmpty();
        id.clear();
    }

    if(!doSwitch) {
        return;
    }

    deleteHighlighter();

    QTextDocument *doc = document();

    if(id == cppId) {
        m_highlighter = new CppHighlighter(doc);
    } else if(id == jsId) {
        m_highlighter = new JSHighlighter(doc);
    } else if(id == tabId) {
        m_highlighter = new TabHighlighter(doc);
    }
}

void TextEditor::deleteHighlighter()
{
    if(m_highlighter) {
        delete m_highlighter;
        m_highlighter = nullptr;
    }
}

void TextEditor::onFileRenamed(const QString &fileName)
{
    m_fileName = fileName;
    applyHighlighter();
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
