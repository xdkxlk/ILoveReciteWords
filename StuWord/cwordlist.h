#ifndef CWORDLIST_H
#define CWORDLIST_H

#include <QSettings>
#include <QStringList>
#include <QString>

class CWordList
{
public:
    CWordList();
    ~CWordList();
    static QStringList GetPackName();
    static int GetPackNum(const QString &Name);
    static QString GetPath(const QString &Name);
private:
    static QSettings * WordListConfig;
};

#endif // CWORDLIST_H
