/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "cplusplus.h"

namespace memory {

CppHighlighter::CppHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    auto addRule = [&](QTextCharFormat &format, const QString &regexp){
        rule.pattern = QRegExp(regexp);
        rule.format = format;
        highlightingRules.append(rule);
    };

    auto applyList = [&](QStringList &list, QTextCharFormat &format, const QString &regexp){
        for (QString &pattern : list) {
            pattern = QString(regexp).arg(pattern);
            addRule(format, pattern);
        }
    };

    //////
    /// PREPROCESSOR
    ////
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

    //////
    /// COMMENTS
    ////
    singleLineCommentFormat.setForeground(Qt::darkGray);
    addRule(singleLineCommentFormat, "//[^\n]*");

    multiLineCommentFormat.setForeground(Qt::darkGray);

    commentStartExpression = QRegExp("/\\*");
    commentEndExpression = QRegExp("\\*/");

    //////
    /// FUNCTIONS
    ////
    /*functionFormat.setFontItalic(true);
    addRule(functionFormat, "\\b[A-Za-z0-9_]+(?=\\()");*/

    //////
    /// STRINGS
    ////
    quotationFormat.setForeground(Qt::darkGreen);
    addRule(quotationFormat, "\".*\"");

    //////
    /// LITERALS
    ////
    constantFormat.setForeground(Qt::darkGray);
    addRule(constantFormat, "\\b[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?[uflUFL]{0,3}\\b");
    addRule(constantFormat, "\\b0[x][0-9a-fA-F]+\\b");
    addRule(constantFormat, "\\b0[b][0-1]+\\b");
    addRule(constantFormat, "\\b0[o][0-7]+\\b");
}

void CppHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = commentStartExpression.indexIn(text);

    while (startIndex >= 0) {
        int endIndex = commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

} // namespace memory
