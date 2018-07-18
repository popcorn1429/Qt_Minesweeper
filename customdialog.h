#ifndef CUSTOMDIALOG_H
#define CUSTOMDIALOG_H

#include <QDialog>

namespace Ui {
class CustomDialog;
}

class CustomDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CustomDialog(QWidget *parent = 0);
    ~CustomDialog();

    void getCustomConf(int& row, int& col, int& mine);

private:
    bool validate();

public slots:
    virtual void accept();

private slots:
    void on_horizontalSlider_col_valueChanged(int value);

    void on_horizontalSlider_row_valueChanged(int value);

    void on_horizontalSlider_mine_valueChanged(int value);

private:
    Ui::CustomDialog *ui;
};

#endif // CUSTOMDIALOG_H
