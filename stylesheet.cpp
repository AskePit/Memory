#include "stylesheet.h"

#include <QObject>
#include <QDebug>

StyleSheet::StyleSheet(CStringRef className)
    : className(className)
    , changed(false)
{
}

CStringRef StyleSheet::get() const
{
    if(!changed) {
        return sheet;
    }

    sheet = className;
    sheet += "{";

    for (QHash<QString, QString>::const_iterator attr = attributes.begin(); attr != attributes.end(); ++attr) {
        sheet += attr.key();
        sheet += ": ";
        sheet += attr.value();
        sheet += "; ";
    }

    sheet += "}";

    qDebug() << "pieces:" << pieces;
    sheet += pieces.join(' ');

    changed = false;
    return sheet;
}

const QString StyleSheet::operator[](CStringRef key) const
{
    return attributes.value(key, "");
}


void StyleSheet::set(CStringRef key, CStringRef val, CStringRef unit)
{
    attributes[key] = val + unit;
    changed = true;
}

void StyleSheet::set(CStringRef key, int val, CStringRef unit)
{
    attributes[key] = QString::number(val) + unit;
    changed = true;
}

void StyleSheet::set(CStringRef key, float val, CStringRef unit)
{
    attributes[key] = QString::number(val) + unit;
    changed = true;
}

static QString to2Hex(int val) {
    QString hex;

    if(val < 0x10) {
        hex = "0";
    }

    hex += QString::number(val, 16);
    return hex;
}

void StyleSheet::set(CStringRef key, int r, int g, int b)
{
    attributes[key] = QString("#") + to2Hex(r) + to2Hex(g) + to2Hex(b);
    changed = true;
}

void StyleSheet::addPiece(CStringRef piece)
{
    pieces << piece;
    changed = true;
    //qDebug() << pieces;
}
