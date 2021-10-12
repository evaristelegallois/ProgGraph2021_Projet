// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#include "princ.h"
#include "settingsdialog.h"
#include <QDebug>

Princ::Princ(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);

    connect (glarea, SIGNAL(radiusChanged(double)), this, SLOT(setSliderRadius(double)));
    connect (sli_radius, SIGNAL(valueChanged(int)), this, SLOT(onSliderRadius(int)));

    //connexion des slots de princ aux signaux émis par la fenêtre de paramètres
    //ne fonctionne pas car la connexion ne se fait pas, mais nous ne savons pas pourquoi
    connect(settingsDialog, SIGNAL(startButtonClicked()),this, SLOT(start()));
    connect(settingsDialog, SIGNAL(stopButtonClicked()),this, SLOT(stop()));
    connect(settingsDialog, SIGNAL(speedUpButtonClicked()),this, SLOT(speedUp()));
    connect(settingsDialog, SIGNAL(slowDownButtonClicked()),this, SLOT(slowDown()));
}

void Princ::setSliderRadius(double radius)
{
    qDebug() << __FUNCTION__ << radius << sender();
    int value = radius*20;
    if (sli_radius->value() != value) {
        qDebug() << "  sli_radius->setvalue()";
        sli_radius->setValue(value);
    }
}

void Princ::onSliderRadius(int value)
{
    qDebug() << __FUNCTION__ << value << sender();
    qDebug() << "  emit radiusChanged()";
    emit glarea->radiusChanged(value/20.0);
}

//slots permettant d'agir sur l'animation dans glarea
//le programme n'entre pas dans ces fonctions car les prints
//ne sont jamais affichés; il semblerait donc que les connexions
//signal/slot ne se fassent pas correctement
void Princ::start()
{
    qDebug() << __FUNCTION__ << sender();
    ui->glarea->start();
    update();
}

void Princ::stop()
{
    qDebug() << __FUNCTION__ << sender();
    ui->glarea->stop();
    update();
}

void Princ::speedUp()
{
    qDebug() << __FUNCTION__ << sender();
    ui->glarea->speedUp();
    update();
}

void Princ::slowDown()
{
    qDebug() << __FUNCTION__ << sender();
    ui->glarea->slowDown();
    update();
}

//fonction d'affichage de la fenêtre de paramètres
bool set = true;
void Princ::displaySettings()
{
    SettingsDialog *cls = new SettingsDialog(this);
    cls->setModal(false);
    cls->show();
    set = true;
    int returnType = cls->exec();
    if(returnType==1){
        set = false;
    }else{
        set = false;
    }
}

void Princ::on_btn_settings_clicked()
{
    displaySettings();
}
