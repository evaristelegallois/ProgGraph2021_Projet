// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#ifndef PRINC_H
#define PRINC_H

#include "ui_princ.h"
#include "settingsdialog.h"

class Princ : public QMainWindow, private Ui::Princ
{
    Q_OBJECT

public:
    explicit Princ(QWidget *parent = 0);

public slots:
    void setSliderRadius(double radius);
    void displaySettings();

protected slots:
    void onSliderRadius(int value);

//pour ouvrir la fenêtre de paramètres
private slots:
    void on_btn_settings_clicked();

//pour agir sur la scène depuis la fenêtre de paramètres
public slots:
    void start();
    void stop();
    void speedUp();
    void slowDown();

//instanciation de la fenêtre de paramètres
private:
    Ui::Princ *ui;
    SettingsDialog *settingsDialog = new SettingsDialog(this);
};

#endif // PRINC_H
