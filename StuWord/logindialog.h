#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QString & name,QString & password, QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void on_lineEditName_textChanged(const QString &arg1);

    void on_lineEditPass_textChanged(const QString &arg1);

    void ButtonLogoinClicked();

    void RegistClicked();

    void SubmitRegistClicked();

    void on_ButtonBack_clicked();

private:
    Ui::LoginDialog *ui;
    QString & m_Name;
    QString & m_Password;
    bool m_IsRegisting;
};

#endif // LOGINDIALOG_H
