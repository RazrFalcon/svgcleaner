#ifndef MINDEF_H
#define MINDEF_H

#include <QSet>

#define QL1S(x) QLatin1String(x)
#define QL1C(x) QLatin1Char(x)

typedef QSet<QString> StringSet;
typedef QHash<uint,QString> IntHash;
typedef QHash<QString,QString> StringHash;
typedef QMap<QString,QString> StringMap;

uint hash(const QChar *p, int n);
uint qHash(const QString &key);

// we cannot use static variables while unit testing
// so replace 'static' with nothing
#ifdef U_TEST
# define u_static
#else
# define u_static static
#endif

#endif // MINDEF_H
