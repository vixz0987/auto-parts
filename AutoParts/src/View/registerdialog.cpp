#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "../Controller/authcontroller.h"
#include <QMessageBox>
#include <QRegularExpression>

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setWindowTitle("Активация учётной записи");

    // Подключаем кнопки
    connect(ui->btnRegister, &QPushButton::clicked,
            this, &RegisterDialog::onRegisterClicked);
    connect(ui->btnCancel, &QPushButton::clicked,
            this, &QDialog::reject);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::onRegisterClicked()
{
    QString login = ui->lineLogin->text().trimmed();
    QString fio = ui->lineFio->text().trimmed();
    QString password = ui->linePassword->text();
    QString confirm = ui->lineConfirmPassword->text();

    // Проверка заполнения
    if (login.isEmpty() || fio.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Все поля обязательны для заполнения!");
        return;
    }

    // Совпадение паролей
    if (password != confirm) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают!");
        return;
    }

    // Сложность пароля
    QRegularExpression re("^(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*()_+=-]).{8,}$");
    if (!re.match(password).hasMatch()) {
        QMessageBox::warning(this, "Слабый пароль",
                             "Пароль должен быть не менее 8 символов, содержать "
                             "заглавную букву, цифру и спецсимвол (!@#$%^&*()_+=-).");
        return;
    }

    // Пытаемся активировать учётную запись
    bool success = AuthController::activateUser(login, fio, password);
    if (success) {
        QMessageBox::information(this, "Успех",
                                 "Учётная запись активирована! Теперь вы можете войти.");
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось активировать учётную запись.\n"
                              "Проверьте правильность логина и ФИО, а также не активирована ли она ранее.");
    }
}