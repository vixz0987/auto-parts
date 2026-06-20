#include "profiledialog.h"
#include "ui_profiledialog.h"
#include "Services/sessionmanager.h"
#include <QMessageBox>
#include <QRegularExpression>

ProfileDialog::ProfileDialog(ClientService* service, int userId, QWidget *parent)
    : QDialog(parent), ui(new Ui::ProfileDialog), m_service(service), m_userId(userId)
{
    ui->setupUi(this);
    setWindowTitle("Профиль пользователя");

    UserData user = SessionManager::instance()->currentUser();
    ui->lineFio->setText(user.fio);
    ui->lineLogin->setText(user.login);
    ui->lineLogin->setReadOnly(true);

    connect(ui->buttonSave, &QPushButton::clicked, this, &ProfileDialog::onSave);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_service, &ClientService::operationSuccess, this, &ProfileDialog::onOperationSuccess);
    connect(m_service, &ClientService::operationError, this, &ProfileDialog::onOperationError);
}

ProfileDialog::~ProfileDialog() { delete ui; }

void ProfileDialog::onSave()
{
    QString newFio = ui->lineFio->text().trimmed();
    QString oldPass = ui->lineOldPassword->text();
    QString newPass = ui->lineNewPassword->text();
    QString confirmPass = ui->lineConfirmPassword->text();

    if (!oldPass.isEmpty() || !newPass.isEmpty() || !confirmPass.isEmpty()) {
        if (oldPass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Для смены пароля заполните все три поля!");
            return;
        }
        if (newPass != confirmPass) {
            QMessageBox::warning(this, "Ошибка", "Новый пароль и подтверждение не совпадают!");
            return;
        }
        QRegularExpression re("^(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*()_+=-]).{8,}$");
        if (!re.match(newPass).hasMatch()) {
            QMessageBox::warning(this, "Слабый пароль",
                                 "Пароль должен быть не менее 8 символов, содержать заглавную букву, цифру и спецсимвол.");
            return;
        }
        ui->buttonSave->setEnabled(false);
        ui->buttonCancel->setEnabled(false);
        m_service->changePassword(m_userId, oldPass, newPass);
        return;
    }

    if (newFio.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите новое ФИО или заполните поля для смены пароля.");
        return;
    }
    UserData user = SessionManager::instance()->currentUser();
    if (newFio == user.fio) {
        QMessageBox::information(this, "Информация", "ФИО не изменилось.");
        return;
    }
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);
    m_service->changeFio(m_userId, newFio);
}

void ProfileDialog::onOperationSuccess(quint32 /*requestId*/, const QString& message)
{
    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
    if (message.startsWith("FIO_CHANGED")) {
        UserData user = SessionManager::instance()->currentUser();
        user.fio = ui->lineFio->text().trimmed();
        SessionManager::instance()->setCurrentUser(user);
    }
    QMessageBox::information(this, "Успех", "Данные успешно обновлены!");
    accept();
}

void ProfileDialog::onOperationError(quint32 /*requestId*/, const QString& error)
{
    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);
    QMessageBox::critical(this, "Ошибка", error);
}