#include "js.h"

namespace memory {

JSHighlighter::JSHighlighter(QTextDocument *parent)
    : CLikeHighlighter(parent)
{
    //////
    /// KEYWORDS
    ////
    m_keywords = QStringList {
        QStringLiteral("abstract"),
        QStringLiteral("arguments"),
        QStringLiteral("await"),
        QStringLiteral("async"),
        QStringLiteral("boolean"),
        QStringLiteral("break"),
        QStringLiteral("byte"),
        QStringLiteral("case"),
        QStringLiteral("catch"),
        QStringLiteral("char"),
        QStringLiteral("class"),
        QStringLiteral("const"),
        QStringLiteral("continue"),
        QStringLiteral("debugger"),
        QStringLiteral("default"),
        QStringLiteral("delete"),
        QStringLiteral("do"),
        QStringLiteral("double"),
        QStringLiteral("else"),
        QStringLiteral("enum"),
        QStringLiteral("eval"),
        QStringLiteral("export"),
        QStringLiteral("extends"),
        QStringLiteral("false"),
        QStringLiteral("final"),
        QStringLiteral("finally"),
        QStringLiteral("float"),
        QStringLiteral("for"),
        QStringLiteral("function"),
        QStringLiteral("goto"),
        QStringLiteral("if"),
        QStringLiteral("implements"),
        QStringLiteral("import"),
        QStringLiteral("in"),
        QStringLiteral("instanceof"),
        QStringLiteral("int"),
        QStringLiteral("interface"),
        QStringLiteral("let"),
        QStringLiteral("long"),
        QStringLiteral("native"),
        QStringLiteral("new"),
        QStringLiteral("null"),
        QStringLiteral("package"),
        QStringLiteral("private"),
        QStringLiteral("protected"),
        QStringLiteral("public"),
        QStringLiteral("return"),
        QStringLiteral("short"),
        QStringLiteral("static"),
        QStringLiteral("super"),
        QStringLiteral("switch"),
        QStringLiteral("synchronized"),
        QStringLiteral("this"),
        QStringLiteral("throw"),
        QStringLiteral("throws"),
        QStringLiteral("transient"),
        QStringLiteral("true"),
        QStringLiteral("try"),
        QStringLiteral("typeof"),
        QStringLiteral("var"),
        QStringLiteral("void"),
        QStringLiteral("volatile"),
        QStringLiteral("while"),
        QStringLiteral("with"),
        QStringLiteral("yield"),
    };


    //////
    /// CLASSNAMES
    ////
    m_classnames = QStringList {
        QStringLiteral("Array"),
        QStringLiteral("Date"),
        QStringLiteral("hasOwnProperty"),
        QStringLiteral("Infinity"),
        QStringLiteral("isFinite"),
        QStringLiteral("isNaN"),
        QStringLiteral("isPrototypeOf"),
        QStringLiteral("length"),
        QStringLiteral("Math"),
        QStringLiteral("NaN"),
        QStringLiteral("name"),
        QStringLiteral("Number"),
        QStringLiteral("Object"),
        QStringLiteral("prototype"),
        QStringLiteral("String"),
        QStringLiteral("toString"),
        QStringLiteral("undefined"),
        QStringLiteral("valueOf"),
        QStringLiteral("getClass"),
        QStringLiteral("java"),
        QStringLiteral("JavaArray"),
        QStringLiteral("javaClass"),
        QStringLiteral("JavaObject"),
        QStringLiteral("JavaPackage"),
        QStringLiteral("alert"),
        QStringLiteral("all"),
        QStringLiteral("anchor"),
        QStringLiteral("anchors"),
        QStringLiteral("area"),
        QStringLiteral("assign"),
        QStringLiteral("blur"),
        QStringLiteral("button"),
        QStringLiteral("checkbox"),
        QStringLiteral("clearInterval"),
        QStringLiteral("clearTimeout"),
        QStringLiteral("clientInformation"),
        QStringLiteral("close"),
        QStringLiteral("closed"),
        QStringLiteral("confirm"),
        QStringLiteral("constructor"),
        QStringLiteral("crypto"),
        QStringLiteral("decodeURI"),
        QStringLiteral("decodeURIComponent"),
        QStringLiteral("defaultStatus"),
        QStringLiteral("document"),
        QStringLiteral("element"),
        QStringLiteral("elements"),
        QStringLiteral("embed"),
        QStringLiteral("embeds"),
        QStringLiteral("encodeURI"),
        QStringLiteral("encodeURIComponent"),
        QStringLiteral("escape"),
        QStringLiteral("event"),
        QStringLiteral("fileUpload"),
        QStringLiteral("focus"),
        QStringLiteral("form"),
        QStringLiteral("forms"),
        QStringLiteral("frame"),
        QStringLiteral("innerHeight"),
        QStringLiteral("innerWidth"),
        QStringLiteral("layer"),
        QStringLiteral("layers"),
        QStringLiteral("link"),
        QStringLiteral("location"),
        QStringLiteral("mimeTypes"),
        QStringLiteral("navigate"),
        QStringLiteral("navigator"),
        QStringLiteral("frames"),
        QStringLiteral("frameRate"),
        QStringLiteral("hidden"),
        QStringLiteral("history"),
        QStringLiteral("image"),
        QStringLiteral("images"),
        QStringLiteral("offscreenBuffering"),
        QStringLiteral("open"),
        QStringLiteral("opener"),
        QStringLiteral("option"),
        QStringLiteral("outerHeight"),
        QStringLiteral("outerWidth"),
        QStringLiteral("packages"),
        QStringLiteral("pageXOffset"),
        QStringLiteral("pageYOffset"),
        QStringLiteral("parent"),
        QStringLiteral("parseFloat"),
        QStringLiteral("parseInt"),
        QStringLiteral("password"),
        QStringLiteral("pkcs11"),
        QStringLiteral("plugin"),
        QStringLiteral("prompt"),
        QStringLiteral("propertyIsEnum"),
        QStringLiteral("radio"),
        QStringLiteral("reset"),
        QStringLiteral("screenX"),
        QStringLiteral("screenY"),
        QStringLiteral("scroll"),
        QStringLiteral("secure"),
        QStringLiteral("select"),
        QStringLiteral("self"),
        QStringLiteral("setInterval"),
        QStringLiteral("setTimeout"),
        QStringLiteral("status"),
        QStringLiteral("submit"),
        QStringLiteral("taint"),
        QStringLiteral("text"),
        QStringLiteral("textarea"),
        QStringLiteral("top"),
        QStringLiteral("unescape"),
        QStringLiteral("untaint"),
        QStringLiteral("window"),
        QStringLiteral("onblur"),
        QStringLiteral("onclick"),
        QStringLiteral("onerror"),
        QStringLiteral("onfocus"),
        QStringLiteral("onkeydown"),
        QStringLiteral("onkeypress"),
        QStringLiteral("onkeyup"),
        QStringLiteral("onmouseover"),
        QStringLiteral("onload"),
        QStringLiteral("onmouseup"),
        QStringLiteral("onmousedown"),
        QStringLiteral("onsubmit"),
    };
}

} // namespace memory
