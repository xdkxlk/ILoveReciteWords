#include <QCoreApplication>
#include <QTextStream>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile file("CET6_Core.txt");
    if(file.open(QIODevice::ReadOnly))
        qDebug()<<"Opened";
    QSettings out("CET_Core.ini", QSettings::IniFormat);
    QTextStream stream(&file);
    int index=0;
    while(!stream.atEnd())
    {
        QString line = stream.readLine();
        QTextStream StreamLine(&line);
        QString temp;
        StreamLine>>temp;
        out.setValue(QString::number(index)+"/word", temp.trimmed());
        StreamLine>>temp;
        out.setValue(QString::number(index)+"/read", temp.trimmed());
        out.beginWriteArray(QString::number(index)+"/mean");
        for(int i=0; !StreamLine.atEnd() ;i++)
        {
            StreamLine>>temp;
            temp = temp.trimmed();
            if(temp=="")
                break;
            out.setArrayIndex(i);
            out.setValue(QString::number(i),temp);
        }
        out.endArray();
        index++;
    }
    out.setValue("num", index);
    return a.exec();
}
