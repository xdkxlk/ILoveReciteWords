#ifndef CWORDLISTREV_H
#define CWORDLISTREV_H

#include <QObject>
#include <vector>
#include <qmap.h>
#include <qvector.h>
#include <qtimer.h>
#include <qsettings.h>
#include "cword.h"

class CWordListRev : public QObject
{
	Q_OBJECT
	typedef struct{
		QString ListName;
		int Time;
	}WList;
	friend bool WListCmp(CWordListRev::WList a, CWordListRev::WList b);
public:
    explicit CWordListRev(QString PeoName, QObject *parent = 0);
	~CWordListRev();
	bool HaveRev();
	void AddWords(CWord*);
    void SetRevNum(int num)
    {
        m_MaxTempSize=num;
    }

	void Save();

	CWord* StartRev();
	CWord* GetNextWord();
	CWord* GetPrevWord();
	CWord* SetNowWordRem();
	bool HaveNext();
	bool HavePrev();
	int GetRemainNum();
signals:
	void HaveNewRev();
public slots:
	void The_Time_Out();
private:
	QString m_Name;
	QSettings *m_RevConfig; //复习的单词本的基本信息（上次运行的时间，所有单词表分块的名字，已经复习到的等级，下次剩余的复习时间）
	QTimer *m_Timer; //对最近的一次复习进行倒计时
	int m_RemainTime; //最近的一次的复习的时间
	std::vector<WList> m_ListCon; //装所有分块的list的名字，剩余时间
	int m_MaxTempSize; //一个list的最大大小（本程序与一天所背的单词量相同）
	QSettings *m_TempRevList; //还未到达一个list的大小的单词的临时保存

	int m_NowIndex; //现在复习到的单词的序号
	QStringList m_WordList; //m_NeedReviewList的keys，也是单词
	QSettings *m_NeedReviewList; //已经到时间的复习list，存有单词
	CWord *m_NowWord;
	CWord *m_MakeWord(int Index);

	void m_FinishTempFile();
	void m_RevTheListNow(QString);
	void m_GetNowRemainTime(int PrevRank, int PrevRemain, int PassTime, int &RemainTime, int &NowRank); //得到从最开始开始计time后，到下一次复习的时间RemainTime，现在的复习rank NowRank
};
bool WListCmp(CWordListRev::WList a, CWordListRev::WList b);
#endif // CWORDLISTREV_H
