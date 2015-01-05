#ifndef ADDNWLISTDIALOG_H
#define ADDNWLISTDIALOG_H

#include <QDialog>

namespace Ui {
class AddNWListDialog;
}

class AddNWListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNWListDialog(QString &Name, bool &BeSetDefault, QWidget *parent = 0);
    ~AddNWListDialog();
private slots:
	void on_Submit_clicked();
private:
    Ui::AddNWListDialog *ui;
	QString &m_Name;
	bool &m_BeSetDefault;
};

#endif // ADDNWLISTDIALOG_H
