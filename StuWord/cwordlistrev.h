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
	QSettings *m_RevConfig; //��ϰ�ĵ��ʱ��Ļ�����Ϣ���ϴ����е�ʱ�䣬���е��ʱ�ֿ�����֣��Ѿ���ϰ���ĵȼ����´�ʣ��ĸ�ϰʱ�䣩
	QTimer *m_Timer; //�������һ�θ�ϰ���е���ʱ
	int m_RemainTime; //�����һ�εĸ�ϰ��ʱ��
	std::vector<WList> m_ListCon; //װ���зֿ��list�����֣�ʣ��ʱ��
	int m_MaxTempSize; //һ��list������С����������һ�������ĵ�������ͬ��
	QSettings *m_TempRevList; //��δ����һ��list�Ĵ�С�ĵ��ʵ���ʱ����

	int m_NowIndex; //���ڸ�ϰ���ĵ��ʵ����
	QStringList m_WordList; //m_NeedReviewList��keys��Ҳ�ǵ���
	QSettings *m_NeedReviewList; //�Ѿ���ʱ��ĸ�ϰlist�����е���
	CWord *m_NowWord;
	CWord *m_MakeWord(int Index);

	void m_FinishTempFile();
	void m_RevTheListNow(QString);
	void m_GetNowRemainTime(int PrevRank, int PrevRemain, int PassTime, int &RemainTime, int &NowRank); //�õ����ʼ��ʼ��time�󣬵���һ�θ�ϰ��ʱ��RemainTime�����ڵĸ�ϰrank NowRank
};
bool WListCmp(CWordListRev::WList a, CWordListRev::WList b);
#endif // CWORDLISTREV_H
