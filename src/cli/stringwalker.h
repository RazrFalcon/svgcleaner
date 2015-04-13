#ifndef STRINGWALKER_H
#define STRINGWALKER_H

#include <QString>

class StringWalker
{
public:
    enum Opt { NoSkip, SkipComma };

    StringWalker(const QString &text);
    StringWalker(const QChar *astr, int size);
    int jumpTo(const QChar &c);
    int jumpToSpace();
    QString readBefore(int len) const;
    uint readBeforeId(int len) const;
    void next();
    void next(int count);
    void skipSpaces();
    bool atEnd() const;
    bool isValid() const;
    const QChar& current() const;
    double number(Opt opt = SkipComma, bool *ok = 0);
    static bool isSpace(ushort ch);

private:
    const QChar *str;
    const QChar *end;

    static bool isDigit(ushort ch);
};

#endif // STRINGWALKER_H
