#ifndef CWORDSTULIST_H
#define CWORDSTULIST_H
#include <QObject>
#include <QTimer>
#include <QSettings>
#include "cwordlist.h"
#include "cword.h"

class CWordStuList : public QObject
{
    Q_OBJECT
public:
    explicit CWordStuList(const QString &PeoName, const QString &WordListUse, int DayReciteNum, bool IsInOrder=true, QObject *parent = 0);
    ~CWordStuList();
    static void RemoveAllData(QString PeoName);
    CWord* GetNextWord();
    CWord* GetPrevWord();
    CWord* GetNowWord();
    bool HaveNext();
    bool HavePrev();
    void SaveData();
	bool DayTaskFinished();
    bool SetReciteInOrder(bool InOrder);
    bool GetReciteInOrder();
    double GetTotalProgess();
    double GetDayProgress();
	int GetDayWord();
	int GetDayRemain();
	int GetTotalRemainNum();
	int GetTotalWord();
signals:
    void NeedSetDayReciteNum();
public slots:
    void AddNewWord(int WordID);
    bool SetDayReciteNum(int Num);
private:
    typedef struct{
        bool HaveRecite;
        CWord* Word;
    }Word;
    bool m_IsInOrder;//是否顺序背诵
    QSettings * m_WordListUse;
    QString m_ProName;
    QList<int> m_WordListData; //还没有学的的单词
    QList<Word> m_Words;  //正在学习的单词
    QSettings * m_StuListConfig;
    int m_NowIndex;
    int m_DayHaveReciteNum;
    int m_DayReciteNum;
    int m_DayRemainNum;

    QTimer m_Time;//一天的倒计时
};

#endif // CWORDSTULIST_H
