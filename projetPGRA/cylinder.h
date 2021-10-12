// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#ifndef CYLINDER_H
#define CYLINDER_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class Cylinder
{
public:
    Cylinder(GLfloat ep_cyl, GLfloat r_cyl, GLint nb_fac,
              GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool phong_shading = true);
    ~Cylinder();
    void buildVertData(QVector<GLfloat> &data);
    void createTriangle(QVector<GLfloat> &data, GLdouble angle, GLint ind, GLint sign);
    void createQuad(QVector<GLfloat> &data, GLdouble angle, GLint ind);
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *glFuncs);

    //getters
    GLfloat radius();
    GLfloat thickness();

private:
    bool m_phong_shading = true;
    QOpenGLBuffer m_vbo;
    GLfloat ep_cyl;
    GLfloat r_cyl;
    GLint nb_fac;
    GLfloat coul_r;
    GLfloat coul_v;
    GLfloat coul_b;
};

#endif // CYLINDER_H
