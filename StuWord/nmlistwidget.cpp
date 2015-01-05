#include "nmlistwidget.h"
#include <QMenu>
#include <QMouseEvent>

NMListWidget::NMListWidget(QWidget *parent) :
    QListWidget(parent)
{
	m_Delete = new QAction(QString::fromUtf8("\xe5\x88\xa0\xe9\x99\xa4"), this); //É¾³ý
	m_Login = new QAction(QString::fromUtf8("\xe8\xbd\xbd\xe5\x85\xa5"), this);  //ÔØÈë
    connect(m_Delete,SIGNAL(triggered()),this,SLOT(Delete_clicked()));
    connect(m_Login,SIGNAL(triggered()),this,SLOT(Login_clicked()));
}

void NMListWidget::contextMenuEvent(QContextMenuEvent* e)
{
	if (itemAt(mapFromGlobal(cursor().pos())) != NULL)
    {
        QMenu *menu = new QMenu(this);
        menu->addAction(m_Delete);
        menu->addAction(m_Login);
        menu->move(cursor().pos());
        menu->show();
    }
}

void NMListWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		if (itemAt(mapFromGlobal(cursor().pos())) != NULL)
		{
			emit doubleClicked(currentIndex());
		}
	}
}

void NMListWidget::Delete_clicked()
{
    emit Delete();
}

void NMListWidget::Login_clicked()
{
    emit Login();
}
