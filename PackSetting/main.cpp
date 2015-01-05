#include <QCoreApplication>
#include <QSettings>
#include <QDebug>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings *out = new QSettings("WordsPath.ini",QSettings::IniFormat);
    QFile FileName("Name.txt");
    if(!FileName.open(QIODevice::ReadOnly))
        qDebug()<<"Error in read";
    QTextStream stream(&FileName);
    QString name,path;
    while(!stream.atEnd())
    {
        stream>>name>>path;
        out->setValue(name+"/Path",QString("WordPack/"+path));
    }

    QStringList List = out->childGroups();
    for(int i=0;i<List.size();i++)
    {
        qDebug()<<List.at(i);
        QString Path= out->value(List.at(i)+"/Path").toString();
        qDebug()<<Path;
        QSettings *test = new QSettings(Path ,QSettings::IniFormat);
        qDebug()<<test->value("num",-1).toInt();
    }
    delete out;
    qDebug()<<"End";
    return a.exec();
}
