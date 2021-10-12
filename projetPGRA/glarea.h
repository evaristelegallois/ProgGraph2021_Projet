// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#ifndef GLAREA_H
#define GLAREA_H

#include <QKeyEvent>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include "kite.h"
#include "cylinder.h"
#include "cogwheel.h"
#include "link.h"
#include "polyedra.h"

class GLArea : public QOpenGLWidget,
               protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GLArea(QWidget *parent = 0);
    ~GLArea();
    void speedUp();
    void slowDown();
    void stop();
    void start();

public slots:
    void setRadius(double radius);

signals:  // On ne les implémente pas, elles seront générées par MOC ;
          // les paramètres seront passés aux slots connectés.
    void radiusChanged(double newRadius);

protected slots:
    void onTimeout();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseReleaseEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    //fonction de rotation et trajectoire de la chaîne
    GLfloat xChainTrajectory(GLfloat t, GLfloat x_o, GLfloat x_O,
                             GLfloat radius1, GLfloat radius2);
    GLfloat yChainTrajectory(GLfloat t, GLfloat y_o, GLfloat y_O,
                             GLfloat radius1, GLfloat radius2);
    GLfloat chainRotation(GLfloat t, GLfloat radius1,
                           GLfloat radius2, GLfloat distCogCenter);
    //création d'un maillon
    void createLink(Link* link, Cogwheel* cogwheel1, QMatrix4x4 world_mat,
                    QMatrix4x4 shape_mat, QMatrix4x4 cam_mat,
                    QOpenGLFunctions* glFuncs, bool external);

private:
    double m_angle = 0;
    QTimer *m_timer = nullptr;
    double m_anim = 0;
    double m_radius = 0.5;
    double m_ratio = 1;
    double m_step = 0.01;

    QOpenGLShaderProgram *m_program;

    void makeGLObjects();
    void tearGLObjects();
    void setTransforms(QMatrix4x4 &cam_mat, QMatrix4x4 &shape_mat);

private:
    //instanciation des objets de la scène
    //axes, écrous et manivelles
    Cylinder *axis_cog1 = nullptr;
    Cylinder *axis_cog2 = nullptr;
    Cylinder *nut1_cog2 = nullptr;
    Cylinder *nut2_cog2 = nullptr;
    Cylinder *axis_RPedal = nullptr;
    Cylinder *axis_LPedal = nullptr;
    Cylinder *left_crank = nullptr;
    Cylinder *right_crank = nullptr;
    Cylinder *nut_rCrank = nullptr;
    Cylinder *nut_lCrank = nullptr;
    //plateau avant et pignon
    Cogwheel *cogwheel1 = nullptr;
    Cogwheel *cogwheel2 = nullptr;
    //maillons int/ext et pédales
    Link *link1 = nullptr;
    Link *link2 = nullptr;
    Polyedra *rPedal = nullptr;
    Polyedra *lPedal = nullptr;
};

#endif // GLAREA_H
