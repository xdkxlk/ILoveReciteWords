#include "cpeople.h"
#include <QDir>

QSettings * CPeople::m_PeoInfo = new QSettings("People.ini",QSettings::IniFormat);

CPeople::CPeople(const QString &name, const QString &pass, bool &NeedSetPlan) : QObject()
{
    if(ThePassRight(name,pass))
    {
        m_Name = name;
        if(m_PeoInfo->value(name+"/dir").toString()=="")
        {
            m_NeedSetPlan= NeedSetPlan = true;
            m_StuList=NULL;
			m_RevList = NULL;
        }
        else
        {
            m_NeedSetPlan= NeedSetPlan = false;
            m_WordListUse = m_PeoInfo->value(name+"/WordListUse").toString();
            m_DayReciteNum = m_PeoInfo->value(name+"/DayReciteNum").toInt();
            m_ReciteInOrder = m_PeoInfo->value(m_Name + "/ReciteIsInOrder", true).toBool();
            m_StuList = new CWordStuList(name, m_WordListUse, m_DayReciteNum, m_ReciteInOrder);
            connect(m_StuList,SIGNAL(NeedFlashStu()),this,SLOT(FlashStuList()));
            m_NowNewList = m_DefaultNewListName = m_PeoInfo->value(name + "/DefaultNewListName", "MyNewWord").toString();
			QStringList TempNewListName = m_PeoInfo->value(name + "/NewWordsList", "MyNewWord").toStringList();
			for (int i = 0; i < TempNewListName.size(); i++)
			{
				CWordListNewWord *Temp = new CWordListNewWord(name, TempNewListName[i]);
				m_NewWordList.insert(TempNewListName[i], Temp);
			}
			m_RevList = new CWordListRev(name);
			connect(m_RevList, SIGNAL(HaveNewRev()), this, SLOT(HaveRevTask()));
        }
    }
}

CPeople::PlanModel CPeople::SetPlan(const QString &WordPac, int DayNum, bool ReciteInOrder)
{
    if(m_NeedSetPlan)
    {
        m_WordListUse = WordPac;
        m_DayReciteNum = DayNum;
        m_ReciteInOrder = ReciteInOrder;
        QDir Dir;
        Dir.mkdir(m_Name);
        m_PeoInfo->setValue(m_Name + "/dir", m_Name);
        m_PeoInfo->setValue(m_Name + "/WordListUse", m_WordListUse);
        m_PeoInfo->setValue(m_Name + "/DayReciteNum", m_DayReciteNum);
        m_PeoInfo->setValue(m_Name + "/ReciteIsInOrder", m_ReciteInOrder);
        if (m_StuList != NULL)
            delete m_StuList;
        m_StuList = new CWordStuList(m_Name, m_WordListUse, m_DayReciteNum,m_ReciteInOrder);
        connect(m_StuList,SIGNAL(NeedFlashStu()),this,SLOT(FlashStuList()));
        m_NowNewList = m_DefaultNewListName = m_PeoInfo->value(m_Name + "/DefaultNewListName", "MyNewWord").toString();
        QStringList TempNewListName = m_PeoInfo->value(m_Name + "/NewWordsList", "MyNewWord").toStringList();
        for (int i = 0; i < TempNewListName.size(); i++)
        {
            CWordListNewWord *Temp = new CWordListNewWord(m_Name, TempNewListName[i]);
            m_NewWordList.insert(TempNewListName[i], Temp);
        }
        if (m_RevList != NULL)
            delete m_RevList;
        m_RevList = new CWordListRev(m_Name);
        connect(m_RevList, SIGNAL(HaveNewRev()), this, SLOT(HaveRevTask()));
        if (m_RevList->HaveRev())
            emit NeedRev();
        m_NeedSetPlan=false;
        return Apply_Success;
    }
    else
    {
        PlanModel Model;
        if(m_DayReciteNum!=DayNum)
        {
            m_DayReciteNum = DayNum;
            if(m_StuList->SetDayReciteNum(m_DayReciteNum))
            {
                Model=Apply_Success;
                m_RevList->SetRevNum(m_DayReciteNum);
            }
            else
                Model=Apply_NextTime;
        }
        if(m_ReciteInOrder!=ReciteInOrder)
        {
            m_ReciteInOrder=ReciteInOrder;
            if(m_StuList->SetReciteInOrder(m_ReciteInOrder))
                Model=Apply_Success;
            else
                Model=Apply_NextTime;
        }
        if(m_WordListUse!=WordPac)
        {
            m_WordListUse = WordPac;
            CWordStuList::RemoveAllData(m_Name);
            disconnect(m_StuList,SIGNAL(NeedFlashStu()),this,SLOT(FlashStuList()));
            if (m_StuList != NULL)
                delete m_StuList;
            m_StuList = new CWordStuList(m_Name, m_WordListUse, m_DayReciteNum,m_ReciteInOrder);
            connect(m_StuList,SIGNAL(NeedFlashStu()),this,SLOT(FlashStuList()));
            Model=Apply_LostData;
        }
        return Model;
    }
}

CPeople::~CPeople()
{
	if (m_RevList != NULL)
		delete m_RevList;
	if (m_StuList != NULL)
		delete m_StuList;
	for (QMap<QString, CWordListNewWord*>::iterator index = m_NewWordList.begin(); index != m_NewWordList.end(); index++)
	{
		delete index.value();
	}
	m_NewWordList.clear();
}

void CPeople::Save()
{
	if (m_StuList != NULL && m_RevList != NULL)
	{
		m_PeoInfo->setValue(m_Name + "/ReciteIsInOrder", m_ReciteInOrder);
		m_PeoInfo->setValue(m_Name + "/DayReciteNum", m_DayReciteNum);
		m_PeoInfo->setValue(m_Name + "/WordListUse", m_WordListUse);
		m_StuList->SaveData();
		m_PeoInfo->setValue(m_Name + "/DefaultNewListName", m_DefaultNewListName);
		QStringList NewWordListName(m_NewWordList.keys());
		m_PeoInfo->setValue(m_Name + "/NewWordsList", NewWordListName);
		m_RevList->Save();
	}
}

void CPeople::FlashStuList()
{
    m_PeoInfo->setValue(m_Name + "/ReciteIsInOrder", m_ReciteInOrder);
    m_PeoInfo->setValue(m_Name + "/DayReciteNum", m_DayReciteNum);
    m_PeoInfo->setValue(m_Name + "/WordListUse", m_WordListUse);
    if(m_StuList!=NULL)
        m_StuList->SaveData();
    delete m_StuList;
    m_StuList = new CWordStuList(m_Name, m_WordListUse, m_DayReciteNum,m_ReciteInOrder);
}

bool CPeople::TheNameExist(const QString & name)
{
    return m_PeoInfo->contains(name);
}

bool CPeople::ThePassRight(const QString & name, const QString & pass)
{
    if(m_PeoInfo->contains(name))
    {
        if(pass == m_PeoInfo->value(name,"").toString())
        {
            return true;
        }
    }
    return false;
}

bool CPeople::RegistPeo(const QString & name, const QString & pass)
{
    if(!m_PeoInfo->contains(name))
    {
        m_PeoInfo->setValue(name,pass);
        m_PeoInfo->setValue(name+"/dir","");
        return true;
    }
    return false;
}

double CPeople::GetTotalProgress()
{
    return m_StuList->GetTotalProgess();
}

double CPeople::GetDayProgress()
{
    return m_StuList->GetDayProgress();
}

int CPeople::GetDayStuWord()
{
	return m_StuList->GetDayWord();
}
int CPeople::GetDayStuRemain()
{
	return m_StuList->GetDayRemain();
}
int CPeople::GetStuTotalRemainNum()
{
	return m_StuList->GetTotalRemainNum();
}
int CPeople::GetStuTotalWord()
{
	return m_StuList->GetTotalWord();
}

CWord* CPeople::StartStudy()
{
	bool Finished = m_StuList->DayTaskFinished();
	CWord* Word = m_StuList->GetNowWord();
	if (!Finished)
		m_RevList->AddWords(Word);
	return Word;
}

CWord* CPeople::GetStuNextWord()
{
	bool Finished = m_StuList->DayTaskFinished();
	CWord* Word = m_StuList->GetNextWord();
	if (!Finished)
		m_RevList->AddWords(Word);
	return Word;
}

CWord* CPeople::GetStuPrevWord()
{
	bool Finished = m_StuList->DayTaskFinished();
	CWord* Word = m_StuList->GetPrevWord();
	if (!Finished)
		m_RevList->AddWords(Word);
	return Word;
}

bool CPeople::StuHavePrev()
{
    return m_StuList->HavePrev();
}

bool CPeople::StuHaveNext()
{
    return m_StuList->HaveNext();
}

void CPeople::SetReciteInOrder(bool Order)
{
    m_StuList->SetReciteInOrder(Order);
}

bool CPeople::ReciteIsInOrder()
{
    return m_StuList->GetReciteInOrder();
}

bool CPeople::DeleteNewWordList(QString Name)
{
	if (Name == m_DefaultNewListName)
		return false;
	else
	{
		QMap<QString, CWordListNewWord*>::iterator Index = m_NewWordList.find(Name);
		if (Index == m_NewWordList.end())
			return false;
		delete m_NewWordList[Name];
		CWordListNewWord::RemoveList(m_Name, Name);
		m_NewWordList.erase(Index);
		QStringList Temp = m_NewWordList.keys();
		m_PeoInfo->setValue(m_Name + "/NewWordsList", Temp);
	}
	return true;
}

bool CPeople::AddNewWordList(QString Name)
{
	if (m_NewWordList.find(Name) != m_NewWordList.end())
		return false;
	m_NewWordList.insert(Name, new CWordListNewWord(m_Name, Name));
	QStringList Temp = m_NewWordList.keys();
	m_PeoInfo->setValue(m_Name + "/NewWordsList", Temp);
	return true;
}

bool CPeople::SetDefNewWordList(QString Name)
{
	if (m_NewWordList.find(Name) == m_NewWordList.end())
		return false;
	m_DefaultNewListName = Name;
	return true;
}

int CPeople::GetDefaultListID()
{
	QMap<QString, CWordListNewWord*>::iterator Index = m_NewWordList.find(m_DefaultNewListName);
	int ID = 0;
	for (QMap<QString, CWordListNewWord*>::iterator temp = m_NewWordList.begin(); temp != Index; temp++, ID++)
	{ }
	return ID;
}

QStringList CPeople::GetNewWordListName()
{
	return m_NewWordList.keys();
}

CWord* CPeople::OpenNewWordList(QString Name)
{
	m_NowNewList = Name;
	return m_NewWordList[Name]->StartLook();
}

CWord* CPeople::GetNLNextWord()
{
	return m_NewWordList[m_NowNewList]->GetNextWord();
}

CWord* CPeople::GetNLPrevWord()
{
	return m_NewWordList[m_NowNewList]->GetPrevWord();
}

bool CPeople::NLHaveNext()
{
	return m_NewWordList[m_NowNewList]->HaveNext();
}

bool CPeople::NLHavePrev()
{
	return m_NewWordList[m_NowNewList]->HavePrev();
}

bool CPeople::DefNewListExist(CWord* Word)
{
	return m_NewWordList[m_DefaultNewListName]->TheWordExist(Word);
}

void CPeople::AddToDefNewList(CWord* Word)
{
	m_NewWordList[m_DefaultNewListName]->AddNewWord(Word);
}

void CPeople::RemFromDefNewList(CWord* Word)
{
	m_NewWordList[m_DefaultNewListName]->EraseWord(Word);
}

void CPeople::AddToNowNewList(CWord* Word)
{
	m_NewWordList[m_NowNewList]->AddNewWord(Word);
}

CWord* CPeople::RemFromNowNewList(CWord* Word)
{
	return m_NewWordList[m_NowNewList]->EraseWord(Word);
}

void CPeople::HaveRevTask()
{
	emit NeedRev();
}

bool CPeople::TestRev()
{
	return m_RevList->HaveRev();
}

CWord *CPeople::StartReview()
{
	return m_RevList->StartRev();
}
CWord* CPeople::GetRevNextWord()
{
	return m_RevList->GetNextWord();
}
CWord* CPeople::GetRevPrevWord()
{
	return m_RevList->GetPrevWord();
}
CWord* CPeople::SetRevNowWordRem()
{
	return m_RevList->SetNowWordRem();
}
bool CPeople::RevHaveNext()
{
	return m_RevList->HaveNext();
}
bool CPeople::RevHavePrev()
{
	return m_RevList->HavePrev();
}
int CPeople::GetRevRemainNum()
{
	return m_RevList->GetRemainNum();
}
