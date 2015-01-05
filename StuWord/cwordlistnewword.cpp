#include "cwordlistnewword.h"
#include <QFile>
#include <qdir.h>
#include <qvector.h>
#include <qalgorithms.h>
#include <QDataStream>

CWordListNewWord::CWordListNewWord(const QString &PeoName, const QString &ListName)
{
	QDir Dir;
	if (!Dir.exists(PeoName + "/NewWordList"))
		Dir.mkdir(PeoName + "/NewWordList");
	m_ListData = new QSettings(PeoName + "/NewWordList" + "/" + ListName+ ".ini", QSettings::IniFormat);
	Key = m_ListData->childGroups();
    m_NowIndex=1;
	m_NowWord = NULL;
	m_PeoName = PeoName;
}

bool CWordListNewWord::RemoveList(QString PeoName, QString ListName)
{
	QString Path = PeoName + "/NewWordList" + "/" + ListName + ".ini";
	QByteArray ba = Path.toLatin1();
	return remove(ba) == 0;
}

CWordListNewWord::~CWordListNewWord()
{
	if (m_ListData != NULL)
		delete m_ListData;
	if (m_NowWord != NULL)
		delete m_NowWord;
}

void CWordListNewWord::AddNewWord(CWord * Word)
{
	QString TheWord = Word->GetWord();
	QString Read = Word->GetRead();
	QVector<QString> Mean = Word->GetMeans();
	m_ListData->setValue(TheWord + "/Word", TheWord);
	m_ListData->setValue(TheWord + "/Read", Read);
	m_ListData->beginWriteArray(TheWord + "/Mean");
	for (int i = 0; i < Mean.size(); i++)
	{
		m_ListData->setArrayIndex(i);
		m_ListData->setValue(QString::number(i), Mean[i]);
	}
	m_ListData->endArray();
	Key.push_back(TheWord);
}

CWord * CWordListNewWord::m_GetWord(int Index)
{
	QString StrKey = Key[Index - 1];
	QString TheWord = m_ListData->value(StrKey + "/Word").toString();
	QString Read = m_ListData->value(StrKey + "/Read").toString();
	QVector<QString> Mean;
	int Size = m_ListData->beginReadArray(StrKey + "/Mean");
	for (int i = 0; i < Size; i++)
	{
		m_ListData->setArrayIndex(i);
		Mean.push_back(m_ListData->value(QString::number(i)).toString());
	}
	m_ListData->endArray();
	return new CWord(StrKey, Read, Mean);
}

CWord* CWordListNewWord::EraseWord(CWord * Word)
{
	m_ListData->remove(Word->GetWord());
	Key = m_ListData->childGroups();
	if (Key.size() == 0)
	{
		if (m_NowWord != NULL)
			delete m_NowWord;
		return NULL;
	}
	if (m_NowIndex > Key.size())
	{
		m_NowIndex = Key.size();
	}
	if (m_NowWord != NULL)
		delete m_NowWord;
	m_NowWord = m_GetWord(m_NowIndex);
	return m_NowWord;
}

CWord* CWordListNewWord::StartLook()
{
	m_NowIndex = 1;
	if (m_NowIndex > Key.size())
		return NULL;
	m_NowWord = m_GetWord(m_NowIndex);
	return m_NowWord;
}

CWord* CWordListNewWord::GetNextWord()
{
	if (m_NowIndex<Key.size())
    {
        m_NowIndex++;
    }
	if (m_NowWord != NULL)
		delete m_NowWord;
	m_NowWord = m_GetWord(m_NowIndex);
	return m_NowWord;
}

CWord* CWordListNewWord::GetPrevWord()
{
    if(m_NowIndex>1)
    {
        m_NowIndex--;
    }
	if (m_NowWord != NULL)
		delete m_NowWord;
	m_NowWord = m_GetWord(m_NowIndex);
	return m_NowWord;
}

bool CWordListNewWord::TheWordExist(CWord * Word)
{
	QString TheWord = Word->GetWord();
	return qFind(Key.begin(), Key.end(), TheWord) != Key.end();
}

bool CWordListNewWord::HaveNext()
{
	return m_NowIndex < Key.size();
}

bool CWordListNewWord::HavePrev()
{
	return m_NowIndex > 1;
}