#include "detaildialog.h"
#include "ui_detaildialog.h"
#include <QMessageBox>
#include <QPushButton>

DetailDialog::DetailDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::DetailDialog)
{
    ui->setupUi(this);
    setWindowTitle("Деталь");

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked,
            this, &DetailDialog::onAccept);
    connect(ui->buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked,
            this, &QDialog::reject);
}

DetailDialog::~DetailDialog() { delete ui; }

QString DetailDialog::article() const { return ui->lineArticle->text().trimmed(); }
QString DetailDialog::name() const { return ui->lineName->text().trimmed(); }

void DetailDialog::setArticle(const QString &article) { ui->lineArticle->setText(article); }
void DetailDialog::setName(const QString &name) { ui->lineName->setText(name); }

void DetailDialog::onAccept()
{
    if (article().isEmpty() || name().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Артикул и название обязательны!");
        return;
    }
    accept();
}
