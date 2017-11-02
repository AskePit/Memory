#ifndef MEMORYTEXTEDITOR_H
#define MEMORYTEXTEDITOR_H

#include <QPlainTextEdit>

class QSyntaxHighlighter;

namespace memory {

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    enum class Type {
        PlainText,
        Code,
        Hex
    };

    explicit TextEditor(QWidget *parent = 0);
    void openFile(const QString &m_fileName); // true if text, false if binary
    void saveFile(const QString &m_fileName);
    void saveFile();
    void applyHighlighter();
    void deleteHighlighter();
    bool isBinary() { return m_binary; }

public slots:
    void onFileRenamed(const QString &fileName);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    friend class LineNumberArea;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
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

    LineNumberArea m_lineNumberArea;
    QString m_fileName;
    bool m_binary {false};
    QSyntaxHighlighter *m_highlighter {nullptr};
};

} // namespace memory

#endif // MEMORYTEXTEDITOR_H
