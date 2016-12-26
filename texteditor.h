#ifndef MEMORYTEXTEDITOR_H
#define MEMORYTEXTEDITOR_H

#include <QPlainTextEdit>

namespace memory {

class LineNumberArea;

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;
};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(TextEditor *editor)
        : QWidget(editor)
    {
        textEditor = editor;
    }

    QSize sizeHint() const override {
        return QSize(textEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        textEditor->lineNumberAreaPaintEvent(event);
    }

private:
    TextEditor *textEditor;
};

} // namespace memory

#endif // MEMORYTEXTEDITOR_H
