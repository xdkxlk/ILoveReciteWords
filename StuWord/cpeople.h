#ifndef CPEOPLE_H
#define CPEOPLE_H
#include <QMap>
#include <QString>
#include <QSettings>
#include <qvector.h>
#include <qobject.h>
#include "cwordstulist.h"
#include "cwordlistnewword.h"
#include "cwordlistrev.h"

class CPeople : public QObject
{
	Q_OBJECT
public:
    CPeople(const QString &name, const QString &pass, bool &NeedSetPlan);
	~CPeople();
    static bool TheNameExist(const QString & name);
    static bool ThePassRight(const QString & name, const QString & pass);
    static bool RegistPeo(const QString & name, const QString & pass);

    enum PlanModel{Apply_Success, Apply_LostData, Apply_NextTime};
    bool WordPackChange(QString Pack)
    {
        return Pack!=m_WordListUse;
    }
    bool DayNumChange(int num)
    {
        return m_DayReciteNum!=num;
    }
    bool ReciteOrderChange(bool Order)
    {
        return m_ReciteInOrder!=Order;
    }
    bool NeedSetPlan()
    {
        return m_NeedSetPlan;
    }
    PlanModel SetPlan(const QString &WordPac, int DayNum, bool ReciteInOrder);
    QString GetPackUseName()
    {
        return m_WordListUse;
    }

    int GetDayReciteNum()
    {
        return m_DayReciteNum;
    }

    void Save();

    double GetTotalProgress();
    double GetDayProgress();
	int GetDayStuWord();
	int GetDayStuRemain();
	int GetStuTotalRemainNum();
	int GetStuTotalWord();
    double GetReviewProgress();
	CWord* StartStudy();
    CWord* GetStuNextWord();
    CWord* GetStuPrevWord();
    bool StuHavePrev();
    bool StuHaveNext();
    void SetReciteInOrder(bool);
    bool ReciteIsInOrder();

	bool DeleteNewWordList(QString Name);
	bool AddNewWordList(QString Name);
	bool SetDefNewWordList(QString Name);
	int GetDefaultListID();
	QStringList GetNewWordListName();
	CWord* OpenNewWordList(QString Name);
	CWord* GetNLNextWord();
	CWord* GetNLPrevWord();
	bool NLHaveNext();
	bool NLHavePrev();
	bool DefNewListExist(CWord*);
	bool NowNewListExist(CWord*);
	void AddToDefNewList(CWord*);
	void RemFromDefNewList(CWord*);
	void AddToNowNewList(CWord*);
	CWord* RemFromNowNewList(CWord*);

	bool TestRev();
	CWord *StartReview();
	CWord* GetRevNextWord();
	CWord* GetRevPrevWord();
	CWord* SetRevNowWordRem();
	bool RevHaveNext();
	bool RevHavePrev();
	int GetRevRemainNum();
signals:
	void NeedRev();
private slots:
	void HaveRevTask();
private:
    static QSettings * m_PeoInfo;

    QString m_Name;
    bool m_NeedSetPlan;
    QString m_WordListUse;
    bool m_ReciteInOrder;
    int m_DayReciteNum;
    CWordStuList * m_StuList;
	QString m_DefaultNewListName;
	QString m_NowNewList;
	QMap<QString, CWordListNewWord*> m_NewWordList;

	CWordListRev *m_RevList;
};

#endif // CPEOPLE_H
