// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#include "link.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>
#include <math.h>

// Cette classe doit être instanciée lorsqu'il y a un contexte GL courant,
// par exemple dans GLArea::makeGLObject.

Link::Link(GLfloat ep_m, GLfloat width, GLfloat height, GLfloat height_t,
                GLfloat ep_cyl, GLfloat r_cyl, GLfloat nb_fac,
                   GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool external) :
    ep_m(ep_m), width(width), height(height), height_t(height_t),
    ep_cyl(ep_cyl), r_cyl(r_cyl), nb_fac(nb_fac),
    coul_r(coul_r), coul_v(coul_v), coul_b(coul_b)
{
    m_external = external; //true si maillon externe

    //nous avons choisi d'instancier les membres de la classe Link et d'implémenter des
    //getters pour les dessiner individuellement depuis une instance de Link
    if (m_external)
    {
        //les maillons externes sont composés de deux cylindres en plus des polyèdres
        cylinder1 = new Cylinder(ep_cyl,r_cyl,nb_fac,1-coul_r,coul_v,1-coul_b);
        cylinder2 = new Cylinder(ep_cyl,r_cyl,nb_fac,1-coul_r,coul_v,1-coul_b);
        polyedra1 = new Polyedra(ep_m,width,height,height_t, coul_r,coul_v,coul_b, false);
        polyedra2 = new Polyedra(ep_m,width,height,height_t, coul_r,coul_v,coul_b, false);
    }
    else
    {
        polyedra1 = new Polyedra(ep_m,width,height,height_t, coul_r,coul_v,coul_b, false);
        polyedra2 = new Polyedra(ep_m,width,height,height_t, coul_r,coul_v,coul_b, false);
    }
}

Link::~Link()
{
}

//getters
Cylinder* Link::getCylinder(int index)
{
    if (index==1) return cylinder1;
    else if (index==2) return cylinder2;

    return 0;
}

Polyedra* Link::getPolyedra(int index)
{
    if (index==1) return polyedra1;
    else if (index==2) return polyedra2;

    return 0;
}

