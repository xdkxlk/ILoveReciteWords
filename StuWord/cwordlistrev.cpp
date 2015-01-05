#include "cwordlistrev.h"
#include <qdir.h>
#include <qalgorithms.h>
#include <algorithm>
#include <qfile.h>
#include <qtextstream.h>
#include <qdatetime.h>

//5 * 60 * 1000, 30 * 60 * 1000, 12 * 60 * 60 * 1000
const int Rank[10] = { 5 * 1000, 30  * 1000,  60 * 60 * 1000, 24 * 60 * 60 * 1000, 24 * 60 * 60 * 2 * 1000, 24 * 60 * 60 * 4 * 1000, 24 * 60 * 60 * 7 * 1000, 24 * 60 * 60 * 15 };
const int RankSize = 8;
using std::vector;
bool WListCmp(CWordListRev::WList a, CWordListRev::WList b);

CWordListRev::CWordListRev(QString PeoName, QObject *parent) :
QObject(parent)
{
	m_Name = PeoName;
	m_RemainTime = -1;
	m_NowIndex = 1;
	m_NowWord = NULL;
	QDateTime NowTime = QDateTime::currentDateTime();
	QDateTime LastTime;
	QDir Dir;
	if (!Dir.exists(PeoName + "/RevList"))
		Dir.mkdir(PeoName + "/RevList");
	m_RevConfig = new QSettings(PeoName + "/RevList/RevConfig.ini", QSettings::IniFormat);
	m_NeedReviewList = new QSettings(PeoName + "/RevList/RevList.ini", QSettings::IniFormat);
	m_TempRevList = new QSettings(PeoName + "/RevList/TempList.ini", QSettings::IniFormat);
	LastTime = m_RevConfig->value("LastRunTime", QDateTime::currentDateTime()).toDateTime();
	qint64 PassedTime = LastTime.msecsTo(NowTime); //本次运行与上次运行相差的秒数
	QSettings PeoConfig("People.ini", QSettings::IniFormat);
	m_MaxTempSize = PeoConfig.value(PeoName + "/DayReciteNum").toInt();
	QStringList RevListName = m_RevConfig->childGroups();
	for (int i = 0; i < RevListName.size(); i++)
	{
		int Temp = m_RevConfig->value(RevListName[i] + "/RemainTime").toInt();
		if (Temp <= PassedTime) //再次运行时的时间已经超过了剩余时间
		{
			int NowNextTime, NowRank;
			int PrevRank = m_RevConfig->value(RevListName[i] + "/Rank").toInt();
			m_GetNowRemainTime(PrevRank, Temp, PassedTime, NowNextTime, NowRank);
			if (NowNextTime != -1)
			{
				Temp = NowNextTime;
				m_RevConfig->setValue(RevListName[i] + "/Rank", NowRank);
				m_RevTheListNow(RevListName[i]);
			}
			else
			{
				m_RevConfig->remove(RevListName[i]);
				m_RevTheListNow(RevListName[i]);
				continue;
			}
		}
		else
			Temp -= PassedTime;
		if (Temp != 0)
		{
			if (m_RemainTime == -1)
				m_RemainTime = Temp;
			else if (m_RemainTime > Temp)
				m_RemainTime = Temp;
		}
		WList Read;
		Read.ListName = RevListName[i];
		Read.Time = Temp;
		m_ListCon.push_back(Read);
	}
	std::sort(m_ListCon.begin(), m_ListCon.end(), WListCmp);
	m_Timer = new QTimer(this);
	if (m_RemainTime > 0)
	{
		m_Timer->start(m_RemainTime);
	}
	connect(m_Timer, SIGNAL(timeout()), this, SLOT(The_Time_Out()));

}

bool CWordListRev::HaveRev()
{
	m_WordList = m_NeedReviewList->childGroups();
	if (m_WordList.size() != 0)
		return true;
	else
		return false;
}

CWordListRev::~CWordListRev()
{
	delete m_Timer;
	delete m_RevConfig;
	delete m_NeedReviewList;
	if (m_NowWord != NULL)
		delete m_NowWord;
}

void CWordListRev::Save()
{
	int Time = m_Timer->remainingTime();
	m_Timer->stop();
	m_RevConfig->setValue("LastRunTime", QDateTime::currentDateTime());
	int UsedTime = m_RemainTime - Time;
	for (int i = 0; i < m_ListCon.size(); i++)
	{
		m_ListCon[i].Time -= UsedTime;
		m_RevConfig->setValue(m_ListCon[i].ListName + "/RemainTime", m_ListCon[i].Time);
	}
}

void CWordListRev::The_Time_Out()
{
	for (int i = 0; i < m_ListCon.size(); i++)
	{
		m_ListCon[i].Time -= m_RemainTime;
	}
	WList Temp = m_ListCon[m_ListCon.size() - 1];
	int OLRank = m_RevConfig->value(Temp.ListName + "/Rank", 0).toInt();
	OLRank++;
	if (OLRank >= RankSize)
	{
		m_ListCon.pop_back();
		m_RevConfig->remove(Temp.ListName);
	}
	else
	{
		m_RevConfig->setValue(Temp.ListName + "/Rank", OLRank);
		m_ListCon[m_ListCon.size() - 1].Time = Rank[OLRank];
		std::sort(m_ListCon.begin(), m_ListCon.end(), WListCmp);
	}
	//差把到时间的list放入m_NeedReviewList；
	m_RevTheListNow(Temp.ListName);

	if (m_ListCon.size() > 0)
	{
		m_RemainTime = m_ListCon[m_ListCon.size() - 1].Time;
		m_Timer->stop();
		m_Timer->start(m_RemainTime);
	}
	emit HaveNewRev();
}

void CWordListRev::AddWords(CWord* Word)
{
	QString TheWord = Word->GetWord();
	QString Temp = m_TempRevList->value(TheWord + "/Word", "").toString();
	if (Temp == "")
	{
		int Num = m_TempRevList->value("Num", 0).toInt();
		Num++;
		QString Read = Word->GetRead();
		QVector<QString> Mean = Word->GetMeans();
		m_TempRevList->setValue(TheWord + "/Word", TheWord);
		m_TempRevList->setValue(TheWord + "/Read", Read);
		m_TempRevList->beginWriteArray(TheWord + "/Mean");
		for (int i = 0; i < Mean.size(); i++)
		{
			m_TempRevList->setArrayIndex(i);
			m_TempRevList->setValue(QString::number(i), Mean[i]);
		}
		m_TempRevList->endArray();
		m_TempRevList->setValue("Num", Num);
		if (Num == m_MaxTempSize)
		{
			m_FinishTempFile();
		}
	}
}

void CWordListRev::m_FinishTempFile()
{
	QStringList Test = m_TempRevList->childGroups();
	delete m_TempRevList;
	QString OldPath = m_Name + "/RevList/TempList.ini";
	int FileNum = m_RevConfig->value("Num", 0).toInt();
	FileNum++;
	QString NewPath = m_Name + "/RevList/" + QString::number(FileNum) + ".ini";
	QByteArray Path1 = OldPath.toLatin1(), Path2 = NewPath.toLatin1();
	rename(Path1.data(), Path2.data());
	m_RevConfig->setValue("Num", FileNum);
	m_RevConfig->setValue(QString::number(FileNum) + ".ini" + "/Rank", 0);
	m_RevConfig->setValue(QString::number(FileNum) + ".ini" + "/RemainTime", Rank[0]);
	WList Temp;
	Temp.ListName = QString::number(FileNum) + ".ini";
	Temp.Time = Rank[0];
	if (m_RemainTime == -1)
	{
		m_RemainTime = Temp.Time;
		m_Timer->start(m_RemainTime);
		m_ListCon.push_back(Temp);
		std::sort(m_ListCon.begin(), m_ListCon.end(), WListCmp);
	}
	else
	{
		int TimerRemain = m_Timer->remainingTime();
		m_Timer->stop();
		for (int i = 0; i < m_ListCon.size(); i++)
		{
			m_ListCon[i].Time = m_ListCon[i].Time - (m_RemainTime - TimerRemain);
		}
		m_ListCon.push_back(Temp);
		std::sort(m_ListCon.begin(), m_ListCon.end(), WListCmp);
		m_RemainTime = m_ListCon[m_ListCon.size() - 1].Time;
		m_Timer->start(m_RemainTime);
	}
	m_TempRevList = new QSettings(m_Name + "/RevList/TempList.ini", QSettings::IniFormat);
	m_TempRevList->clear();
}

void CWordListRev::m_RevTheListNow(QString Name)
{
	if (m_NeedReviewList!=NULL)
		delete m_NeedReviewList;
	QFile File(m_Name + "/RevList/RevList.ini");
	QFile OldFile(m_Name + "/RevList/" + Name);
	File.open(QIODevice::Append | QIODevice::WriteOnly);
	OldFile.open(QIODevice::ReadOnly);
	QTextStream ReadStream(&OldFile);
	QTextStream WriteStream(&File);
	while (!ReadStream.atEnd())
	{
		QString ReadAll = ReadStream.readAll();
		WriteStream << ReadAll;
	}
	File.close();
	OldFile.close();
	m_NeedReviewList = new QSettings(m_Name + "/RevList/RevList.ini", QSettings::IniFormat);
    m_WordList = m_NeedReviewList->childGroups();
    if(m_NowWord!=NULL)
    {
        for(int i=0;i<m_WordList.size();i++)
        {
            if(m_WordList[i]==m_NowWord->GetWord())
            {
                m_NowIndex=i+1;
                break;
            }
        }
    }
    int i=0;
}

void CWordListRev::m_GetNowRemainTime(int PrevRank, int PrevRemain, int PassTime, int &RemainTime, int &NowRank)
{
	qint64 Sum = 0;
	qint64 Time = 0;
	int index = 0;
	for (; index <= PrevRank; index++)
	{
		Time += Rank[index];
		Sum += Rank[index];
	}
	Time -= PrevRemain;
	Time += PassTime;
	for (; index<RankSize; index++)
	{
		Sum += Rank[index];
		if (Sum > Time)
			break;
	}
	RemainTime = Sum - Time;
	NowRank = index;
}

CWord* CWordListRev::StartRev()
{
	m_WordList = m_NeedReviewList->childGroups();
    if (m_NowWord != NULL)
        delete m_NowWord;
    m_NowWord=NULL;
	if (m_WordList.size() == 0)
		return NULL;
	m_NowIndex = 1;
	m_NowWord = m_MakeWord(m_NowIndex);
	return m_NowWord;
}

CWord* CWordListRev::GetNextWord()
{
	m_NowIndex %= m_WordList.size();
	m_NowIndex++;
	if (m_WordList.size() == 0)
	{
		if (m_NowWord != NULL)
			delete m_NowWord;
		m_NowWord = NULL;
	}
	else
	{
		m_NowWord = m_MakeWord(m_NowIndex);
	}
	return m_NowWord;
}

CWord* CWordListRev::GetPrevWord()
{
	if (m_NowIndex > 1)
		m_NowIndex--;
	if (m_NowWord != NULL)
		delete m_NowWord;
	m_NowWord = m_MakeWord(m_NowIndex);
	return m_NowWord;
}

CWord* CWordListRev::SetNowWordRem()
{
	m_NeedReviewList->remove(m_WordList[m_NowIndex - 1]);
	m_WordList = m_NeedReviewList->childGroups();
	if (m_WordList.size() == 0)
	{
		if (m_NowWord != NULL)
			delete m_NowWord;
        m_NowWord=NULL;
		return NULL;
	}
	if (m_NowIndex > m_WordList.size())
	{
		m_NowIndex = m_WordList.size();
	}
	if (m_NowWord != NULL)
		delete m_NowWord;
	m_NowWord = m_MakeWord(m_NowIndex);
	return m_NowWord;
}

bool CWordListRev::HaveNext()
{
	return m_NowIndex < m_WordList.size();
}

bool CWordListRev::HavePrev()
{
	if (m_WordList.size() == 0)
		return false;
	return m_NowIndex>1;
}

int CWordListRev::GetRemainNum()
{
	m_WordList = m_NeedReviewList->childGroups();
	return m_WordList.size();
}

CWord *CWordListRev::m_MakeWord(int Index)
{
	QString StrKey = m_WordList[Index - 1];
	QString TheWord = m_NeedReviewList->value(StrKey + "/Word").toString();
	QString Read = m_NeedReviewList->value(StrKey + "/Read").toString();
	QVector<QString> Mean;
	int Size = m_NeedReviewList->beginReadArray(StrKey + "/Mean");
	for (int i = 0; i < Size; i++)
	{
		m_NeedReviewList->setArrayIndex(i);
		Mean.push_back(m_NeedReviewList->value(QString::number(i)).toString());
	}
	m_NeedReviewList->endArray();
	return new CWord(StrKey, Read, Mean);
}

bool WListCmp(CWordListRev::WList a, CWordListRev::WList b)
{
	return a.Time > b.Time;
}
