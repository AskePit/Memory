#ifndef MEMORYTEXTEDITOR_H
#define MEMORYTEXTEDITOR_H

#include <QPlainTextEdit>
#include <bitset>

class QSyntaxHighlighter;

template <typename T>
class opt {
public:
    opt()
        : m_initialized(false)
        , m_val(T())
    {}
    opt(T val)
        : m_initialized(true)
        , m_val(val)
    {}
    opt(const opt &other)
        : m_initialized(other.m_initialized)
        , m_val(other.m_val)
    {}
    opt(opt &&other)
        : m_initialized(other.m_initialized)
        , m_val(std::move(other.m_val))
    {}
    opt &operator=(T val) {
        m_val = val;
        m_initialized = true;
        return *this;
    }

    operator bool() const { return m_initialized; }
    T &get() { return m_val; }
    const T &get() const { return m_val; }
    T get_or(T def) const { return m_initialized? m_val : def; }

private:
    bool m_initialized {false};
    T m_val;
};

template <typename base_type = unsigned long>
class mask {
public:
    mask(base_type val)
        : m(val)
    {}
    mask(const mask &other)
        : m(other.m)
    {}
    mask &operator=(base_type val) {
        m = val; return *this;
    }

    bool operator&(base_type t) const { return m & t; }
    bool test(base_type t) const { return *this & t; }
    bool hasNo(base_type t) const { return !(*this & t); }

    mask &operator|=(base_type t) {
        m |= t;
        return *this;
    }

    mask &set(base_type t) { return *this |= t; }

    mask &clear() {
        m = 0;
        return *this;
    }

    mask &clear(base_type t) {
        m & (~t);
        return this;
    }

    size_t count() const {
        std::bitset<sizeof(base_type)*CHAR_BIT> bits(m);
        return bits.count();
    }

    opt<base_type> unique() {
        opt<base_type> u;
        if(count() == 1) {
            u = m;
        }
        return u;
    }

private:
    base_type m {0};
};

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
        static constexpr int count = 3;
        using mask = ::mask<>; //! Type bitmask
    };

    explicit TextEditor(QWidget *parent = 0);
    explicit TextEditor(Type::mask types = Type::Text, QWidget *parent = 0);
    void setTypes(Type::mask types);
    void switchToType(Type::t_ types);
    void openFile(const QString &m_fileName);
    void saveFile(const QString &m_fileName);
    void saveFile();
    Type::mask types() { return m_allowedTypes; } //! Types of text editor
    Type::t_ currentType() { return m_currentType; } //! Current content type

public slots:
    void onFileRenamed(const QString &fileName);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateLook();

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

    Type::mask m_allowedTypes {Type::Text | Type::Hex}; //! Types allowed by TextEditor
    Type::t_ m_currentType {Type::No}; //! Current TextEditorType
    Type::t_ m_fileType {Type::No}; //! Type of a current file

    QSyntaxHighlighter *m_highlighter {nullptr};
};

} // namespace memory

#endif // MEMORYTEXTEDITOR_H
