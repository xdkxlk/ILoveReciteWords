#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "cwordlist.h"
#include "cword.h"
#include "addnwlistdialog.h"
#include "cspeak.h"
#include <QDate>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qmessagebox.h>
#include <qtextstream.h>
#include <qdesktopwidget.h>

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	CSpeak::InitSpeak();
	ui->MButtonReview->setEnabled(false);
	ui->PcomboBoxWordPac->addItems(CWordList::GetPackName());
	int WordNum = CWordList::GetPackNum(ui->PcomboBoxWordPac->currentText());
	ui->PlabelWordAllNum->setText(QString::number(WordNum) + " Words");
	ui->PspinBoxRecNum->setRange(5, WordNum);
	ui->SgroupBox->setVisible(false);
	ui->NgroupBox->setVisible(false);

	ui->NMgroupBox->setVisible(false);
	m_NMListWidget = new NMListWidget(this);
	m_NMSetDefault = new QPushButton("\350\256\276\344\270\272\351\273\230\350\256\244", this);//设为默认
	m_NMOpen = new QPushButton("\346\211\223\345\274\200", this);//打开 
	m_NMBack = new QPushButton("\350\277\224\345\233\236", this);//返回 
	m_NMAdd = new QPushButton("\346\226\260\345\273\272\347\224\237\350\257\215\346\234\254", this);//新建生词本 
	m_NMDelete = new QPushButton("\345\210\240\351\231\244", this);//删除
	QHBoxLayout *NMLayout = new QHBoxLayout(this);
	NMLayout->addWidget(m_NMListWidget);
	QVBoxLayout *NMVLayout = new QVBoxLayout(this);
	NMVLayout->addWidget(m_NMSetDefault);
	NMVLayout->addStretch();
	NMVLayout->addWidget(m_NMAdd);
	NMVLayout->addWidget(m_NMDelete);
	NMVLayout->addWidget(m_NMOpen);
	NMVLayout->addWidget(m_NMBack);
	NMLayout->addLayout(NMVLayout);
	ui->NMgroupBox->setLayout(NMLayout);
    connect(m_NMListWidget, SIGNAL(Login()), this, SLOT(on_NMButtonOpen_clicked()));
	connect(m_NMListWidget, SIGNAL(Delete()), this, SLOT(on_NMActionDelete_clicked()));
	connect(m_NMSetDefault, SIGNAL(clicked()), this, SLOT(on_NMSetDefault_clicked()));
	connect(m_NMDelete, SIGNAL(clicked()), this, SLOT(on_NMActionDelete_clicked()));
	connect(m_NMAdd, SIGNAL(clicked()), this, SLOT(on_NMActionAddNew_clicked()));
	connect(m_NMListWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_NMButtonOpen_clicked()));
	connect(m_NMOpen, SIGNAL(clicked()), this, SLOT(on_NMButtonOpen_clicked()));
    connect(m_NMBack, SIGNAL(clicked()), this, SLOT(on_NMButtonBack_clicked()));

    QRegExp Reg("\\w+");
    QRegExpValidator *pRegExp1 = new QRegExpValidator(Reg, this);
    ui->RlineEdit->setValidator(pRegExp1);

	ui->RgroupBox->setVisible(false);

	m_PFlashPlanLabel();
	m_People = NULL;

	setFixedHeight(400);
	setFixedWidth(350);
	setWindowTitle("\350\256\241\345\210\222\350\256\276\347\275\256");//计划设置
	QDesktopWidget *pDesk = QApplication::desktop();
	move((pDesk->width() - width()) / 2, (pDesk->height() - height()) / 2);
}

MainWindow::~MainWindow()
{
	if (m_People == NULL)
		delete m_People;
	delete ui;
	CSpeak::ClearSpeak();
}

void MainWindow::closeEvent(QCloseEvent * e)
{
	if (m_People != NULL)
		m_People->Save();
}

bool MainWindow::Logoin()
{
	LoginDialog Dialog(m_Name, m_Password, this);
	if (Dialog.exec() == QDialog::Accepted)
	{
		bool NeedSetPlan = false;
		if (m_People != NULL)
			delete m_People;
		m_People = new CPeople(m_Name, m_Password, NeedSetPlan);
		connect(m_People, SIGNAL(NeedRev()), this, SLOT(on_HaveRevTask()));
		if (NeedSetPlan)
		{
			ui->PlangroupBox->setVisible(true);
			ui->MaingroupBox->setVisible(false);
			ui->menuBar->hide();
		}
		else
		{
			ui->PlangroupBox->setVisible(false);
			ui->MaingroupBox->setVisible(true);
			UpDateMainWindows();
		}
		return true;
	}
	return false;
}

void MainWindow::on_PcomboBoxWordPac_currentIndexChanged(const QString &arg1)
{
	int WordNum = CWordList::GetPackNum(arg1);
	ui->PlabelWordAllNum->setText(QString::number(WordNum));
	ui->PspinBoxRecNum->setMaximum(WordNum);
	m_PFlashPlanLabel();
}

void MainWindow::m_PFlashPlanLabel()
{
	int WordNum = CWordList::GetPackNum(ui->PcomboBoxWordPac->currentText());
	int WordDay = ui->PspinBoxRecNum->value();
	if (WordDay == 0)
	{
		ui->PlabelPreView->setText("");
		return;
	}
	QDate date = QDate::currentDate();
	int day = WordNum / WordDay - 1;
	date = date.addDays(day);
	if (WordNum%WordDay != 0)
	{
		date = date.addDays(1);
	}

	QString Plane = "\344\275\240\345\260\206\345\234\250 " + date.toString("yyyy-M-d") + " \345\256\214\346\210\220\350\256\241\345\210\222";//你将在  完成计划 
	if (WordNum%WordDay != 0)
	{
		Plane += "\n\346\234\200\345\220\216\344\270\200\345\244\251\344\275\240\345\260\206\350\203\214\350\257\265 " + QString::number(WordNum%WordDay) + " \344\270\252\345\215\225\350\257\215";// 最后一天你将背诵   个单词
	}
	ui->PlabelPreView->setText(Plane);
}

void MainWindow::on_PspinBoxRecNum_valueChanged(int arg1)
{
	m_PFlashPlanLabel();
}

void MainWindow::on_PButtonDone_clicked()
{
    QString Pack=ui->PcomboBoxWordPac->currentText();
    int Num=ui->PspinBoxRecNum->value();
    bool Order=ui->PcheckBoxInOrder->isChecked();
    if(m_People->NeedSetPlan())
    {
        m_People->SetPlan(Pack,Num,Order );
    }
    else
    {
        bool HaveSet=false;
        if(m_People->WordPackChange(Pack))
        {
            //注意   所选单词表发生变化，此将丢失所有数据，是否继续
            if(QMessageBox::question(this,"\346\263\250\346\204\217","\346\211\200\351\200\211\345\215\225\350\257\215\350\241\250\345\217\221\347\224\237\345\217\230\345\214\226\357\274\214\346\255\244\345\260\206\344\270\242\345\244\261\346\211\200\346\234\211\346\225\260\346\215\256\357\274\214\346\230\257\345\220\246\347\273\247\347\273\255"
                                                                       ,QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)==QMessageBox::Ok)
            {
                m_People->SetPlan(Pack,Num,Order );
                HaveSet=true;
            }
        }
        else if((m_People->DayNumChange(Num) || m_People->ReciteOrderChange(Order)) && !HaveSet)
        {
            //注意   计划发生改变，是否继续
            if(QMessageBox::question(this,"\346\263\250\346\204\217","\350\256\241\345\210\222\345\217\221\347\224\237\346\224\271\345\217\230\357\274\214\346\230\257\345\220\246\347\273\247\347\273\255"
                                     ,QMessageBox::Ok|QMessageBox::Cancel,QMessageBox::Ok)==QMessageBox::Ok)
            {
                CPeople::PlanModel Model = m_People->SetPlan(Pack,Num,Order );
                switch(Model)
                {
                case CPeople::Apply_Success:
                    //成功更改，并已经生效
                    QMessageBox::information(this,"\346\263\250\346\204\217","\346\210\220\345\212\237\346\233\264\346\224\271\357\274\214\345\271\266\345\267\262\347\273\217\347\224\237\346\225\210");
                    break;
                default:
                    //   计划将在明天生效
                    QMessageBox::information(this,"\346\263\250\346\204\217","\350\256\241\345\210\222\345\260\206\345\234\250\346\230\216\345\244\251\347\224\237\346\225\210");
                    break;
                }
            }
        }
    }
    ui->PlangroupBox->setVisible(false);
    ui->MaingroupBox->setVisible(true);
	ui->menuBar->show();
	UpDateMainWindows();
}

void MainWindow::UpDateMainWindows()
{
	double DayPro = m_People->GetDayProgress();
	double TotalPro = m_People->GetTotalProgress();
	ui->MaingroupBox->setTitle("Hello " + m_Name);
	ui->MprogressBarAll->setValue(TotalPro * 100);
	ui->MprogressBarDayWord->setValue(DayPro * 100);
	ui->MButtonReview->setEnabled(m_People->TestRev());
	QString LabelStr = QString::number(m_People->GetDayStuRemain()) + "/" + QString::number(m_People->GetDayStuWord());
	ui->MlabelDayStu->setText(LabelStr);
	LabelStr = QString::number(m_People->GetStuTotalRemainNum()) + "/" + QString::number(m_People->GetStuTotalWord());
	ui->MlabelAllWord->setText(LabelStr);
	LabelStr = QString::number(m_People->GetRevRemainNum());
	ui->MlabelRevNum->setText(LabelStr);
	setWindowTitle("\346\210\221\347\210\261\350\256\260\345\215\225\350\257\215");//我爱记单词
}

void MainWindow::on_MButtonStartStu_clicked()
{
	ui->MaingroupBox->setVisible(false);
	ui->SgroupBox->setVisible(true);
	m_StuNowWord = m_People->StartStudy();
	UpDateStuWin(m_StuNowWord);
}

void MainWindow::on_SButtonNext_clicked()
{
	m_StuNowWord = m_People->GetStuNextWord();
	UpDateStuWin(m_StuNowWord);
}

void MainWindow::on_SButtonPrev_clicked()
{
	m_StuNowWord = m_People->GetStuPrevWord();
	UpDateStuWin(m_StuNowWord);
}

void MainWindow::on_SButtonAMList_clicked()
{
	if (m_People->DefNewListExist(m_StuNowWord))
	{
		ui->SButtonAMList->setText("\346\267\273\345\212\240\345\210\260\347\224\237\350\257\215\346\234\254");//添加到生词本
		m_People->RemFromDefNewList(m_StuNowWord);
	}
	else
	{
		ui->SButtonAMList->setText("\344\273\216\347\224\237\350\257\215\346\234\254\347\247\273\351\231\244");//从生词本移除
		m_People->AddToDefNewList(m_StuNowWord);
	}
}

void MainWindow::on_SButtonBack_clicked()
{
	ui->SgroupBox->setVisible(false);
	ui->MaingroupBox->setVisible(true);
	UpDateMainWindows();
}

void MainWindow::on_SButtonShowMean_clicked()
{
	ui->SButtonShowMean->setVisible(false);
	ui->SButtonHMean->setVisible(true);
	ui->SlabelMean->setVisible(true);
}

void MainWindow::on_SButtonHMean_clicked()
{
	ui->SButtonShowMean->setVisible(true);
	ui->SButtonHMean->setVisible(false);
	ui->SlabelMean->setVisible(false);
}

void MainWindow::on_SButtonRead_clicked()
{
    CSpeak::Speak(m_StuNowWord->GetWord());
}

void MainWindow::UpDateStuWin(CWord *Word)
{
	ui->SlabelWord->setText(Word->GetWord());
	ui->SlabelRead->setText(Word->GetRead());
	QString Temp;
	QVector<QString> Means = Word->GetMeans();
	for (int i = 0; i < Means.size(); i++)
	{
		Temp += Means[i];
		Temp += "\n";
	}
	ui->SlabelMean->setText(Temp);
	if (m_People->DefNewListExist(Word))
	{
		ui->SButtonAMList->setText("\344\273\216\347\224\237\350\257\215\346\234\254\347\247\273\351\231\244");//从生词本移除
	}
	else
	{
		ui->SButtonAMList->setText("\346\267\273\345\212\240\345\210\260\347\224\237\350\257\215\346\234\254");//添加到生词本
	}
	ui->SButtonNext->setEnabled(m_People->StuHaveNext());
	ui->SButtonPrev->setEnabled(m_People->StuHavePrev());
	ui->SButtonShowMean->setVisible(true);
	ui->SButtonHMean->setVisible(false);
	ui->SlabelMean->setVisible(false);
    setWindowTitle("\345\215\225\350\257\215\345\255\246\344\271\240");//单词学习
}

void MainWindow::on_MButtonNewWord_clicked()
{
	ui->MaingroupBox->setVisible(false);
	ui->NMgroupBox->setVisible(true);
	m_NMListWidget->clear();
	QStringList NameList = m_People->GetNewWordListName();
	UpDateNMWin();
}

void MainWindow::on_NMActionDelete_clicked()
{
	QString Text = m_NMListWidget->currentItem()->text();
	QTextStream Stream(&Text);
	QString Name;
	Stream >> Name;
	bool Success = m_People->DeleteNewWordList(Name);
	if (Success)
	{
		QMessageBox::information(this, "\346\263\250\346\204\217", "\345\210\240\351\231\244\346\210\220\345\212\237"); //注意  删除成功
	}
	else
	{
		QMessageBox::warning(this, "\346\263\250\346\204\217", "\346\227\240\346\263\225\345\210\240\351\231\244\351\273\230\350\256\244\347\224\237\350\257\215\346\234\254"); //注意  无法删除默认生词本
	}
	UpDateNMWin();
}

void MainWindow::on_NMActionAddNew_clicked()
{
	QString Name;
	bool BeDafault;
	AddNWListDialog Dialog(Name, BeDafault, this);
	if (Dialog.exec() == QDialog::Accepted)
	{
		m_People->AddNewWordList(Name);
		if (BeDafault)
			m_People->SetDefNewWordList(Name);
	}
	UpDateNMWin();
}

void MainWindow::on_NMButtonBack_clicked()
{
	ui->MaingroupBox->setVisible(true);
	ui->NMgroupBox->setVisible(false);
    UpDateMainWindows();
}

void MainWindow::on_NMButtonOpen_clicked()
{
	ui->NMgroupBox->setVisible(false);
	ui->NgroupBox->setVisible(true);
	QString Text = m_NMListWidget->currentItem()->text();
	QTextStream Stream(&Text);
	QString Name;
	Stream >> Name;
	m_NLNowWord = m_People->OpenNewWordList(Name);
	ui->NgroupBox->setTitle(Name);
	UpDateNewWordWin(m_NLNowWord);
}

void MainWindow::on_NMSetDefault_clicked()
{
	QString Text = m_NMListWidget->currentItem()->text();
	QTextStream Stream(&Text);
	QString Name;
	Stream >> Name;
	m_People->SetDefNewWordList(Name);
	UpDateNMWin();
}

void MainWindow::UpDateNMWin()
{
	m_NMListWidget->clear();
	QStringList NameList = m_People->GetNewWordListName();
	NameList[m_People->GetDefaultListID()] += "\t(\351\273\230\350\256\244)"; //默认
	m_NMListWidget->addItems(NameList);
	m_NMListWidget->setCurrentRow(0);
	setWindowTitle("\347\224\237\350\257\215\346\234\254");//生词本
}

void MainWindow::on_NButtonBack_clicked()
{
	ui->NgroupBox->setVisible(false);
    ui->NMgroupBox->setVisible(true);
}
void MainWindow::on_NButtonARNew_clicked()
{
	m_NLNowWord = m_People->RemFromNowNewList(m_NLNowWord);
	UpDateNewWordWin(m_NLNowWord);
}

void MainWindow::on_NButtonPrev_clicked()
{
	m_NLNowWord = m_People->GetNLPrevWord();
	UpDateNewWordWin(m_NLNowWord);
}

void MainWindow::on_NButtonNext_clicked()
{
	m_NLNowWord = m_People->GetNLNextWord();
	UpDateNewWordWin(m_NLNowWord);
}

void MainWindow::on_NButtonRead_clicked()
{
    CSpeak::Speak(m_NLNowWord->GetWord());
}

void MainWindow::UpDateNewWordWin(CWord* Word)
{
	if (Word == NULL)
	{
		ui->NlabelWord->setText(" ");
		ui->NlabelRead->setText(" ");
		ui->NlabelMean->setText("\346\262\241\346\234\211\347\224\237\350\257\215");//没有生词
		ui->NButtonRead->setEnabled(false);
		ui->NButtonARNew->setEnabled(false);
		ui->NButtonNext->setEnabled(false);
		ui->NButtonPrev->setEnabled(false);
		return;
	}
	ui->NButtonRead->setEnabled(true);
	ui->NButtonARNew->setEnabled(true);
	ui->NlabelWord->setText(Word->GetWord());
	ui->NlabelRead->setText(Word->GetRead());
	QString Temp;
	QVector<QString> Means = Word->GetMeans();
	for (int i = 0; i < Means.size(); i++)
	{
		Temp += Means[i];
		Temp += "\n";
	}
	ui->NlabelMean->setText(Temp);
	ui->NButtonNext->setEnabled(m_People->NLHaveNext());
    ui->NButtonPrev->setEnabled(m_People->NLHavePrev());
}

void MainWindow::on_HaveRevTask()
{
	ui->MButtonReview->setEnabled(true);
	UpDateMainWindows();
}
void MainWindow::on_MButtonReview_clicked()
{
	ui->RgroupBox->setVisible(true);
	ui->MaingroupBox->setVisible(false);
	ui->RlabelWord->setVisible(false);
	ui->RlabelMean->setVisible(false);
	ui->RButtonFoget->setEnabled(false);
	ui->RButtonRem->setEnabled(false);
	ui->RButtonHMean->setVisible(false);
	ui->RButtonWordDone->setEnabled(false);
	m_RevNowWord = m_People->StartReview();
	UpDateRevWin(m_RevNowWord);
}

void MainWindow::on_RlineEdit_textChanged(const QString &arg1)
{
	if (arg1.trimmed() == "")
	{
		ui->RButtonWordDone->setEnabled(false);
		ui->RButtonFoget->setEnabled(false);
		ui->RButtonRem->setEnabled(false);
		return;
	}
	else
	{
		ui->RButtonWordDone->setEnabled(true);
		ui->RButtonFoget->setEnabled(false);
		ui->RButtonRem->setEnabled(false);
	}
}

void MainWindow::on_RButtonWordDone_clicked()
{
    QString Input = ui->RlineEdit->text().trimmed().toLower();
	QString Key = m_RevNowWord->GetWord();
	bool IsRight = (Input == Key);
	ui->RlabelWord->setVisible(true);
	if (IsRight)
	{
		ui->RlabelWord->setText("\346\255\243\347\241\256");//正确
	}
	else
	{
		ui->RlabelWord->setText("\351\224\231\350\257\257\n" + Key);//错误
	}
	ui->RButtonRem->setEnabled(IsRight);
	ui->RButtonFoget->setEnabled(true);
	ui->RButtonWordDone->setEnabled(false);
    ui->RlineEdit->setReadOnly(true);
}

void MainWindow::on_RButtonAMToList_clicked()
{
	if (m_People->DefNewListExist(m_RevNowWord))
	{
		ui->RButtonAMToList->setText("\346\267\273\345\212\240\345\210\260\347\224\237\350\257\215\346\234\254");//添加到生词本
		m_People->RemFromDefNewList(m_RevNowWord);
	}
	else
	{
		ui->RButtonAMToList->setText("\344\273\216\347\224\237\350\257\215\346\234\254\347\247\273\351\231\244");//从生词本移除
		m_People->AddToDefNewList(m_RevNowWord);
	}
}

void MainWindow::on_RButtonSMean_clicked()
{
	ui->RButtonHMean->setVisible(true);
	ui->RButtonSMean->setVisible(false);
	ui->RlabelMean->setVisible(true);
}

void MainWindow::on_RButtonHMean_clicked()
{
	ui->RButtonHMean->setVisible(false);
	ui->RButtonSMean->setVisible(true);
	ui->RlabelMean->setVisible(false);
}

void MainWindow::on_RButtonRem_clicked()
{
	m_RevNowWord = m_People->SetRevNowWordRem();
	UpDateRevWin(m_RevNowWord);
}

void MainWindow::on_RButtonFoget_clicked()
{
	m_RevNowWord = m_People->GetRevNextWord();
	UpDateRevWin(m_RevNowWord);
}

void MainWindow::on_RButtonBack_clicked()
{
	ui->RgroupBox->setVisible(false);
	ui->MaingroupBox->setVisible(true);
	UpDateMainWindows();
}

void MainWindow::on_RButtonRead_clicked()
{
    CSpeak::Speak(m_RevNowWord->GetWord());
}

void MainWindow::UpDateRevWin(CWord *Word)
{
	if (Word == NULL)
	{
		QMessageBox::information(this, "\346\201\255\345\226\234", "\344\275\240\345\267\262\347\273\217\345\256\214\346\210\220\346\211\200\346\234\211\345\244\215\344\271\240\350\256\241\345\210\222");
		//恭喜   你已经完成所有复习计划
		ui->RgroupBox->setVisible(false);
		ui->MaingroupBox->setVisible(true);
        UpDateMainWindows();
		return;
	}
	ui->RlineEdit->clear();
    ui->RlineEdit->setReadOnly(false);
	ui->RButtonHMean->setVisible(false);
	ui->RButtonSMean->setVisible(true);
	ui->RlabelWord->setVisible(false);
	ui->RlabelMean->setVisible(false);
	ui->RButtonRead->setEnabled(true);
	ui->RlabelRead->setText(Word->GetRead());
	QString Temp;
	QVector<QString> Means = Word->GetMeans();
	for (int i = 0; i < Means.size(); i++)
	{
		Temp += Means[i];
		Temp += "\n";
	}
    if (m_People->DefNewListExist(Word))
    {
        ui->RButtonAMToList->setText("\344\273\216\347\224\237\350\257\215\346\234\254\347\247\273\351\231\244");//从生词本移除
    }
    else
    {
        ui->RButtonAMToList->setText("\346\267\273\345\212\240\345\210\260\347\224\237\350\257\215\346\234\254");//添加到生词本
    }
	ui->RlabelMean->setText(Temp);
	ui->RButtonFoget->setEnabled(false);
	ui->RButtonRem->setEnabled(false);
	setWindowTitle("\345\215\225\350\257\215\345\244\215\344\271\240");//单词复习
}

void MainWindow::on_actionLogout_triggered()
{
	m_People->Save();
	delete m_People;
	m_People = NULL;
	hide();
	if (Logoin())
		show();
	else
		close();
}

void MainWindow::on_actionChangePlan_triggered()
{
    ui->PlangroupBox->setVisible(true);
    ui->MaingroupBox->setVisible(false);
    ui->PcomboBoxWordPac->setCurrentText(m_People->GetPackUseName());
    ui->PspinBoxRecNum->setValue(m_People->GetDayReciteNum());
    ui->PcheckBoxInOrder->setChecked(m_People->ReciteIsInOrder());
    m_PFlashPlanLabel();
}

void MainWindow::on_actionQuite_triggered()
{
    close();
}
