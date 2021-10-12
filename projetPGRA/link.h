// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#ifndef LINK_H
#define LINK_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "polyedra.h"
#include "cylinder.h"

class Link
{
public:
    Link(GLfloat ep_m, GLfloat width, GLfloat height, GLfloat height_t,
         GLfloat ep_cyl, GLfloat r_cyl, GLfloat nb_fac,
              GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool external = true);
    ~Link();

    //getters
    Cylinder* getCylinder(int index);
    Polyedra* getPolyedra(int index);


private:
    bool m_external = true;
    QOpenGLBuffer m_vbo;
    //polyèdres
    GLfloat ep_m;
    GLfloat width;
    GLfloat height;
    GLfloat height_t;
    //cylindre
    GLfloat ep_cyl;
    GLfloat r_cyl;
    GLint nb_fac;
    GLfloat coul_r;
    GLfloat coul_v;
    GLfloat coul_b;

    //instanciation des membres de la classe
    Cylinder *cylinder1 = nullptr;
    Cylinder *cylinder2 = nullptr;
    Polyedra *polyedra1 = nullptr;
    Polyedra *polyedra2 = nullptr;
};

#endif // LINK_H
