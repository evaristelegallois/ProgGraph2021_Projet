// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#include "polyedra.h"
#include <QDebug>
#include <QMatrix4x4>
#include <QVector3D>
#include <QColor>
#include <QImage>
#include <QOpenGLTexture>
#include <math.h>


// Cette classe doit être instanciée lorsqu'il y a un contexte GL courant,
// par exemple dans GLArea::makeGLObject.

Polyedra::Polyedra(GLfloat ep_m, GLfloat width, GLfloat height, GLfloat height_t,
                   GLfloat coul_r, GLfloat coul_v, GLfloat coul_b, bool isPedal) :
    ep_m(ep_m), width(width), height(height), height_t(height_t),
    coul_r(coul_r), coul_v(coul_v), coul_b(coul_b), isPedal(isPedal)
{
    //initialisation du VBO
    m_vbo.create();
    m_vbo.bind();
    QVector<GLfloat> vertData;
    buildVertData(vertData);
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));

    //récupération des images et chargement dans des textures
    for (int i = 0; i < 2; i++) {
        QString nom = QString(":/texture%1.png").arg(i+1);
        QImage image(nom);
        if (image.isNull())
             qDebug() << "load image" << nom << "failed";
        //else qDebug() << "load image" << nom << image.size();
        m_textures[i] = new QOpenGLTexture(image);
    }

    m_vbo.release();
}

Polyedra::~Polyedra()
{
    m_vbo.destroy();
}

//création d'un polyèdre; composé d'un pavé flanqué de deux polyèdres de type trapézoïdes
//height_t = hauteur des trapèzes
//width = longueur du rectangle
//height = largeur du rectangle
void Polyedra::createPolyedra(QVector<GLfloat> &data, GLint sign)
{
        //vertices
        QVector<QVector3D> vs = {

            QVector3D( 0, 0, sign*ep_m/2), //A
            QVector3D(width, 0,  sign*ep_m/2), //B
            QVector3D(width*(1+0.25),height*(0.25), sign*ep_m/2), //C
            QVector3D(width*(1+0.25),height*(0.75), sign*ep_m/2), //D
            QVector3D(width, height,  sign*ep_m/2), //E
            QVector3D(0, height, sign*ep_m/2), //F
            QVector3D(-width*0.25,height*(0.75), sign*ep_m/2), //G
            QVector3D(-width*0.25,height*(0.25), sign*ep_m/2) //H
        };

                       // A  B  C  D  E  F  G  H
        int ind_ver[] = { 0, 1, 2, 3, 4, 5, 6, 7 };

        QColor color = QColor::fromRgbF(coul_r, coul_v, coul_b);

        //un vecteur normal par quadrilatère suffit (car ce sont des rectangles)
        //idem pour les trapèzes car ils sont isocèles
        QVector3D vAB = vs[1]-vs[0],
                  vAF = vs[5]-vs[0],

                  vGA = vs[0]-vs[6],
                  vGF = vs[5]-vs[6],

                  vDE = vs[4]-vs[3],
                  vDB = vs[1]-vs[3];

        // normal() calcule la normale normalisée
        //on a un vecteur normal par triangle donc 2 fois moins de normales
        QVector3D nAFB = QVector3D::normal(vAB, vAF);
        QVector3D nGFA = QVector3D::normal(vGA, vGF);
        QVector3D nDBE = QVector3D::normal(vDE, vDB);

        QVector<QVector3D> normals;
        QVector<int> ind_nor;

        normals = { nAFB, nGFA, nDBE };
        ind_nor = { 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
                    2, 2, 2, 2, 2, 2 };

        //si on instancie une pédale, il faut charger les textures dans
        //le VBO à la place des couleurs; non terminé, il faut normaliser
        //les coordonnées à l'aide du point le plus éloigné de l'origine
        //commes les textures ont des coordonées 2D, pour ne pas modifier
        //l'implémentation du dessin des polyèdres, on charge une troisième
        //coordonnée valant 0
        if (isPedal){
            //triangle = 3 sommets
            for (int i = 0; i < 8; ++i) {
                // coordonnées sommets
                data.append(vs[ind_ver[i]].x());
                data.append(vs[ind_ver[i]].y());
                data.append(vs[ind_ver[i]].z());
                // couleurs sommets
                data.append(vs[ind_ver[i]].x());
                data.append(vs[ind_ver[i]].y());
                data.append(0);
                // normales sommets
                data.append(normals[ind_nor[i]].x());
                data.append(normals[ind_nor[i]].y());
                data.append(normals[ind_nor[i]].z());
            }
        } else {
            //triangle = 3 sommets
            for (int i = 0; i < 8; ++i) {
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
}

//création des facettes (épaisseur du polyèdre)
void Polyedra::createQuad(QVector<GLfloat> &data)
{
    QVector<QVector3D> vs = {
        //facettes
        QVector3D( 0, 0, ep_m/2), //A
        QVector3D(width, 0, ep_m/2), //B
        QVector3D(width, 0, -ep_m/2), //B'
        QVector3D( 0, 0, -ep_m/2), //A'

        QVector3D( 0, 0, -ep_m/2), //A'
        QVector3D(-width*0.25,height*(0.25), -ep_m/2), //H'
        QVector3D(-width*0.25,height*(0.25), ep_m/2), //H
        QVector3D( 0, 0, ep_m/2), //A

        QVector3D(-width*0.25,height*(0.75), ep_m/2), //G
        QVector3D(0, height, ep_m/2), //F
        QVector3D(0, height, -ep_m/2), //F'
        QVector3D(-width*0.25,height*(0.75), -ep_m/2), //G'

        QVector3D(-width*0.25,height*(0.75), -ep_m/2), //G'
        QVector3D(-width*0.25,height*(0.25), -ep_m/2), //H'
        QVector3D(-width*0.25,height*(0.25), ep_m/2), //H
        QVector3D(-width*0.25,height*(0.75), ep_m/2), //G

        QVector3D(width, height,  ep_m/2), //E
        QVector3D(0, height, ep_m/2), //F
        QVector3D(0, height, -ep_m/2), //F'
        QVector3D(width, height,  -ep_m/2), //E'

        QVector3D(width, height,  -ep_m/2), //E'
        QVector3D(width*(1+0.25),height*(0.75), -ep_m/2), //D'
        QVector3D(width*(1+0.25),height*(0.75), ep_m/2), //D
        QVector3D(width, height,  ep_m/2), //E

        QVector3D(width*(1+0.25),height*(0.25), ep_m/2), //C
        QVector3D(width, 0, ep_m/2), //B
        QVector3D(width, 0, -ep_m/2), //B'
        QVector3D(width*(1+0.25),height*(0.25), -ep_m/2), //C'

        QVector3D(width*(1+0.25),height*(0.25), -ep_m/2), //C'
        QVector3D(width*(1+0.25),height*(0.75), -ep_m/2), //D'
        QVector3D(width*(1+0.25),height*(0.75), ep_m/2), //D
        QVector3D(width*(1+0.25),height*(0.25), ep_m/2), //C

    };

                      // A  B  B' A' A' H' H  A  G  F  F' G' G'
                     // H' H  G  E  F  F' E' E' D' D
                    // E  C  B  B' C' C' D' D  C
    int ind_ver[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                      13, 14, 15, 16, 17, 18, 19, 20, 21, 22,
                      23, 24, 25, 26, 27, 28, 29, 30, 31 };

    QColor color = QColor::fromRgbF(0.8*coul_r, 0.8*coul_v, 0.8*coul_b);

    //un vecteur normal par quadrilatère suffit (car ce sont des rectangles)
    QVector3D vAB = vs[1]-vs[0],
              vAAp = vs[3]-vs[0],

              vHA = vs[0]-vs[6],
              vHHp = vs[5]-vs[6],

              vGH = vs[6]-vs[8],
              vGGp = vs[11]-vs[8],

              vFG = vs[8]-vs[9],
              vFFp = vs[10]-vs[9],

              vEF = vs[17]-vs[16],
              vEEp = vs[19]-vs[16],

              vDE = vs[16]-vs[22],
              vDDp = vs[21]-vs[22],

              vCD = vs[22]-vs[24],
              vCCp = vs[28]-vs[24],

              vBC = vs[24]-vs[1],
              vBBp = vs[2]-vs[1];

    // normal() calcule la normale normalisée
    //on a un vecteur normal par triangle donc 2 fois moins de normales
    QVector3D nABAp = QVector3D::normal(vAB, vAAp);
    QVector3D nHAHp = QVector3D::normal(vHA, vHHp);
    QVector3D nGHGp = QVector3D::normal(vGH, vGGp);
    QVector3D nFGFp = QVector3D::normal(vFG, vFFp);
    QVector3D nEFEp = QVector3D::normal(vEF, vEEp);
    QVector3D nDEDp = QVector3D::normal(vDE, vDDp);
    QVector3D nCDCp = QVector3D::normal(vCD, vCCp);
    QVector3D nBCBp = QVector3D::normal(vBC, vBBp);

    QVector<QVector3D> normals;
    QVector<int> ind_nor;

    // - car les vecteurs normaux ont été générés à partir de vecteurs
    // orientés dans le sens indirect
    normals = { -nABAp, -nHAHp, -nGHGp, -nFGFp,
                -nEFEp, -nDEDp, -nCDCp, -nBCBp };
    ind_nor = { 0, 0, 0, 0, 1, 1, 1, 1,
                2, 2, 2, 2, 3, 3, 3, 3,
                4, 4, 4, 4, 5, 5, 5, 5,
                6, 6, 6, 6, 7, 7, 7, 7 };

    //si on instancie une pédale, il faut charger les textures dans
    //le VBO à la place des couleurs; non terminé, il faut normaliser
    //les coordonnées à l'aide du point le plus éloigné de l'origine
    //commes les textures ont des coordonées 2D, pour ne pas modifier
    //l'implémentation du dessin des polyèdres, on charge une troisième
    //coordonnée valant 0
    if (isPedal){
        //triangle = 3 sommets
        for (int i = 0; i < 32; ++i) {
            // coordonnées sommets
            data.append(vs[ind_ver[i]].x());
            data.append(vs[ind_ver[i]].y());
            data.append(vs[ind_ver[i]].z());
            // textures sommets
            data.append(vs[ind_ver[i]].x());
            data.append(vs[ind_ver[i]].y());
            data.append(0);
            // normales sommets
            data.append(normals[ind_nor[i]].x());
            data.append(normals[ind_nor[i]].y());
            data.append(normals[ind_nor[i]].z());
        }
    } else {
        //triangle = 3 sommets
        for (int i = 0; i < 32; ++i) {
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

}

void Polyedra::buildVertData(QVector<GLfloat> &data)
{
        //on dessine d'abord les deux faces
        createPolyedra(data, 1);
        createPolyedra(data, -1);

        //puis les facettes
        createQuad(data);
}

void Polyedra::draw(QOpenGLShaderProgram *program, QOpenGLFunctions *glFuncs)
{
    m_vbo.bind();

    //si on instancie une pédale, il faut charger les textures dans
    //le VBO à la place des couleurs
    if (isPedal){

        program->setAttributeBuffer("posAttr",
            GL_FLOAT, 0 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
        program->setAttributeBuffer("texAttr",
            GL_FLOAT, 3 * sizeof(GLfloat), 3, 6 * sizeof(GLfloat));
        program->setAttributeBuffer("norAttr",
            GL_FLOAT, 5 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));

        program->enableAttributeArray("posAttr");
        program->enableAttributeArray("texAttr");
        program->enableAttributeArray("norAttr");

        m_textures[0]->bind();
        for (int i = 0; i<6; i=i+1){
            glFuncs->glDrawArrays(GL_POLYGON, i*8, 8); //polygone à 8 sommets
        }
        m_textures[0]->release();

        program->disableAttributeArray("posAttr");
        program->disableAttributeArray("texAttr");
        program->disableAttributeArray("norAttr");

    } else {

        program->setAttributeBuffer("posAttr",
            GL_FLOAT, 0 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
        program->setAttributeBuffer("colAttr",
            GL_FLOAT, 3 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));
        program->setAttributeBuffer("norAttr",
            GL_FLOAT, 6 * sizeof(GLfloat), 3, 9 * sizeof(GLfloat));

        program->enableAttributeArray("posAttr");
        program->enableAttributeArray("colAttr");
        program->enableAttributeArray("norAttr");

        // Pour des questions de portabilité, hors de la classe GLArea, tous les appels
        // aux fonctions glBidule doivent être préfixés par glFuncs->.
        //i<6 car 8*2 sommets pour les 2 faces, et 8*4 sommets pour les facettes
        for (int i = 0; i<6; i=i+1){
            glFuncs->glDrawArrays(GL_POLYGON, i*8, 8); //polygone à 8 sommets
        }

        program->disableAttributeArray("posAttr");
        program->disableAttributeArray("colAttr");
        program->disableAttributeArray("norAttr");

    }
    m_vbo.release();
}

//getters
GLfloat Polyedra::widthQuad()
{
    return width;
}

GLfloat Polyedra::heightTrapezoid()
{
    return height_t;
}

GLfloat Polyedra::heightQuad()
{
    return height;
}
