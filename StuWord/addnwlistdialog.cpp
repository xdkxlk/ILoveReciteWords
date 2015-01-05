#include "addnwlistdialog.h"
#include "ui_addnwlistdialog.h"
#include <qregexp.h>

AddNWListDialog::AddNWListDialog(QString &Name, bool &BeSetDefault, QWidget *parent) :
QDialog(parent), m_Name(Name), m_BeSetDefault(BeSetDefault),
    ui(new Ui::AddNWListDialog)
{
    ui->setupUi(this);
	QRegExp Reg("\\w+");
	QRegExpValidator *pRegExp1 = new QRegExpValidator(Reg, this);
	ui->lineEdit->setValidator(pRegExp1);
	connect(ui->ButtonSubmit, SIGNAL(clicked()), this, SLOT(on_Submit_clicked()));
	setWindowTitle("\346\226\260\345\273\272\347\224\237\350\257\215\346\234\254");//新建生词本
}

AddNWListDialog::~AddNWListDialog()
{
    delete ui;
}

void AddNWListDialog::on_Submit_clicked()
{
	m_Name = ui->lineEdit->text();
	m_BeSetDefault = ui->checkBox->isChecked();
}