#include "cplusplus.h"

namespace memory {

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : CLikeHighlighter(parent)
{
    //////
    /// PREPROCESSOR
    ////

    QTextCharFormat ppFormat;
    ppFormat.setForeground(QColor(45, 45, 45));
    ppFormat.setFontWeight(QFont::Bold);
    QStringList ppPatterns = {
        "define","undef", "include", "if", "ifdef", "ifndef", "else",
        "elif", "endif", "line", "error", "warning", "pragma"
    };
    applyList(ppPatterns, ppFormat, "^\\s*#%1\\b");


    //////
    /// KEYWORDS
    ////

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(0, 128, 0));
    QStringList keywordPatterns = {
        "alignas", "alignof", "and", "and_eq", "asm", "auto",
        "bitand", "bitor", "bool", "break",
        "case", "catch", "char", "char16_t", "char32_t", "class", "compl", "const", "constexpr", "const_cast", "continue",
        "decltype", "default", "delete", "do", "double", "dynamic_cast",
        "else", "enum", "explicit", "export", "extern",
        "false", "final", "float", "for", "friend",
        "goto",
        "if", "inline", "int",
        "long",
        "mutable",
        "namespace", "new", "noexcept", "not", "not_eq", "nullptr",
        "operator", "or", "or_eq", "override",
        "private", "protected", "public",
        "register", "reinterpret_cast", "return",
        "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch",
        "template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
        "union", "unsigned", "using",
        "virtual", "void", "volatile",
        "wchar_t", "while", "xor", "xor_eq",
    };
    applyList(keywordPatterns, keywordFormat, "\\b%1\\b");


    //////
    /// CLASSNAMES
    ////

    QTextCharFormat classFormat;
    classFormat.setForeground(Qt::darkMagenta);
    QStringList classPatterns = {
        "std",
        "double_t",
        "float_t",
        "va_list",
        "ptrdiff_t",
        "max_align_t",
        "nullptr_t",
        "intmax_t", "uintmax_t",
        "int8_t", "int16_t", "int32_t", "int64_t",
        "uint8_t", "uint16_t", "uint32_t", "uint64_t",
        "int_least8_t", "int_least16_t", "int_least32_t", "int_least64_t",
        "uint_least8_t", "uint_least16_t", "uint_least32_t", "uint_least64_t",
        "int_fast8_t", "int_fast16_t", "int_fast32_t", "int_fast64_t",
        "uint_fast8_t", "uint_fast16_t", "uint_fast32_t", "uint_fast64_t",
        "intptr_t", "uintptr_t",
        "FILE",
        "fpos_t",
        "size_t",
        "div_t",
        "ldiv_t",
        "lldiv_t",
        "clock_t",
        "time_t",
        "tm",
        "char16_t", "char32_t",
        "mbstate_t",
        "wchar_t",
        "wint_t",
        "wctrans_t",
        "wctype_t",
        "array",
        "deque",
        "forward_list",
        "list",
        "map", "multimap", "unordered_map", "unordered_multimap",
        "queue",
        "priority_queue",
        "set", "multiset", "unordered_set", "unordered_multiset",
        "stack",
        "vector",
        "basic_ifstream", "basic_ofstream", "basic_fstream", "basic_filebuf",
        "ifstream", "ofstream", "fstream", "filebuf",
        "wifstream", "wofstream", "wfstream", "wfilebuf",
        "basic_ios",
        "fpos",
        "ios", "ios_base",
        "wios",
        "io_errc",
        "streamoff", "streampos", "streamsize", "wstreampos",
        "cin", "cout", "cerr", "clog",
        "wcin", "wcout", "wcerr", "wclog",
        "istream", "iostream", "wistream", "wiostream", "ostream", "wostream",
        "endl", "ends", "flush",
        "basic_istringstream", "basic_ostringstream", "basic_stringstream", "basic_stringbuf",
        "istringstream", "ostringstream", "stringstream", "stringbuf",
        "wistringstream", "wostringstream", "wstringstream", "wstringbuf",
        "basic_streambuf", "streambuf", "wstreambuf",
        "atomic", "atomic_flag",
        "memory_order",
        "condition_variable", "condition_variable_any",
        "cv_status",
        "promise",
        "packaged_task",
        "future",
        "shared_future",
        "future_error",
        "future_errc",
        "future_status",
        "launch",
        "mutex", "recursive_mutex", "timed_mutex", "recursive_timed_mutex",
        "lock_guard",
        "unique_lock",
        "once_flag",
        "adopt_lock_t",
        "defer_lock_t",
        "try_to_lock_t",
        "thread",
        "this_thread",
        "bitset",
        "duration",
        "time_point",
        "system_clock", "steady_clock", "high_resolution_clock",
        "hours", "minutes", "seconds", "milliseconds", "microseconds", "nanoseconds",
        "complex",
        "exception",
        "bad_exception",
        "nested_exception",
        "exception_ptr",
        "terminate_handler",
        "unexpected_handler",
        "initializer_list",
        "iterator",
        "numeric_limits",
        "locale",
        "auto_ptr", "auto_ptr_ref", "shared_ptr", "weak_ptr", "unique_ptr",
        "default_delete",
        "ratio",
        "regex",
        "basic_string",
        "string",
        "u16string",
        "u32string",
        "wstring",
        "tuple",
        "tuple_size",
        "tuple_element",
        "type_index",
        "type_info",
        "bad_cast",
        "bad_typeid",
        "pair",
        "valarray",
        "slice",
        "gslice",
        "slice_array",
        "gslice_array",
        "mask_array",
        "indirect_array",
    };
    applyList(classPatterns, classFormat, "\\b%1\\b");

    addRule(classFormat, "\\bQ[A-Za-z]+\\b");
}

} // namespace memory
