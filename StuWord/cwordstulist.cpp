#include "cwordstulist.h"
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QDateTime>
#include <QDate>
#include <qalgorithms.h>

typedef struct{
	bool HaveRecite;
	int ID;
}StudyData;

CWordStuList::CWordStuList(const QString &PeoName, const QString &WordListUse, int DayReciteNum, bool IsInOrder, QObject *parent) :
    QObject(parent)
{
    m_IsInOrder = IsInOrder;
    m_ProName = PeoName;

    QDateTime NowDateTime = QDateTime::currentDateTime();
    QDateTime EndDayDateTime = NowDateTime.addDays(1);
    EndDayDateTime.setTime(QTime(0,0,0));
    m_Time.start(NowDateTime.msecsTo(EndDayDateTime));

    m_WordListUse = new QSettings(CWordList::GetPath(WordListUse), QSettings::IniFormat);
	QFile File(PeoName + "/StuList.dat"); //还未学习的单词ID
	QFile StudyingList(PeoName + "/StudyingList.dat"); //正在学习的单词
	// 为结构体  StudyData
    QList<StudyData> StuyingData;
    m_StuListConfig = new QSettings(PeoName + "/StuListConfig.ini", QSettings::IniFormat);
    QDate LastTime = m_StuListConfig->value("LastTimeRecite", QDate::currentDate()).toDate();
    QDate NowTime = QDate::currentDate();
    bool OnTheSameDay = (LastTime==NowTime);
	if (File.exists())
	{
		File.open(QIODevice::ReadOnly);
		QDataStream Stream(&File);
		Stream >> m_WordListData;
		File.close();
		StudyingList.open(QIODevice::ReadOnly);
        QTextStream Read(&StudyingList);
        while(!Read.atEnd())
        {
            StudyData temp;
            int ReciteFlag;
            Read>>ReciteFlag>>temp.ID;
            temp.HaveRecite=(bool)ReciteFlag;
            if(OnTheSameDay)
            {
                StuyingData.push_back(temp);
            }
            else
            {
                if(!temp.HaveRecite)
                    StuyingData.push_back(temp);
            }
        }
		StudyingList.close();
	}
	else
	{
		int PackNum = m_WordListUse->value("num").toInt();
        if(IsInOrder)
        {
            for(int i=0;i<PackNum;i++)
            {
                m_WordListData.push_back(i);
            }
        }
        else
        {
            while (m_WordListData.size() != PackNum)
            {
                int temp = qrand() % PackNum;
                if (qFind(m_WordListData.begin(), m_WordListData.end(), temp) == m_WordListData.end())
                    m_WordListData.push_back(temp);
            }
        }
	}
	m_DayReciteNum = DayReciteNum;

	int Days = LastTime.daysTo(NowTime);
	if (Days == 0)
	{
		m_DayRemainNum = m_StuListConfig->value("DayRemainNum", m_DayReciteNum).toInt();
		m_DayHaveReciteNum = m_StuListConfig->value("DayHaveReciteNum", 0).toInt();
		m_NowIndex = m_StuListConfig->value("NowIndex", 1).toInt();
	}
	else
	{
		m_DayHaveReciteNum = 0;
        m_NowIndex = 1;
		m_DayRemainNum = m_StuListConfig->value("DayRemainNum", 0).toInt();
		m_DayRemainNum = Days*m_DayReciteNum + m_DayRemainNum;
	}
    for (int i = 0; i < StuyingData.size(); i++)
	{
        int ID = StuyingData[i].ID;
		Word temp;
        temp.HaveRecite = StuyingData[i].HaveRecite;
		temp.Word = new CWord(m_WordListUse, ID);
		m_Words.push_back(temp);
	}
    for (int i = 0; i < m_WordListData.size() && i < m_DayRemainNum - StuyingData.size(); i++)
	{
		int ID = m_WordListData[0];
		m_WordListData.pop_front();
		Word temp;
		temp.HaveRecite = false;
		temp.Word = new CWord(m_WordListUse, ID);
		m_Words.push_back(temp);
	}
}

CWordStuList::~CWordStuList()
{
	for (int i = 0; i < m_Words.size(); i++)
		delete m_Words[i].Word;
}

void CWordStuList::SaveData()
{
	QFile File(m_ProName + "/StuList.dat");
	File.open(QIODevice::WriteOnly);
	QDataStream Stream(&File);
	Stream << m_WordListData;
	File.close();

	QFile StudyingList(m_ProName + "/StudyingList.dat");
	StudyingList.open(QIODevice::WriteOnly);
    QTextStream StuStream(&StudyingList);
	for (int i = 0; i < m_Words.size(); i++)
    {
        if(i==0)
        {
            StuStream << m_Words[i].HaveRecite<<" " ;
            StuStream << m_Words[i].Word->GetID();
        }
        else
        {
            StuStream <<" "<< m_Words[i].HaveRecite;
            StuStream <<" "<< m_Words[i].Word->GetID();
        }
	}
	StudyingList.close();

	m_StuListConfig->setValue("LastTimeRecite", QDate::currentDate());
	m_StuListConfig->setValue("DayRemainNum", m_DayRemainNum);
	m_StuListConfig->setValue("DayHaveReciteNum", m_DayHaveReciteNum);
	m_StuListConfig->setValue("NowIndex", m_NowIndex);
}

void CWordStuList::RemoveAllData(QString PeoName)
{
    QString Path[3] = {PeoName + "/StuList.dat", PeoName + "/StudyingList.dat", PeoName + "/StuListConfig.ini"};
    for(int i=0;i<3;i++)
    {
        QByteArray Arry = Path[i].toLatin1();
        remove(Arry.data());
    }
}

bool CWordStuList::SetReciteInOrder(bool InOrder)
{
    if(m_IsInOrder==InOrder)
        return true;
    m_IsInOrder=InOrder;
    if(m_DayHaveReciteNum==0)
    {
        for(int i=0;i<m_Words.size();i++)
        {
            m_WordListData.push_back(m_Words[i].Word->GetID());
            delete m_Words[i].Word;
        }
		m_Words.clear();
    }
    if(InOrder)
        qSort(m_WordListData.begin(), m_WordListData.end());
    else
    {
        QList<int> Temp=m_WordListData;
        QList<int> TempIndex;
        int Size = Temp.size();
        m_WordListData.clear();
        while (m_WordListData.size() != Size)
        {
            int index = qrand() % Size;
            if (qFind(TempIndex.begin(), TempIndex.end(), index) == TempIndex.end())
            {
                TempIndex.push_back(index);
                m_WordListData.push_back(Temp[index]);
            }
        }
    }
    if(m_DayHaveReciteNum==0)
    {
        for (int i = 0; i < m_WordListData.size() && i < m_DayRemainNum; i++)
        {
            int ID = m_WordListData[0];
            m_WordListData.pop_front();
            Word temp;
            temp.HaveRecite = false;
            temp.Word = new CWord(m_WordListUse, ID);
            m_Words.push_back(temp);
        }
        return true;
    }
    return false;
}

bool CWordStuList::GetReciteInOrder()
{
    return m_IsInOrder;
}

CWord* CWordStuList::GetNowWord()
{
	if (m_NowIndex == 0)
		return NULL;
	if (!m_Words[m_NowIndex - 1].HaveRecite)
	{
		m_Words[m_NowIndex - 1].HaveRecite = true;
		m_DayRemainNum--;
		m_DayHaveReciteNum++;
	}
	return m_Words[m_NowIndex - 1].Word;
}

CWord* CWordStuList::GetNextWord()
{
	if (m_NowIndex <= m_Words.size())
	{
		m_NowIndex++;
		if (!m_Words[m_NowIndex - 1].HaveRecite)
		{
			m_Words[m_NowIndex - 1].HaveRecite = true;
			m_DayRemainNum--;
			m_DayHaveReciteNum++;
		}
		return m_Words[m_NowIndex - 1].Word;
	}
	else
		return NULL;
}

CWord* CWordStuList::GetPrevWord()
{
	if (m_NowIndex > 1)
	{
		m_NowIndex--;
		return m_Words[m_NowIndex - 1].Word;
	}
	else
		return NULL;
}

bool CWordStuList::DayTaskFinished()
{
	return m_DayRemainNum == 0;
}

bool CWordStuList::HaveNext()
{
	if (m_DayReciteNum <= 0)
		emit NeedSetDayReciteNum();
	return m_NowIndex<m_Words.size();
}

bool CWordStuList::HavePrev()
{
	if (m_DayReciteNum <= 0)
		emit NeedSetDayReciteNum();
	return m_NowIndex>1;
}

double CWordStuList::GetTotalProgess()
{
	int TotalNum = m_WordListUse->value("num").toInt();
	return 1 - (1.0 * m_WordListData.size() / TotalNum);
}

double CWordStuList::GetDayProgress()
{
    return 1.0*m_DayHaveReciteNum / (m_DayHaveReciteNum+m_DayRemainNum);
}

void CWordStuList::AddNewWord(int WordID)
{
	m_WordListData.push_back(WordID);
	Word Temp;
	Temp.HaveRecite = false;
	Temp.Word = new CWord(m_WordListUse, WordID);
	m_Words.push_back(Temp);
}

bool CWordStuList::SetDayReciteNum(int Num)
{
    if(m_DayHaveReciteNum==0)
    {
        m_DayRemainNum = m_DayRemainNum+(Num-m_DayReciteNum);
        m_DayReciteNum = Num;
        for(int i=0;i<m_Words.size();i++)
        {
            m_WordListData.push_back(m_Words[i].Word->GetID());
            delete m_Words[i].Word;
        }
		m_Words.clear();
        for (int i = 0; i < m_WordListData.size() && i < m_DayRemainNum; i++)
        {
            int ID = m_WordListData[0];
            m_WordListData.pop_front();
            Word temp;
            temp.HaveRecite = false;
            temp.Word = new CWord(m_WordListUse, ID);
            m_Words.push_back(temp);
        }
        return true;
    }
    return false;
}

int CWordStuList::GetDayWord()
{
	return m_DayRemainNum + m_DayHaveReciteNum;
}
int CWordStuList::GetDayRemain()
{
	return m_DayRemainNum;
}
int CWordStuList::GetTotalRemainNum()
{
	return m_WordListData.size() + m_DayRemainNum;
}
int CWordStuList::GetTotalWord()
{
	return m_WordListUse->value("num").toInt();
}
