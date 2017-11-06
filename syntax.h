#ifndef SYNTAX_H
#define SYNTAX_H

#include <map>
#include <QString>
#include <QFileInfo>

enum class Syntax
{
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
    Yaml
};

static const std::map<Syntax, QStringList> extensions = {
    {Syntax::ActionScript, {"as",}},
    {Syntax::Ada, {"ada",}},
    {Syntax::Asp, {"asp",}},
    {Syntax::Asm, {"asm",}},
    {Syntax::Batch, {"bat", "cmd",}},
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
    {Syntax::Makefile, {"",}},
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

bool isCode(const QString &fileName) {
    QString ext = QFileInfo(fileName).suffix();
    if(ext.isEmpty()) {
        return false;
    }
    for(auto &s : extensions) {
        if(s.second.contains(ext, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

#endif // SYNTAX_H
