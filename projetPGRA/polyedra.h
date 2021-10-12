// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#ifndef POLYEDRA_H
#define POLYEDRA_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>

class Polyedra
{
public:
    Polyedra(GLfloat ep_m, GLfloat width, GLfloat height, GLfloat height_t,
              GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool isPedal);
    ~Polyedra();
    void buildVertData(QVector<GLfloat> &data);
    void createPolyedra(QVector<GLfloat> &data, GLint sign);
    void createQuad(QVector<GLfloat> &data);
    void draw(QOpenGLShaderProgram *program, QOpenGLFunctions *glFuncs);

    //getters
    GLfloat widthQuad();
    GLfloat heightTrapezoid();
    GLfloat heightQuad();

private:
    QOpenGLBuffer m_vbo; 
    QOpenGLTexture *m_textures[2];
    GLfloat ep_m;
    GLfloat width;
    GLfloat height;
    GLfloat height_t;
    GLfloat coul_r;
    GLfloat coul_v;
    GLfloat coul_b;
    bool isPedal;
};

#endif // POLYEDRA_H
