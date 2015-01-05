#include "cwordlist.h"

QSettings * CWordList::WordListConfig = new QSettings("WordConfig.ini", QSettings::IniFormat);

CWordList::CWordList()
{
}

CWordList::~CWordList()
{
}

QStringList CWordList::GetPackName()
{
    return WordListConfig->childGroups();
}

int CWordList::GetPackNum(const QString &Name)
{
    QString Path = WordListConfig->value(Name+"/Path").toString();
    QSettings temp(Path, QSettings::IniFormat);
    return temp.value("Num").toInt();
}

QString CWordList::GetPath(const QString &Name)
{
    return WordListConfig->value(Name+"/Path").toString();
}
