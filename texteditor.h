#ifndef MEMORYTEXTEDITOR_H
#define MEMORYTEXTEDITOR_H

#include <QPlainTextEdit>

class QSyntaxHighlighter;

namespace memory {

class TextEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    //! TextEditor type
    class Type {
    public:
        enum t_ {
            No   = 0,      //! No content

            Text = 1 << 0, //! Common editor. No lines count, readable font
            Code = 1 << 1, //! Code editor. Lines count, monospaced font
            Hex  = 1 << 2, //! Hex viewer
        };
        typedef int mask;  //! Type bitmask
    };

    explicit TextEditor(QWidget *parent = 0);
    explicit TextEditor(Type::mask type = Type::Text, QWidget *parent = 0);
    void setType(Type::mask type);
    void openFile(const QString &m_fileName); // true if text, false if binary
    void saveFile(const QString &m_fileName);
    void saveFile();
    Type::mask type() { return m_type; } //! Type of text editor
    Type::t_ content() { return m_content; } //! Current content type

public slots:
    void onFileRenamed(const QString &fileName);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateLook();
    void switchToType(Type::t_ type);

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

    friend class LineNumberArea;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

    void applyHighlighter();
    void deleteHighlighter();

    LineNumberArea m_lineNumberArea;
    QString m_fileName;
    Type::mask m_type {Type::Text | Type::Hex};
    Type::t_ m_content {Type::No};
    QSyntaxHighlighter *m_highlighter {nullptr};
};

} // namespace memory

#endif // MEMORYTEXTEDITOR_H
