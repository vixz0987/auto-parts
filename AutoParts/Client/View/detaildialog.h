#ifndef DETAILDIALOG_H
#define DETAILDIALOG_H

#include <QDialog>

namespace Ui { class DetailDialog; }

class DetailDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DetailDialog(QWidget *parent = nullptr);
    ~DetailDialog();

    QString article() const;
    QString name() const;

    void setArticle(const QString &article);
    void setName(const QString &name);

private slots:
    void onAccept();

private:
    Ui::DetailDialog *ui;
};

#endif // DETAILDIALOG_H

