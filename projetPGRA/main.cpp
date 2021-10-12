// CC-BY Edouard.Thiel@univ-amu.fr - 05/02/2021

#include "princ.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Princ w;
    w.show();

    return a.exec();
}
