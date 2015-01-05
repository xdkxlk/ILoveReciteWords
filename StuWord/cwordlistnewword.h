#ifndef CWORDLISTNEWWORD_H
#define CWORDLISTNEWWORD_H
#include "cwordlist.h"
#include <qsettings.h>
#include <qstringlist.h>
#include "cword.h"
#include <QList>

class CWordListNewWord
{
public:
    CWordListNewWord(const QString &PeoName, const QString &ListName);
    ~CWordListNewWord();
	static bool RemoveList(QString PeoName, QString ListName);
    CWord* GetNextWord();
    CWord* GetPrevWord();
	CWord* StartLook();
    bool HaveNext();
    bool HavePrev();
	bool TheWordExist(CWord *);

public slots:
    void AddNewWord(CWord *);
	CWord* EraseWord(CWord *);
private:
	QString m_PeoName;
	//QString m_ListName;
	QSettings * m_ListData;
	QStringList Key;
	CWord *m_NowWord;
    int m_NowIndex;

	CWord * m_GetWord(int Index);
};

#endif // CWORDLISTNEWWORD_H
