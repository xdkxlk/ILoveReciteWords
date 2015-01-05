#include "mainwindow.h"
#include <QApplication>
#include <QTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QTime t;
	t = QTime::currentTime();
	qsrand(t.msec() + t.second() * 1000);
    MainWindow w;
    if(w.Logoin())
        w.show();
    else
        return 0;
    return a.exec();
}
