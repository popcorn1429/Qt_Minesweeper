#include "customdialog.h"
#include "ui_customdialog.h"
#include <QMessageBox>

CustomDialog::CustomDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomDialog) {
    ui->setupUi(this);
    ui->horizontalSlider_mine->setRange(1, ui->horizontalSlider_col->value()*ui->horizontalSlider_row->value()-1);
    ui->label_col_num->setText(QString::number(ui->horizontalSlider_col->value()));
    ui->label_row_num->setText(QString::number(ui->horizontalSlider_row->value()));
    ui->label_mine_num->setText(QString::number(ui->horizontalSlider_mine->value()));
}

CustomDialog::~CustomDialog() {
    delete ui;
}

void CustomDialog::getCustomConf(int &row, int &col, int &mine) {
    row = ui->horizontalSlider_row->value();
    col = ui->horizontalSlider_col->value();
    mine = ui->horizontalSlider_mine->value();
}

bool CustomDialog::validate() {
    return ui->horizontalSlider_mine->value() < ui->horizontalSlider_row->value() * ui->horizontalSlider_col->value();
}

void CustomDialog::accept() {
    if (!validate()) {
        QMessageBox::warning(this, QStringLiteral("警告"),
                             QStringLiteral("不合理的设置"),
                             QMessageBox::Yes);
    }
    else {
        QDialog::accept();
    }
}

void CustomDialog::on_horizontalSlider_col_valueChanged(int value) {
    int maxMineNum = value * ui->horizontalSlider_row->value() - 1;
    ui->horizontalSlider_mine->setRange(1, maxMineNum);
    ui->label_col_num->setText(QString::number(ui->horizontalSlider_col->value()));
    ui->label_mine_num->setText(QString::number(ui->horizontalSlider_mine->value()));
}

void CustomDialog::on_horizontalSlider_row_valueChanged(int value) {
    int maxMineNum = value * ui->horizontalSlider_col->value() - 1;
    ui->horizontalSlider_mine->setRange(1, maxMineNum);
    ui->label_row_num->setText(QString::number(ui->horizontalSlider_row->value()));
    ui->label_mine_num->setText(QString::number(ui->horizontalSlider_mine->value()));
}

void CustomDialog::on_horizontalSlider_mine_valueChanged(int value) {
    ui->label_mine_num->setText(QString::number(value));
}
