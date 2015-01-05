#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qpushbutton.h>
#include "cpeople.h"
#include "nmlistwidget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    bool Logoin();
    ~MainWindow();

private slots:
    void on_PcomboBoxWordPac_currentIndexChanged(const QString &arg1);

    void on_PspinBoxRecNum_valueChanged(int arg1);

    void on_PButtonDone_clicked();

    void on_MButtonStartStu_clicked();

    void on_SButtonNext_clicked();

    void on_SButtonPrev_clicked();

    void on_SButtonBack_clicked();

    void on_MButtonNewWord_clicked();

    void on_NMButtonBack_clicked();

    void on_NMButtonOpen_clicked();

	void on_NMActionDelete_clicked();

	void on_NMActionAddNew_clicked();

	void on_NMSetDefault_clicked();

    void on_NButtonBack_clicked();

    void on_SButtonAMList_clicked();

    void on_SButtonShowMean_clicked();

    void on_SButtonHMean_clicked();

    void on_SButtonRead_clicked();

    void on_NButtonARNew_clicked();

    void on_NButtonPrev_clicked();

    void on_NButtonNext_clicked();

    void on_NButtonRead_clicked();

	void on_HaveRevTask();

    void on_RButtonSMean_clicked();

    void on_RButtonHMean_clicked();

    void on_MButtonReview_clicked();

    void on_RButtonWordDone_clicked();

    void on_RButtonAMToList_clicked();

    void on_RButtonRem_clicked();

    void on_RButtonFoget_clicked();

    void on_RButtonBack_clicked();

    void on_RButtonRead_clicked();

    void on_RlineEdit_textChanged(const QString &arg1);

    void on_actionLogout_triggered();

    void on_actionChangePlan_triggered();

    void on_actionQuite_triggered();


protected:
    void closeEvent(QCloseEvent * e);
private:
    Ui::MainWindow *ui;
    CPeople *m_People;
    QString m_Name;
    QString m_Password;

    void m_PFlashPlanLabel();
    void UpDateMainWindows();

	CWord *m_StuNowWord;
    void UpDateStuWin(CWord *Word);

	NMListWidget *m_NMListWidget;
	QPushButton *m_NMOpen;
	QPushButton *m_NMBack;
	QPushButton *m_NMAdd;
	QPushButton *m_NMDelete;
	QPushButton *m_NMSetDefault;
	void UpDateNMWin();

	CWord *m_NLNowWord;
	void UpDateNewWordWin(CWord* Word);

	CWord *m_RevNowWord;
	void UpDateRevWin(CWord *Word);
};

#endif // MAINWINDOW_H
