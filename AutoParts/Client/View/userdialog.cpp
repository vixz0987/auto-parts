#include "userdialog.h"
#include "ui_userdialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QComboBox>

UserDialog::UserDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserDialog)
{
    ui->setupUi(this);
    setWindowTitle("Новый пользователь");

    // Заполняем комбобокс ролей
    ui->comboRole->addItems({"admin", "manager", "accountant"});

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &UserDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);
}

UserDialog::~UserDialog()
{
    delete ui;
}

QString UserDialog::login() const { return ui->lineLogin->text().trimmed(); }
QString UserDialog::fio() const { return ui->lineFio->text().trimmed(); }
QString UserDialog::role() const { return ui->comboRole->currentText(); }

void UserDialog::setLogin(const QString &login) { ui->lineLogin->setText(login); }
void UserDialog::setFio(const QString &fio) { ui->lineFio->setText(fio); }
void UserDialog::setRole(const QString &role) {
    ui->comboRole->setCurrentText(role);
}

void UserDialog::setLoginReadOnly(bool readOnly) {
    ui->lineLogin->setReadOnly(readOnly);
}

void UserDialog::onAccept()
{
    if (login().isEmpty() || fio().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля!");
        return;
    }
    accept();
}
