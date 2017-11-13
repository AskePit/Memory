#include "rust.h"

#include <QRegularExpressionMatchIterator>

namespace memory {

RustHighlighter::RustHighlighter(QTextDocument *parent)
    : CLikeHighlighter(parent)
{
    //////
    /// KEYWORDS
    ////
    m_keywords = QStringList {
        QStringLiteral("as"),
        QStringLiteral("box"),
        QStringLiteral("break"),
        QStringLiteral("const"),
        QStringLiteral("continue"),
        QStringLiteral("crate"),
        QStringLiteral("else"),
        QStringLiteral("enum"),
        QStringLiteral("extern"),
        QStringLiteral("false"),
        QStringLiteral("fn"),
        QStringLiteral("for"),
        QStringLiteral("if"),
        QStringLiteral("impl"),
        QStringLiteral("in"),
        QStringLiteral("let"),
        QStringLiteral("loop"),
        QStringLiteral("match"),
        QStringLiteral("mod"),
        QStringLiteral("move"),
        QStringLiteral("mut"),
        QStringLiteral("pub"),
        QStringLiteral("ref"),
        QStringLiteral("return"),
        QStringLiteral("self"),
        QStringLiteral("static"),
        QStringLiteral("struct"),
        QStringLiteral("super"),
        QStringLiteral("trait"),
        QStringLiteral("true"),
        QStringLiteral("type"),
        QStringLiteral("unsafe"),
        QStringLiteral("use"),
        QStringLiteral("where"),
        QStringLiteral("while"),

        QStringLiteral("catch"),
        QStringLiteral("union"),

        QStringLiteral("abstract"),
        QStringLiteral("alignof"),
        QStringLiteral("become"),
        QStringLiteral("box"),
        QStringLiteral("do"),
        QStringLiteral("final"),
        QStringLiteral("macro"),
        QStringLiteral("offsetof"),
        QStringLiteral("override"),
        QStringLiteral("priv"),
        QStringLiteral("proc"),
        QStringLiteral("pure"),
        QStringLiteral("sizeof"),
        QStringLiteral("typeof"),
        QStringLiteral("unsized"),
        QStringLiteral("virtual"),
        QStringLiteral("yield"),
    };


    //////
    /// CLASSNAMES
    ////
    m_classnames = QStringList {
        QStringLiteral("bool"),
        QStringLiteral("char"),
        QStringLiteral("f32"),
        QStringLiteral("f64"),
        QStringLiteral("i8"),
        QStringLiteral("i16"),
        QStringLiteral("i32"),
        QStringLiteral("i64"),
        QStringLiteral("i128"),
        QStringLiteral("isize"),
        QStringLiteral("u8"),
        QStringLiteral("u16"),
        QStringLiteral("u32"),
        QStringLiteral("u64"),
        QStringLiteral("u128"),
        QStringLiteral("usize"),
        QStringLiteral("String"),
        QStringLiteral("str"),
        QStringLiteral("Self"),

        QStringLiteral("std"),

        QStringLiteral("Cow"),
        QStringLiteral("Borrow"),
        QStringLiteral("BorrowMut"),
        QStringLiteral("ToOwned"),
        QStringLiteral("Box"),
        QStringLiteral("FnBox"),
        QStringLiteral("Cell"),
        QStringLiteral("Ref"),
        QStringLiteral("RefCell"),
        QStringLiteral("RefMut"),
        QStringLiteral("UnsafeCell"),
        QStringLiteral("Eq"),
        QStringLiteral("Ord"),
        QStringLiteral("PartialEq"),
        QStringLiteral("PartialOrd"),
        QStringLiteral("BTreeMap"),
        QStringLiteral("BTreeSet"),
        QStringLiteral("BinaryHeap"),
        QStringLiteral("HashMap"),
        QStringLiteral("HashSet"),
        QStringLiteral("LinkedList"),
        QStringLiteral("VecDeque"),
        QStringLiteral("CStr"),
        QStringLiteral("CString"),
        QStringLiteral("Stderr"),
        QStringLiteral("StderrLock"),
        QStringLiteral("Stdin"),
        QStringLiteral("StdinLock"),
        QStringLiteral("Stdout"),
        QStringLiteral("StdoutLock"),
        QStringLiteral("stderr"),
        QStringLiteral("stdin"),
        QStringLiteral("stdout"),
        QStringLiteral("Add"),
        QStringLiteral("AddAssign"),
        QStringLiteral("BitAnd"),
        QStringLiteral("BitAndAssign"),
        QStringLiteral("BitOr"),
        QStringLiteral("BitOrAssign"),
        QStringLiteral("BitXor"),
        QStringLiteral("BitXorAssign"),
        QStringLiteral("Deref"),
        QStringLiteral("DerefMut"),
        QStringLiteral("Div"),
        QStringLiteral("DivAssign"),
        QStringLiteral("Drop"),
        QStringLiteral("Fn"),
        QStringLiteral("FnMut"),
        QStringLiteral("FnOnce"),
        QStringLiteral("Index"),
        QStringLiteral("IndexMut"),
        QStringLiteral("Mul"),
        QStringLiteral("MulAssign"),
        QStringLiteral("Neg"),
        QStringLiteral("Not"),
        QStringLiteral("Rem"),
        QStringLiteral("RemAssign"),
        QStringLiteral("Shl"),
        QStringLiteral("ShlAssign"),
        QStringLiteral("Shr"),
        QStringLiteral("ShrAssign"),
        QStringLiteral("Sub"),
        QStringLiteral("SubAssign"),
        QStringLiteral("Rc"),
        QStringLiteral("Weak"),
        QStringLiteral("Arc"),
        QStringLiteral("Mutex"),
        QStringLiteral("MutexGuard"),
        QStringLiteral("RwLock"),
        QStringLiteral("RwLockReadGuard"),
        QStringLiteral("RwLockWriteGuard"),
        QStringLiteral("Copy"),
        QStringLiteral("Clone"),
        QStringLiteral("Display"),
        QStringLiteral("Debug"),
    };


}

void RustHighlighter::highlightBlock(const QString &text)
{
    m_macrosFormat.setForeground(QColor(100, 100, 100));
    QString macroRegexp = "[\\w\\d_]+!";
    QRegularExpressionMatchIterator matchIterator = QRegularExpression(macroRegexp).globalMatch(text);
    while (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        setFormat(match.capturedStart(), match.capturedLength(), m_macrosFormat);
    }

    CLikeHighlighter::highlightBlock(text);
}

} // namespace memory
