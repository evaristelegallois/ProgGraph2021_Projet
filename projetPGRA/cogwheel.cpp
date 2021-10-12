// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#include "cogwheel.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>
#include <math.h>

// Cette classe doit être instanciée lorsqu'il y a un contexte GL courant,
// par exemple dans GLArea::makeGLObject.

Cogwheel::Cogwheel(GLfloat ep_roue, GLfloat r_roue, GLfloat h_dent, GLint nb_dents,
                   GLfloat coul_r, GLfloat coul_v, GLfloat coul_b) :
    ep_roue(ep_roue), r_roue(r_roue), h_dent(h_dent), nb_dents(nb_dents),
    coul_r(coul_r), coul_v(coul_v), coul_b(coul_b)
{
    //initialisation du VBO
    m_vbo.create();
    m_vbo.bind();
    QVector<GLfloat> vertData;
    buildVertData(vertData);
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
    m_vbo.release();
}


Cogwheel::~Cogwheel()
{
    m_vbo.destroy();
}

void Cogwheel::createPolygon(QVector<GLfloat> &data, GLdouble angle, GLint ind, GLint sign)
{
    GLfloat z = sign*ep_roue/2; //pour placement des deux faces

    QVector<QVector3D> vs = {
        QVector3D( 0, 0, z), //O
        QVector3D((r_roue-h_dent/2)*cos(angle*ind),
        (r_roue-h_dent/2)*sin(angle*ind), z), //B
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  z), //C
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  z), //D
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  z), //E
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+1.0)),
        (r_roue-h_dent/2)*sin(angle*(ind+1.0)),  z) //F
    };

                   // O  B  C  D  E  F
    int ind_ver[] = { 0, 1, 2, 3, 4, 5 };

    QColor color = QColor::fromRgbF(coul_r, coul_v, coul_b);

    //pour calculer le vecteur normal moyen on utilise les vecteurs
    //correspondant aux côtés des "triangles" qui composent le polygone
    //un seul suffit car l'on peut "englober" le polygone dans un triangle
    QVector3D vOB = vs[1]-vs[0],
              vOF = vs[5]-vs[0];

    // normal() calcule la normale normalisée
    QVector3D nOBF = QVector3D::normal(vOB, vOF);

        //pour la face arrière
        if(sign < 0) nOBF = -nOBF;

        //hexagone = 6 sommets
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
            data.append(nOBF.x());
            data.append(nOBF.y());
            data.append(nOBF.z());
        }
        //pas de lissage de phong
}

void Cogwheel::createQuad(QVector<GLfloat> &data, GLdouble angle, GLint ind)
{
    GLfloat z = ep_roue/2; //pour placement en z

    //c'est plus long mais je prefère travailler avec des triangles
    //pour éviter les problèmes de coplanarité que je n'arrive pas à gérer
    //avec des polygones
    QVector<QVector3D> vs = {

        //un quadrilatère = 2 triangles = 6 sommets
        QVector3D((r_roue-h_dent/2)*cos(angle*ind),
        (r_roue-h_dent/2)*sin(angle*ind), z), //B
        QVector3D((r_roue-h_dent/2)*cos(angle*ind),
        (r_roue-h_dent/2)*sin(angle*ind), -z), //B'
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  -z), //C'
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  -z), //C'
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  z), //C
        QVector3D((r_roue-h_dent/2)*cos(angle*ind),
        (r_roue-h_dent/2)*sin(angle*ind), z), //B

        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  z), //C
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  -z), //C'
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  -z), //D'
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  -z), //D'
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  z), //D
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+0.25)),
        (r_roue-h_dent/2)*sin(angle*(ind+0.25)),  z), //C

        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  z), //D
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  -z), //D'
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  -z), //E'
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  -z), //E'
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  z), //E
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.5)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.5)),  z), //D

        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  z), //E
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  -z), //E'
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+1.0)),
        (r_roue-h_dent/2)*sin(angle*(ind+1.0)),  -z), //F'
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+1.0)),
        (r_roue-h_dent/2)*sin(angle*(ind+1.0)),  -z), //F'
        QVector3D((r_roue-h_dent/2)*cos(angle*(ind+1.0)),
        (r_roue-h_dent/2)*sin(angle*(ind+1.0)),  z), //F
        QVector3D((r_roue+h_dent/2)*cos(angle*(ind+0.75)),
        (r_roue+h_dent/2)*sin(angle*(ind+0.75)),  z), //E
    };

                    //B  B' C' C' C  B  C  C' D' D'  D   C   D
                    //D'  E'  E'  E   D   E   E'  F'  F'  F   E
    int ind_ver[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };

    QColor color = QColor::fromRgbF(0.8*coul_r, 0.8*coul_v, 0.8*coul_b);

    //un vecteur normal par quadrilatère suffit (car ce sont des rectangles)
    QVector3D vBCp = vs[2]-vs[0],
              vBBp = vs[1]-vs[0],

              vCDp = vs[8]-vs[4],
              vCCp = vs[2]-vs[4],

              vDEp = vs[14]-vs[10],
              vDDp = vs[8]-vs[10],

              vEFp = vs[20]-vs[16],
              vEEp = vs[14]-vs[16];

     // normal() calcule la normale normalisée
     //on a un vecteur normal par triangle donc 2 fois moins de normales
     QVector3D nBBpCp = QVector3D::normal(vBCp, vBBp);
     QVector3D nCCpDp = QVector3D::normal(vCCp, vCDp);
     QVector3D nDEpDp = QVector3D::normal(vDDp, vDEp);
     QVector3D nEFpEp = QVector3D::normal(vEEp, vEFp);

     QVector<QVector3D> normals;
     QVector<int> ind_nor;

     normals = { nBBpCp, nCCpDp, nDEpDp, nEFpEp };
     ind_nor = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3 };

        //8 triangles = 24 sommets
        for (int i = 0; i < 24; ++i) {
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
        //pas de lissage de phong
}

void Cogwheel::buildVertData(QVector<GLfloat> &data)
{
    GLdouble angle = 360./nb_dents*M_PI/180;

    for(int i=0; i < nb_dents; i++){
        //on dessine d'abord les deux faces
        createPolygon(data, angle, i, 1);
        createPolygon(data, angle, i, -1);

        //puis les facettes
        createQuad(data, angle, i);
    }
}

void Cogwheel::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *glFuncs)
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

    //faces de la roue, 2*nb_dents car 2 faces
    //facettes, 3*nb_dents
    for(int i=0; i < 6*nb_dents; i++){

        // Pour des questions de portabilité, hors de la classe GLArea, tous les appels
        // aux fonctions glBidule doivent être préfixés par glFuncs->.
        glFuncs->glDrawArrays(GL_POLYGON, i*6, 6); //polygones à 6 sommets
        glFuncs->glDrawArrays(GL_TRIANGLES, i/2*24, 24); //6*4 sommets
    }

    program->disableAttributeArray("posAttr");
    program->disableAttributeArray("colAttr");
    program->disableAttributeArray("norAttr");

    m_vbo.release();
}

//getters
GLfloat Cogwheel::radius()
{
    // on renvoie ce qu'on veut, ça servira pour les placement dans GLArea::paintGL.
    return r_roue;
}

GLint Cogwheel::nbCogs()
{
    // on renvoie ce qu'on veut, ça servira pour les placement dans GLArea::paintGL.
    return nb_dents;
}

GLfloat Cogwheel::thickness()
{
    return ep_roue;
}
