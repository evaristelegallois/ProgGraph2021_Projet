// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#ifndef COGWHEEL_H
#define COGWHEEL_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class Cogwheel
{
public:
    Cogwheel(GLfloat ep_roue, GLfloat r_roue, GLfloat h_dent, GLint nb_dents,
              GLfloat coul_r, GLfloat coul_v, GLfloat coul_b);
    ~Cogwheel();
    void buildVertData(QVector<GLfloat> &data);
    void createPolygon(QVector<GLfloat> &data, GLdouble angle, GLint ind, GLint sign);
    void createQuad(QVector<GLfloat> &data, GLdouble angle, GLint ind);
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *glFuncs);

    //getters
    GLfloat radius();
    GLint nbCogs();
    GLfloat thickness();

private:
    QOpenGLBuffer m_vbo;
    GLfloat ep_roue;
    GLfloat r_roue;
    GLfloat h_dent;
    GLint nb_dents;
    GLfloat coul_r;
    GLfloat coul_v;
    GLfloat coul_b;
};

#endif // COGWHEEL_H
