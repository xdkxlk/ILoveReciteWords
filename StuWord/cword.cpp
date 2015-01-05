#include "cword.h"

CWord::CWord(QSettings *WordPack, int ID, QObject *parent) :
    QObject(parent)
{
    QString IDStr = QString::number(ID);
//    int index=WordPack->value("num").toInt();
    m_Word = WordPack->value(IDStr+"/word").toString();
    m_Read = WordPack->value(IDStr+"/read").toString();
    int Size = WordPack->beginReadArray(IDStr+"/mean");
    for(int i=0;i<Size;i++)
    {
        QString temp;
        WordPack->setArrayIndex(i);
        temp = WordPack->value(QString::number(i)).toString();
        m_Mean.push_back(temp);
    }
    WordPack->endArray();
    m_ID = ID;
}

CWord::CWord(QString Word, QString Read, QVector<QString> Mean, QObject *parent) :
	QObject(parent)
{
	m_Word = Word;
	m_Read = Read;
	m_Mean = Mean;
}

int CWord::GetID()
{
    return m_ID;
}

QString CWord::GetWord()
{
    return m_Word;
}

QString CWord::GetRead()
{
    return m_Read;
}

QVector<QString> CWord::GetMeans()
{
    return m_Mean;
}
