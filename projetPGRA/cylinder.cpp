// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#include "cylinder.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>
#include <math.h>

// Cette classe doit être instanciée lorsqu'il y a un contexte GL courant,
// par exemple dans GLArea::makeGLObject.

Cylinder::Cylinder(GLfloat ep_cyl, GLfloat r_cyl, GLint nb_fac,
                   GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool phong_shading) :
    ep_cyl(ep_cyl), r_cyl(r_cyl), nb_fac(nb_fac),
    coul_r(coul_r), coul_v(coul_v), coul_b(coul_b)
{
    m_phong_shading = phong_shading;

    //initialisation du VBO
    m_vbo.create();
    m_vbo.bind();
    QVector<GLfloat> vertData;
    buildVertData(vertData);
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
    m_vbo.release();
}


Cylinder::~Cylinder()
{
    m_vbo.destroy();
}

void Cylinder::createTriangle(QVector<GLfloat> &data, GLdouble angle, GLint ind, GLint sign)
{
        //vertices
        QVector<QVector3D> vs = {
            QVector3D( 0, 0, sign*ep_cyl/2), //A
            QVector3D(r_cyl*cos(angle*ind), r_cyl*sin(angle*ind), sign*ep_cyl/2), //B
            QVector3D(r_cyl*cos(angle*(1+ind)),  r_cyl*sin(angle*(1+ind)),  sign*ep_cyl/2) //C
        };

                       // A  B  C
        int ind_ver[] = { 0, 1, 2 };

        QColor color = QColor::fromRgbF(coul_r, coul_v, coul_b);

        QVector3D vAB = vs[1]-vs[0],
                  vAC = vs[2]-vs[0];

        // normal() calcule la normale normalisée
        QVector3D nACB = QVector3D::normal(vAB, vAC);
        //qDebug() << "normal ACB :" << nACB;


        //pour la face arrière
        if(sign < 0) nACB = -nACB;

        //triangle = 3 sommets
        for (int i = 0; i < 3; ++i) {
            // coordonnées sommets
            data.append(vs[ind_ver[i]].x());
            data.append(vs[ind_ver[i]].y());
            data.append(vs[ind_ver[i]].z());
            // couleurs sommets
            data.append(color.redF());
            data.append(color.greenF());
            data.append(color.blueF());
            // normales sommets
            data.append(nACB.x());
            data.append(nACB.y());
            data.append(nACB.z());
        }
        //besoin de deux triangles pour lissage de phong, hmmm
}

void Cylinder::createQuad(QVector<GLfloat> &data, GLdouble angle, GLint ind)
{
    QVector<QVector3D> vs = {
        //facettes, premier triangle
        QVector3D(r_cyl*cos(angle*ind), r_cyl*sin(angle*ind), -ep_cyl/2), //B'
        QVector3D(r_cyl*cos(angle*(1+ind)),  r_cyl*sin(angle*(1+ind)),  -ep_cyl/2), //C'
        QVector3D(r_cyl*cos(angle*(1+ind)),  r_cyl*sin(angle*(1+ind)),  ep_cyl/2), //C

        //facettes, second triangle
        QVector3D(r_cyl*cos(angle*(1+ind)),  r_cyl*sin(angle*(1+ind)),  ep_cyl/2), //C
        QVector3D(r_cyl*cos(angle*ind), r_cyl*sin(angle*ind), ep_cyl/2), //B
        QVector3D(r_cyl*cos(angle*ind), r_cyl*sin(angle*ind), -ep_cyl/2), //B'
    };

                   // B' C' C  C  B  B'
    int ind_ver[] = { 0, 1, 2, 3, 4, 5 };

    QColor color = QColor::fromRgbF(0.6*coul_r, 0.6*coul_v, 0.6*coul_b);

    QVector3D vBpCp = vs[1]-vs[0],
              vBpC = vs[2]-vs[0],
              vCB = vs[4]-vs[3],
              vCBp = vs[5]-vs[3];

     // normal() calcule la normale normalisée
     QVector3D nBpCCp = QVector3D::normal(vBpC, vBpCp);
     QVector3D nCBpB = QVector3D::normal(vCBp, vCB);
     //qDebug() << "normal B'CC', CB'B :" << nBpCCp << nCBpB;

     QVector<QVector3D> normals;
     QVector<int> ind_nor;

     //lissage de phong
     if (m_phong_shading) {
        // normale interpolée
        QVector3D nBpC = (nBpCCp + nCBpB) / 2.0;
        normals = { nBpCCp, nCBpB, nBpC };
                  // B' C' C  C  B  B'
        ind_nor = { 2, 0, 2, 2, 1, 2 };

     } else {
        normals = { nBpCCp, nCBpB };
               // B' C' C  C  B  B'
         ind_nor = { 0, 0, 0, 1, 1, 1 };
     }

     //quad = 2*triangle = 6 sommets
     for (int i = 0; i < 6; ++i) {
            // coordonnées sommets
            data.append(vs[ind_ver[i]].x());
            data.append(vs[ind_ver[i]].y());
            data.append(vs[ind_ver[i]].z());
            // couleurs sommets
            data.append(color.redF());
            data.append(color.greenF());
            data.append(color.blueF());
            // normales sommets
            data.append(normals[ind_nor[i]].x());
            data.append(normals[ind_nor[i]].y());
            data.append(normals[ind_nor[i]].z());
        }
}

void Cylinder::buildVertData(QVector<GLfloat> &data)
{
    GLdouble angle = 360./nb_fac*M_PI/180;

    for(int i=0; i < nb_fac; i++){

        //on dessine d'abord les deux faces
        createTriangle(data, angle, i, 1);
        createTriangle(data, angle, i, -1);

        //puis les facettes
        createQuad(data, angle, i);
    }
}

void Cylinder::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *glFuncs)
{
    m_vbo.bind();

    program->setAttributeBuffer("posAttr",
        GL_FLOAT, 0 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
    program->setAttributeBuffer("colAttr",
        GL_FLOAT, 3 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
    program->setAttributeBuffer("norAttr",
        GL_FLOAT, 6 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
    program->enableAttributeArray("posAttr");
    program->enableAttributeArray("colAttr");
    program->enableAttributeArray("norAttr");

    //faces du cylindre, 2*nb_fac car 2 faces
    //facettes du cylindre 2*nb_fac car 2 triangles = 1 quad
    for(int i=0; i < 4*nb_fac; i++){

        // Pour des questions de portabilité, hors de la classe GLArea, tous les appels
        // aux fonctions glBidule doivent être préfixés par glFuncs->.
        glFuncs->glDrawArrays(GL_TRIANGLES, i*3, 3); // triangles
    }

    program->disableAttributeArray("posAttr");
    program->disableAttributeArray("colAttr");
    program->disableAttributeArray("norAttr");

    m_vbo.release();
}

//getters
GLfloat Cylinder::radius()
{
    // on renvoie ce qu'on veut, ça servira pour les placement dans GLArea::paintGL.
    return r_cyl;
}

GLfloat Cylinder::thickness()
{
    return ep_cyl;
}


