#include "profiledialog.h"
#include "ui_profiledialog.h"
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include "Utils/sessionmanager.h"

ProfileDialog::ProfileDialog(TcpClient *client, int userId, QWidget *parent)
    : QDialog(parent), ui(new Ui::ProfileDialog), m_client(client), m_userId(userId)
{
    ui->setupUi(this);
    setWindowTitle("Профиль пользователя");

    // Заполняем текущие данные из сессии
    UserInfo user = SessionManager::instance()->currentUser();
    ui->lineFio->setText(user.fio);
    ui->lineLogin->setText(user.login);
    ui->lineLogin->setReadOnly(true); // логин менять нельзя

    connect(ui->buttonSave, &QPushButton::clicked, this, &ProfileDialog::onSave);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_client, &TcpClient::responseReceived, this, &ProfileDialog::onServerResponse);
}

ProfileDialog::~ProfileDialog()
{
    delete ui;
}

void ProfileDialog::onSave()
{
    QString newFio = ui->lineFio->text().trimmed();
    QString oldPass = ui->lineOldPassword->text();
    QString newPass = ui->lineNewPassword->text();
    QString confirmPass = ui->lineConfirmPassword->text();

    // Проверяем, если пароль не пустой, значит его меняем
    if (!oldPass.isEmpty() || !newPass.isEmpty() || !confirmPass.isEmpty()) {
        // Меняем пароль
        if (oldPass.isEmpty() || newPass.isEmpty() || confirmPass.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Для смены пароля заполните все три поля!");
            return;
        }
        if (newPass != confirmPass) {
            QMessageBox::warning(this, "Ошибка", "Новый пароль и подтверждение не совпадают!");
            return;
        }
        // Проверка сложности нового пароля
        QRegularExpression re("^(?=.*[A-Z])(?=.*\\d)(?=.*[!@#$%^&*()_+=-]).{8,}$");
        if (!re.match(newPass).hasMatch()) {
            QMessageBox::warning(this, "Слабый пароль",
                                 "Пароль должен быть не менее 8 символов, содержать заглавную букву, цифру и спецсимвол.");
            return;
        }
        // Отправляем запрос на смену пароля
        QJsonObject obj;
        obj["userId"] = m_userId;
        obj["oldPassword"] = oldPass;
        obj["newPassword"] = newPass;
        QJsonDocument doc(obj);
        m_pendingId = m_client->sendCommand("CHANGEPASSWORD " + doc.toJson(QJsonDocument::Compact));
        ui->buttonSave->setEnabled(false);
        ui->buttonCancel->setEnabled(false);
        return;
    }

    // Если пароль не меняем, но меняем ФИО
    if (newFio.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите новое ФИО или заполните поля для смены пароля.");
        return;
    }
    // Проверяем, что ФИО изменилось
    UserInfo user = SessionManager::instance()->currentUser();
    if (newFio == user.fio) {
        QMessageBox::information(this, "Информация", "ФИО не изменилось.");
        return;
    }
    // Отправляем запрос на смену ФИО
    QJsonObject obj;
    obj["userId"] = m_userId;
    obj["newFio"] = newFio;
    QJsonDocument doc(obj);
    m_pendingId = m_client->sendCommand("CHANGEFIO " + doc.toJson(QJsonDocument::Compact));
    ui->buttonSave->setEnabled(false);
    ui->buttonCancel->setEnabled(false);
}

void ProfileDialog::onServerResponse(quint32 id, const QString &response)
{
    if (id != m_pendingId) return;

    ui->buttonSave->setEnabled(true);
    ui->buttonCancel->setEnabled(true);

    if (response.startsWith("OK PASSWORD_CHANGED") || response.startsWith("OK FIO_CHANGED")) {
        // Обновляем данные в сессии
        UserInfo user = SessionManager::instance()->currentUser();
        if (response.startsWith("OK FIO_CHANGED")) {
            user.fio = ui->lineFio->text().trimmed();
        }
        // Если пароль изменился, мы не знаем новый пароль, но сессия остаётся, просто обновляем данные
        // Можно также заново запросить информацию о пользователе, но пока просто обновим локально
        SessionManager::instance()->setCurrentUser(user);
        QMessageBox::information(this, "Успех", "Данные успешно обновлены!");
        accept();
    } else {
        QString err = "Ошибка обновления данных.";
        if (response.startsWith("ERROR ")) err = response.mid(6);
        QMessageBox::critical(this, "Ошибка", err);
    }
}