#include "logindialog.h"
#include "ui_logindialog.h"
#include "cpeople.h"
#include <QRegExp>
#include <qdesktopwidget.h>

LoginDialog::LoginDialog(QString &name, QString &password, QWidget *parent) :
    QDialog(parent), m_Name(name), m_Password(password),
    ui(new Ui::LoginDialog)
{
    m_IsRegisting = false;
    ui->setupUi(this);
    ui->lineEditPass->setEchoMode(QLineEdit::Password);
    ui->ButtonBack->setVisible(false);

    QRegExp rx1("[0-9,a-z,A-Z]+");
    QRegExpValidator *pRegExp1 = new QRegExpValidator(rx1,this);
    ui->lineEditName->setValidator(pRegExp1);
    ui->labelNameInfo->setVisible(false);
    QPalette pal =ui->labelNameInfo->palette();
    pal.setColor(QPalette::Text,QColor(255,0,0));
    ui->labelNameInfo->setPalette(pal);

    QRegExp rx2("[0-9,a-z,A-Z]+");
    QRegExpValidator *pRegExp2 = new QRegExpValidator(rx2,this);
    ui->lineEditPass->setValidator(pRegExp2);
    ui->labelPassInfo->setVisible(false);
    pal =ui->labelPassInfo->palette();
    pal.setColor(QPalette::Text,QColor(255,0,0));
    ui->labelPassInfo->setPalette(pal);

    ui->labelPassAgain->setVisible(false);
    ui->lineEditPassAgain->setVisible(false);
    ui->lineEditPassAgain->setValidator(pRegExp2);

    ui->ButtonLogin->setEnabled(false);

	setWindowTitle("\347\231\273\351\231\206");//登陆
    connect(ui->ButtonLogin,SIGNAL(clicked()),this,SLOT(ButtonLogoinClicked()));
    connect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(RegistClicked()));
	setFixedSize(250, 150);

	QDesktopWidget *pDesk = QApplication::desktop();
	move((pDesk->width() - width()) / 2, (pDesk->height() - height()) / 2);
    //resize(sizeHint());
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_lineEditName_textChanged(const QString &arg1)
{
    if(arg1!="" && ui->lineEditPass->text()!="")
    {
        if(m_IsRegisting)
            ui->ButtonRegist->setEnabled(true);
        else
            ui->ButtonLogin->setEnabled(true);
    }
    else
    {
        if(m_IsRegisting)
            ui->ButtonRegist->setEnabled(false);
        else
            ui->ButtonLogin->setEnabled(false);
    }
}

void LoginDialog::on_lineEditPass_textChanged(const QString &arg1)
{
	QString temp = ui->lineEditName->text();
	if (arg1 != "" && temp != "")
    {
        if(m_IsRegisting)
            ui->ButtonRegist->setEnabled(true);
        else
            ui->ButtonLogin->setEnabled(true);
    }
    else
    {
        if(m_IsRegisting)
            ui->ButtonRegist->setEnabled(false);
        else
            ui->ButtonLogin->setEnabled(false);
    }
}

void LoginDialog::ButtonLogoinClicked()
{
    if(CPeople::TheNameExist(ui->lineEditName->text()))
    {
        if(CPeople::ThePassRight(ui->lineEditName->text(), ui->lineEditPass->text()))
        {
            m_Name = ui->lineEditName->text();
            m_Password = ui->lineEditPass->text();
            disconnect(ui->ButtonLogin,SIGNAL(clicked()),this,SLOT(ButtonLogoinClicked()));
            connect(ui->ButtonLogin,SIGNAL(clicked()),this,SLOT(accept()));
            emit ui->ButtonLogin->clicked();
            return;
        }
        ui->labelPassInfo->setVisible(true);
        //密码错误
        ui->labelPassInfo->setText("\345\257\206\347\240\201\351\224\231\350\257\257");
        return;
    }
    ui->labelNameInfo->setVisible(true);
    //用户名不存在
    ui->labelNameInfo->setText("\347\224\250\346\210\267\345\220\215\344\270\215\345\255\230\345\234\250");
}

void LoginDialog::RegistClicked()
{
    m_IsRegisting = true;
	ui->ButtonRegist->setEnabled(false);
    ui->labelNameInfo->setVisible(false);
    ui->labelPassInfo->setVisible(false);
    ui->lineEditName->clear();
    ui->lineEditPass->clear();
	ui->lineEditPassAgain->clear();
    ui->ButtonLogin->setVisible(false);
    ui->lineEditPassAgain->setVisible(true);
    ui->labelPassAgain->setVisible(true);
    ui->ButtonBack->setVisible(true);

    disconnect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(RegistClicked()));
    connect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(SubmitRegistClicked()));
    //resize(sizeHint());
}

void LoginDialog::SubmitRegistClicked()
{
    ui->labelNameInfo->setVisible(false);
    ui->labelPassInfo->setVisible(false);
    if(CPeople::TheNameExist(ui->lineEditName->text()))
    {
        ui->labelNameInfo->setVisible(true);
        //此用户名已经存在
        ui->labelNameInfo->setText("\346\255\244\347\224\250\346\210\267\345\220\215\345\267\262\347\273\217\345\255\230\345\234\250");
        return;
    }
    if(ui->lineEditPass->text()!=ui->lineEditPassAgain->text())
    {
        ui->labelNameInfo->setVisible(true);
        //两次输入的密码不同
        ui->labelNameInfo->setText("\344\270\244\346\254\241\350\276\223\345\205\245\347\232\204\345\257\206\347\240\201\344\270\215\345\220\214");
        return;
    }
    m_IsRegisting = false;
    ui->ButtonBack->setVisible(false);
    ui->labelNameInfo->setVisible(false);
    ui->labelPassInfo->setVisible(false);
    ui->ButtonLogin->setVisible(true);
    ui->lineEditPassAgain->setVisible(false);
    ui->labelPassAgain->setVisible(false);
    CPeople::RegistPeo(ui->lineEditName->text(), ui->lineEditPass->text());
    ui->lineEditName->clear();
    ui->lineEditPass->clear();
    connect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(RegistClicked()));
    disconnect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(SubmitRegistClicked()));
    //resize(sizeHint());
}

void LoginDialog::on_ButtonBack_clicked()
{
    ui->ButtonBack->setVisible(false);
    m_IsRegisting = false;
    ui->labelNameInfo->setVisible(false);
    ui->labelPassInfo->setVisible(false);
    ui->ButtonLogin->setVisible(true);
	ui->ButtonRegist->setEnabled(true);
    ui->lineEditPassAgain->setVisible(false);
    ui->labelPassAgain->setVisible(false);
    ui->lineEditName->clear();
    ui->lineEditPass->clear();
    connect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(RegistClicked()));
    disconnect(ui->ButtonRegist,SIGNAL(clicked()),this,SLOT(SubmitRegistClicked()));
    //resize(sizeHint());
}
