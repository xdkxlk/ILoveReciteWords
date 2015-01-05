#ifndef CWORD_H
#define CWORD_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVector>

class CWord : public QObject
{
    Q_OBJECT
public:
    explicit CWord(QSettings *WordPack, int ID, QObject *parent = 0);
	explicit CWord(QString Word, QString Read, QVector<QString> Mean, QObject *parent = 0);
    int GetID();
    QString GetWord();
    QString GetRead();
    QVector<QString> GetMeans();
private:
    QString m_Word, m_Read;
    QVector<QString> m_Mean;
    int m_ID;
};

#endif // CWORD_H
