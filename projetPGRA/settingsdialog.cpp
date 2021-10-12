// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

//emission des signaux après clic sur les boutons correspondants
void SettingsDialog::on_btn_start_clicked()
{
    qDebug() << __FUNCTION__;
    emit startButtonClicked();
}

void SettingsDialog::on_btn_stop_clicked()
{
    qDebug() << __FUNCTION__;
    emit stopButtonClicked();
}

void SettingsDialog::on_btn_speedUp_clicked()
{
    qDebug() << __FUNCTION__;
    emit speedUpButtonClicked();
}

void SettingsDialog::on_btn_slowDown_clicked()
{
    qDebug() << __FUNCTION__;
    emit slowDownButtonClicked();
}
