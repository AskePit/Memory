#include "syntax.h"
#include "highlighters/highlighters.h"

const std::map<Syntax::t, QStringList> Syntax::extensions = {
    {Syntax::ActionScript, {"as",}},
    {Syntax::Ada, {"ada",}},
    {Syntax::Asp, {"asp",}},
    {Syntax::Asm, {"asm",}},
    {Syntax::Batch, {"bat", "cmd", "btm",}},
    {Syntax::Caml, {"ml",}},
    {Syntax::CMake, {"cmake",}},
    {Syntax::Cobol, {"cob", "cbl"}},
    {Syntax::CoffeeScript, {"coffee",}},
    {Syntax::Cpp, {"c", "h", "cpp", "hpp",}},
    {Syntax::CSharp, {"cs",}},
    {Syntax::Csv, {"csv",}},
    {Syntax::Css, {"css",}},
    {Syntax::D, {"d",}},
    {Syntax::Diff, {"diff", "patch",}},
    {Syntax::Fortran, {"f", "for", "ftn", "f90", "f95", "f03", "f08",}},
    {Syntax::Haskell, {"hs", "lhs",}},
    {Syntax::Html, {"html", "htm", "htmls",}},
    {Syntax::Ini, {"ini",}},
    {Syntax::Java, {"java",}},
    {Syntax::JS, {"js", "json",}},
    {Syntax::Lisp, {"lisp",}},
    {Syntax::Lua, {"lua",}},
    {Syntax::Makefile, {"mk",}},
    {Syntax::Matlab, {"mat",}},
    {Syntax::ObjC, {"m", "mm"}},
    {Syntax::Pascal, {"pas", "p", "pl", "pascal", "pp"}},
    {Syntax::Perl, {"perl",}},
    {Syntax::Php, {"php", "php3", "php4"}},
    {Syntax::PostScript, {"ps",}},
    {Syntax::PowerShell, {"ps1",}},
    {Syntax::Python, {"py",}},
    {Syntax::R, {"r",}},
    {Syntax::Ruby, {"rb", "rbw"}},
    {Syntax::Rust, {"rust",}},
    {Syntax::Shell, {"sh",}},
    {Syntax::Scheme, {"ss", "sls", "scm"}},
    {Syntax::Smalltalk, {"st",}},
    {Syntax::Sql, {"sql",}},
    {Syntax::Tab, {"tab",}},
    {Syntax::Tcl, {"tcl",}},
    {Syntax::Tex, {"tex", "latex"}},
    {Syntax::TypeScript, {"ts",}},
    {Syntax::VB, {"bas", "vb", "vbp"}},
    {Syntax::Vhdl, {"vhdl",}},
    {Syntax::Verilog, {"v", "vh",}},
    {Syntax::Xml, {"xml", "res"}},
    {Syntax::Yaml, {"yaml",}},
};

std::map<Syntax::t, QSyntaxHighlighter *> Syntax::highlightersPool;

Syntax::t Syntax::fromFile(const QString &fileName) {
    QString ext = QFileInfo(fileName).suffix();
    if(ext.isEmpty()) {
        return Syntax::No;
    }

    if(fileName == "Makefile") {
        return Syntax::Makefile;
    }

    for(auto &s : extensions) {
        if(s.second.contains(ext, Qt::CaseInsensitive)) {
            return s.first;
        }
    }
    return Syntax::No;
}

QSyntaxHighlighter *Syntax::getHighlighter(Syntax::t syntax) {
    if(syntax == Syntax::No) {
        return nullptr;
    }

    auto it = highlightersPool.find(syntax);
    if(it == highlightersPool.end()) {
        QSyntaxHighlighter *highlighter = nullptr;

        switch(syntax) {
            case Syntax::Cpp: highlighter = new memory::CppHighlighter; break;
            case Syntax::JS: highlighter = new memory::JSHighlighter; break;
            case Syntax::Python: highlighter = new memory::PythonHighlighter; break;
            case Syntax::Tab: highlighter = new memory::TabHighlighter; break;
            default: ;
        }

        if(highlighter) {
            highlightersPool.insert( std::make_pair(syntax, highlighter) );
        }

        return highlighter;
    } else {
        return it->second;
    }
}
