#ifndef NMLISTWIDGET_H
#define NMLISTWIDGET_H

#include <QListWidget>
#include <QAction>

class NMListWidget : public QListWidget
{
    Q_OBJECT
public:
	explicit NMListWidget(QWidget *parent = 0);

signals:
    void Delete();
    void Login();
public slots:
    void Delete_clicked();
    void Login_clicked();
protected:
    void contextMenuEvent(QContextMenuEvent* e);
    void mouseDoubleClickEvent(QMouseEvent *event);
private:
    QAction *m_Delete;
    QAction *m_Login;
};

#endif // NMLISTWIDGET_H
