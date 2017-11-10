#ifndef SYNTAX_H
#define SYNTAX_H

#include <map>
#include <QString>
#include <QFileInfo>
#include <QSyntaxHighlighter>

class Syntax {
public:
    enum t
    {
        No = 0,

        ActionScript,
        Ada,
        Asp,
        Asm,
        Batch,
        Caml,
        CMake,
        Cobol,
        CoffeeScript,
        Cpp,
        CSharp,
        Csv,
        Css,
        D,
        Diff,
        Fortran,
        Haskell,
        Html,
        Ini,
        Java,
        JS,
        Lisp,
        Lua,
        Makefile,
        Matlab,
        ObjC,
        Pascal,
        Perl,
        Php,
        PostScript,
        PowerShell,
        Python,
        R,
        Ruby,
        Rust,
        Shell,
        Scheme,
        Smalltalk,
        Sql,
        Tab,
        Tcl,
        Tex,
        TypeScript,
        VB,
        Vhdl,
        Verilog,
        Xml,
        Yaml,
    };

    static Syntax::t fromFile(const QString &fileName);

    static QSyntaxHighlighter *getHighlighter(Syntax::t syntax);
    static QSyntaxHighlighter *getHighlighter(const QString &fileName) {
        return getHighlighter(Syntax::fromFile(fileName));
    }

private:
    static const std::map<Syntax::t, QStringList> extensions;
    static std::map<Syntax::t, QSyntaxHighlighter *> highlightersPool;
};

inline bool isCode(const QString &fileName) {
    return Syntax::fromFile(fileName) != Syntax::No;
}

#endif // SYNTAX_H
