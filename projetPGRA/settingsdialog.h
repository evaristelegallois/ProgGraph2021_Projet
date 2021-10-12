// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private:
    Ui::SettingsDialog *ui;

//signals et slots pour intéractions avec princ
signals :
    void startButtonClicked();
    void stopButtonClicked();
    void speedUpButtonClicked();
    void slowDownButtonClicked();

private slots:
    void on_btn_start_clicked();
    void on_btn_stop_clicked();
    void on_btn_speedUp_clicked();
    void on_btn_slowDown_clicked();
};
#endif // SETTINGSDIALOG_H
