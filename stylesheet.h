#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <QString>
#include <QHash>
#include <QColor>

typedef const QString & CStringRef;

class QObject;

class StyleSheet
{
public:
    StyleSheet(CStringRef className);

    CStringRef get() const;
    const QString operator[](CStringRef key) const;

    void set(CStringRef key, CStringRef val, CStringRef unit = "");
    void set(CStringRef key, int val, CStringRef unit = "");
    void set(CStringRef key, float val, CStringRef unit = "");
    void set(CStringRef key, int r, int g, int b);
    void addPiece(CStringRef piece);

private:
    QString className;
    QHash<QString, QString> attributes;
    QStringList pieces;
    mutable QString sheet;
    mutable bool changed;
};

#endif // STYLESHEET_H
