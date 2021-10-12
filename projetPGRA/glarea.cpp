// PROJET PGRA - 04/02/2021 - PAREL Gabriel & FARAJI Mahya
// modifié depuis pro-10 kite CC-BY Edouard.Thiel@univ-amu.fr - 06/02/2021

#include "glarea.h"
#include <QDebug>
#include <QSurfaceFormat>
#include <QMatrix4x4>
#include <QVector3D>
#include <math.h>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

static const QString vertexShaderFile   = ":/vertex.glsl";
static const QString fragmentShaderFile = ":/fragment.glsl";


GLArea::GLArea(QWidget *parent) :
    QOpenGLWidget(parent)
{
    qDebug() << "init GLArea" ;

    QSurfaceFormat sf;
    sf.setDepthBufferSize(24);
    sf.setSamples(16);  // suréchantillonnage pour l'antialiasing
    setFormat(sf);
    qDebug() << "Depth is"<< format().depthBufferSize();

    setEnabled(true);  // événements clavier et souris
    setFocusPolicy(Qt::StrongFocus); // accepte focus
    setFocus();                      // donne le focus

    // slider
    connect (this, SIGNAL(radiusChanged(double)), this, SLOT(setRadius(double)));

    // timer animation
    m_timer = new QTimer(this);
    m_timer->setInterval(50);  // msec
    connect (m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    m_timer->start();
}

GLArea::~GLArea()
{
    qDebug() << "destroy GLArea";

    delete m_timer;

    // Contrairement aux méthodes virtuelles initializeGL, resizeGL et repaintGL,
    // dans le destructeur le contexte GL n'est pas automatiquement rendu courant.
    makeCurrent();
    tearGLObjects();
    doneCurrent();
}

void GLArea::initializeGL()
{
    qDebug() << __FUNCTION__ ;

    // Initialise le wrapper des fonctions OpenGL pour des questions de portabilité.
    // Les fontions glBidule pourront être directement appelées dans cette classe
    // car elle hérite de QOpenGLFunctions. Par contre pour les autres classes qui
    // font du OpenGL, il faudra appeler les fonctions glBidule en faisant :
    //     glFuncs->glBidule(...);
    // en leur transmettant glFuncs, récupéré par (cf paintGL) :
    //     QOpenGLFunctions *glFuncs = context()->functions();
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    makeGLObjects();

    // shaders
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderFile);  // compile
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderFile);
    if (! m_program->link()) {  // édition de lien des shaders dans le shader program
        qWarning("Failed to compile and link shader program:");
        qWarning() << m_program->log();
    }

    m_program->setUniformValue("texture", 0);
    qDebug() << "Keyboard shortcuts:";
    qDebug() << "d start  s stop  a slowDown  A/t speedUp  space turnAround";
}

//instanciation des objets de la scène
void GLArea::makeGLObjects()
{
    //pignon et plateau avant
    GLfloat ep_roue=0.2;
    GLfloat r_roue=0.4;
    GLint nb_dents=10;
    GLfloat h_dent = 0.15;
    cogwheel1 = new Cogwheel(ep_roue, r_roue, h_dent, nb_dents, 0.0, 1.0, 0.0);
    cogwheel2 = new Cogwheel(ep_roue, 2*r_roue, h_dent, 2*nb_dents, 0.0, 1.0, 0.0);

    //axes, écrous, et manivelles
    GLfloat ep_cyl=0.5;
    GLfloat r_cyl=0.05;
    GLint nb_fac=20;
    axis_cog1 = new Cylinder(ep_cyl,cogwheel1->radius()/3,nb_fac,1.0,0.0,0.0, true);
    axis_cog2 = new Cylinder(2*ep_cyl,cogwheel2->radius()/5,nb_fac,1.0,0.0,0.0, true);
    nut1_cog2 = new Cylinder(ep_cyl/2,cogwheel2->radius()/3,nb_fac,0.0,0.0,1.0, true);
    nut2_cog2 = new Cylinder(ep_cyl/2,cogwheel2->radius()/3,nb_fac,0.0,0.0,1.0, true);
    axis_RPedal = new Cylinder(ep_cyl,cogwheel2->radius()/10,nb_fac,1.0,0.0,0.0, true);
    axis_LPedal = new Cylinder(ep_cyl,cogwheel2->radius()/10,nb_fac,1.0,0.0,0.0, true);
    left_crank = new Cylinder(ep_cyl*2,cogwheel2->radius()/6,nb_fac,0.0,0.0,1.0, true);
    right_crank = new Cylinder(ep_cyl*2,cogwheel2->radius()/6,nb_fac,0.0,0.0,1.0, true);
    nut_rCrank = new Cylinder(ep_cyl*1.5,cogwheel2->radius()/5,nb_fac,0.0,0.0,1.0, true);
    nut_lCrank = new Cylinder(ep_cyl*1.5,cogwheel2->radius()/5,nb_fac,0.0,0.0,1.0, true);

    //maillons et pédales
    GLfloat ep_m = 0.05;
    //calcul de la longueur des maillons
    GLfloat width = 1.5*tan((M_PI/cogwheel2->nbCogs()))*(cogwheel2->radius());
    GLfloat width_pedal = 0.2;
    GLfloat height = 0.15;
    GLfloat height_t = 0.05;
    link1 = new Link(ep_m, width*1.5, height*1.2, height_t, 3*cogwheel1->thickness()/2+0.02,
                    r_cyl*1.2, nb_fac, 0.0, 0.0, 1.0, true); //maillon externe
    link2 = new Link(ep_m*0.8, width*1.5, height*1, height_t, 3*cogwheel1->thickness()/2+0.02,
                     r_cyl*1.2, nb_fac, 0.0, 0.0, 1.0, false); //maillon interne
    
    //les textures ne s'affichent pour le moment car nous n'arrivons pas à afficher conjointement
    //couleurs et textures; passer à true le cas échéant
    rPedal = new Polyedra(ep_m*3, width_pedal*3, height*3, height_t, 0.0, 0.0, 1.0, false);
    lPedal = new Polyedra(ep_m*3, width_pedal*3, height*3, height_t, 0.0, 0.0, 1.0, false);
}

//destruction des objets de la scène
void GLArea::tearGLObjects()
{
    delete axis_cog1;
    delete axis_cog2;
    delete cogwheel1;
    delete cogwheel2;
    delete nut1_cog2;
    delete nut2_cog2;
    delete axis_RPedal;
    delete axis_LPedal;
    delete left_crank;
    delete right_crank;
    delete nut_lCrank;
    delete nut_rCrank;
    delete link1;
    delete link2;
    delete rPedal;
    delete lPedal;
}

void GLArea::resizeGL(int w, int h)
{
    qDebug() << __FUNCTION__ << w << h;

    // C'est fait par défaut
    glViewport(0, 0, w, h);

    m_ratio = (double) w / h;
}

// Fonctions de placement et rotation des maillons
//trajectoire des maillons, décomposée par coordonnées x et y
//la trajectoire n'est pas tout à fait correcte car les maillons ne sont pas centrés
//sur les dents des roues, mais nous ne sommes pas parvenus à faire mieux
GLfloat GLArea::xChainTrajectory(GLfloat t, GLfloat x_o, GLfloat x_O, GLfloat radius1,
                                 GLfloat radius2)
{
    while (t>3.5*M_PI) t=t-3.5*M_PI; //modulo 3.5pi
        //1 demi tour, 1 demi tour qui va deux fois + vite, et 2 unités multiples de
        //pi par praticité : pi + pi/2 + 2*pi

    //rotation autour du plateau
    if (t < 1.1*M_PI and t >= 0) return (radius2-0.07)*cos(t+M_PI/2)+x_O;
    //translation du plateau vers le pignon
    else if (t <= 2.1*M_PI and t >= 1.1*M_PI) return radius2*cos(M_PI/2) + x_O + ((t/M_PI) - 1.1) //(t-pi)/pi
            *((radius1+0.1)*cos(M_PI/2) + x_o - radius2*cos(M_PI/2) - x_O); //0.1 pour corriger le placement
    //rotation autour du pignon
    else if (t < 2.4*M_PI and t > 2.1*M_PI) return (radius1+0.1)*cos(1.95*t-M_PI/2)+x_o;
    //translation du pignon vers le plateau
    else if (t <= 3.4*M_PI and t >= 2.4*M_PI) return (radius1)*cos(M_PI/2-0.1*M_PI) + x_o + ((t/M_PI) - 2.4)
            *(radius2*cos(M_PI/2) + x_O - (radius1)*cos(M_PI/2-0.1*M_PI) - x_o); //(t-2.5*pi)/pi
    //transition début et fin de la boucle
    else if (t < 3.5*M_PI and t >= 3.4) return (radius2-0.07)*cos(t+M_PI/2-3.5*M_PI)+x_O;

    return 0;
}

GLfloat GLArea::yChainTrajectory(GLfloat t, GLfloat y_o, GLfloat y_O, GLfloat radius1,
                                 GLfloat radius2)
{
    while (t>3.5*M_PI) t=t-3.5*M_PI; //modulo 3.5pi

    if (t < 1.1*M_PI and t >= 0) return (radius2-0.07)*sin(t+M_PI/2)+y_O;
    //0.08 pour corriger le placement
    else if (t <= 2.1*M_PI and t >= 1.1*M_PI) return (radius2+0.1)*sin(-M_PI/2) + y_O-0.08 +
            ((t/M_PI) - 1.1)*((radius1+0.1)*sin(-M_PI/2) + y_o - (radius2+0.1)*sin(-M_PI/2) - y_O+0.08);
    else if (t < 2.4*M_PI and t > 2.1*M_PI) return (radius1+0.1)*sin(1.95*t-M_PI/2)+y_o;
    //0.17 pour corriger le placement
    else if (t <= 3.4*M_PI and t >= 2.4*M_PI) return radius1*sin(M_PI/2-0.1*M_PI) + y_o -0.17 +
            ((t/M_PI) - 2.4) *(radius2*sin(M_PI/2) + y_O - radius1*sin(M_PI/2-0.1*M_PI) - y_o+0.17);
    else if (t < 3.5*M_PI and t >= 3.4) return (radius2-0.07)*sin(t+M_PI/2-3.5*M_PI)+y_O;
    return 0;
}

//rotation de chaque maillon pour les orienter, dépend de l'axe de rotation du pignon/plateau
GLfloat GLArea::chainRotation(GLfloat t, GLfloat radius1, GLfloat radius2,
                              GLfloat distCogCenter)
{
    while (t>3.5*M_PI) t=t-3.5*M_PI; //modulo 3.5pi

    //angle/2 pour cog2 car tourne 2 fois moins vite
    if (t < 1.1*M_PI and t >= 0) return t;
    else if (t < 2.1*M_PI and t > 1.1*M_PI) return atan(-(radius2-radius1+0.08)/distCogCenter);
    else if (t < 2.4*M_PI and t > 2.1*M_PI) return 2*(t-2*M_PI); //offset à 2*pi
    else if (t < 3.5*M_PI and t > 2.4*M_PI) return atan((radius2-radius1+0.17)/distCogCenter);

    return 0;
}

void GLArea::paintGL()
{
    //qDebug() << __FUNCTION__ ;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    QOpenGLFunctions *glFuncs = context()->functions();  // cf initializeGL

    m_program->bind(); // active le shader program

    // Projection
    QMatrix4x4 proj_mat;
    GLfloat hr = m_radius, wr = hr * m_ratio;
    proj_mat.frustum(-wr, wr, -hr, hr, 1.0, 5.0);
    m_program->setUniformValue("projMatrix", proj_mat);

    // Caméra
    QMatrix4x4 cam_mat;
    cam_mat.translate(0, 0, -3.0);

    // Orientation de la scène
    QMatrix4x4 world_mat;
    world_mat.rotate(m_angle, 0, 1, 0);

    // Position d'un objet par rapport à la scène ; on réinitialisera chaque fois
    QMatrix4x4 shape_mat;

    // Placement des objets sur la scène

    //l'éclairage est assuré d'une part par une ligne de code dans le fragment
    //shader (lumière ambiante) et d'autre part par le calcul des normales dans chaque
    //classe correspondant à un objet (lumière diffuse)


    shape_mat = world_mat;
    shape_mat.translate(10, 0, 0);
    setTransforms(cam_mat, shape_mat);


    //centre du pignon
    GLfloat x_o = 0.2+2*cogwheel1->radius()+0.45;
    GLfloat y_o = 0;
    //centre du plateau
    GLfloat x_O = -0.2-cogwheel2->radius()+0.45;
    GLfloat y_O = 0;


    // Dessin de l'axe du pignon
    shape_mat = world_mat;
    shape_mat.translate(x_o, y_o, 0);
    setTransforms(cam_mat, shape_mat);
    axis_cog1->draw(m_program, glFuncs);

    // Dessin de l'axe du plateau avant
    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, 0);
    setTransforms(cam_mat, shape_mat);
    axis_cog2->draw(m_program, glFuncs);


    // Dessin des écrous de l'axe du plateau
    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, -0.25);
    setTransforms(cam_mat, shape_mat);
    nut1_cog2->draw(m_program, glFuncs);

    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, 0.25);
    setTransforms(cam_mat, shape_mat);
    nut2_cog2->draw(m_program, glFuncs);


    // on applique des translations dépendant de m_anim aux
    // objets mobiles, ce qui permet l'animation; comme m_anim
    // n'est pas compris entre 0 et 2pi, on a m_anim*2pi

    // Dessin des écrous des manivelles
    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, -0.25*2);
    shape_mat.translate(cogwheel2->radius()*cos(m_anim*2*M_PI+M_PI),
                        cogwheel2->radius()*sin(m_anim*2*M_PI+M_PI), 0);
                        // + pi car sens opposé
    setTransforms(cam_mat, shape_mat);
    nut_rCrank->draw(m_program, glFuncs);

    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, 0.25*2);
    shape_mat.translate(cogwheel2->radius()*cos(m_anim*2*M_PI),
                        cogwheel2->radius()*sin(m_anim*2*M_PI), 0);
    setTransforms(cam_mat, shape_mat);
    nut_lCrank->draw(m_program, glFuncs);


    // Dessin des pédales
    shape_mat = world_mat;
    shape_mat.translate(x_O-rPedal->widthQuad()/2, y_O, -0.25*3.4);
    shape_mat.translate(cogwheel2->radius()*cos(m_anim*2*M_PI+M_PI),
                        cogwheel2->radius()*sin(m_anim*2*M_PI+M_PI), 0);
    shape_mat.rotate(90,1,0,0);
    setTransforms(cam_mat, shape_mat);
    rPedal->draw(m_program, glFuncs);

    shape_mat = world_mat;
    shape_mat.translate(x_O-lPedal->widthQuad()/2, y_O, 0.2*2);
    shape_mat.translate(cogwheel2->radius()*cos(m_anim*2*M_PI),
                        cogwheel2->radius()*sin(m_anim*2*M_PI), 0);
    shape_mat.rotate(90,1,0,0);
    setTransforms(cam_mat, shape_mat);
    lPedal->draw(m_program, glFuncs);


    // Dessin des manivelles
    shape_mat = world_mat;
    shape_mat.translate(x_O+cos(m_anim*2*M_PI+M_PI)/2, y_O+sin(m_anim*2*M_PI+
                                                               M_PI)/2, -0.25);
    shape_mat.rotate(m_anim*2*180, 0, 0, 1); //180/pi car degrés, reste 180
    shape_mat.rotate(90,0,1,0);
    setTransforms(cam_mat, shape_mat);
    right_crank->draw(m_program, glFuncs);

    shape_mat = world_mat;
    shape_mat.translate(x_O+cos(m_anim*2*M_PI)/2, y_O+sin(m_anim*2*M_PI)/2, 0.25);
    shape_mat.rotate(m_anim*2*180, 0, 0, 1);
    shape_mat.rotate(90,0,1,0);
    setTransforms(cam_mat, shape_mat);
    left_crank->draw(m_program, glFuncs);


    // Dessin de l'axe des pédales
    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, -0.25*2.6);
    shape_mat.translate(cogwheel2->radius()*cos(m_anim*2*M_PI+M_PI),
                        cogwheel2->radius()*sin(m_anim*2*M_PI+M_PI), 0);
    setTransforms(cam_mat, shape_mat);
    axis_RPedal->draw(m_program, glFuncs);

    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, 0.25*2.6);
    shape_mat.translate(cogwheel2->radius()*cos(m_anim*2*M_PI),
                        cogwheel2->radius()*sin(m_anim*2*M_PI), 0);
    setTransforms(cam_mat, shape_mat);
    axis_LPedal->draw(m_program, glFuncs);


    // Dessin du pignon
    shape_mat = world_mat;
    shape_mat.translate(x_o, y_o, 0);
    shape_mat.rotate((m_anim+0.5)*360*2, 0, 0, 1); //180*2, tourne 2 fois + vite
    setTransforms(cam_mat, shape_mat);
    cogwheel1->draw(m_program, glFuncs);

    // Dessin du plateau avant
    shape_mat = world_mat;
    shape_mat.translate(x_O, y_O, 0);
    shape_mat.rotate((m_anim+0.5)*180*2, 0, 0, 1);
    setTransforms(cam_mat, shape_mat);
    cogwheel2->draw(m_program, glFuncs);


    // Dessin des maillons
    GLfloat Oo = x_O-x_o; //distance entre les deux centres des roues

    //on place les maillons sur la trajectoire voulue à l'aide des fonctions associées
    for (int j = 0; j<28; j = j+1)
    {
        shape_mat = world_mat;
        shape_mat.translate(xChainTrajectory(m_anim*2*M_PI+j*M_PI/8,x_o,x_O,
                                             cogwheel1->radius(),cogwheel2->radius()),
                        yChainTrajectory(m_anim*2*M_PI+j*M_PI/8,y_o,y_O,
                                         cogwheel1->radius(),cogwheel2->radius()),
                        0); //j*M_PI/8 permet de déterminer l'espacement des maillons

        shape_mat.rotate(chainRotation(m_anim*2*M_PI+j*M_PI/8,cogwheel1->radius(),
                                       cogwheel2->radius(),Oo)*180/M_PI, 0,0,1);

        //un maillon sur deux est interne, un maillon sur deux est externe
        //j pair -> maillon externe, j impair -> maillon interne
        if (j%2 == 0)
        {
            //on ne réinitialise pas la matrice ici car cela annulerait les modifications
            //faites plus haut; il faut adapter les transformations suivantes en fonction
            //placement des maillons externes, partie polyédrale
            shape_mat.translate(0,0,cogwheel1->thickness()/2);
            setTransforms(cam_mat, shape_mat);
            link1->getPolyedra(1)->draw(m_program, glFuncs);

            shape_mat.translate(0,0,-cogwheel1->thickness());
            setTransforms(cam_mat, shape_mat);
            link1->getPolyedra(2)->draw(m_program, glFuncs);


            //placement des maillons, partie cylindrique
            shape_mat.translate(link1->getPolyedra(1)->widthQuad(),
                                link1->getPolyedra(1)->heightQuad()/2,cogwheel1->thickness()/2);
            setTransforms(cam_mat, shape_mat);
            link1->getCylinder(1)->draw(m_program, glFuncs);

            shape_mat.translate(-link1->getPolyedra(1)->widthQuad(),0,0);
            setTransforms(cam_mat, shape_mat);
            link1->getCylinder(2)->draw(m_program, glFuncs);

        } else {
            //maillons internes
            shape_mat.translate(0,0,cogwheel1->thickness()/2);
            setTransforms(cam_mat, shape_mat);
            link2->getPolyedra(1)->draw(m_program, glFuncs);

            shape_mat.translate(0,0,-cogwheel1->thickness());
            setTransforms(cam_mat, shape_mat);
            link2->getPolyedra(2)->draw(m_program, glFuncs);
        }

    }

    m_program->release();
}

void GLArea::setTransforms(QMatrix4x4 &cam_mat, QMatrix4x4 &shape_mat)
{
    QMatrix4x4 mv_mat = cam_mat*shape_mat;
    m_program->setUniformValue("mvMatrix", mv_mat);

    QMatrix3x3 nor_mat = shape_mat.normalMatrix();
    m_program->setUniformValue("norMatrix", nor_mat);
}

void GLArea::keyPressEvent(QKeyEvent *ev)
{
    //qDebug() << __FUNCTION__ << ev->text();

    switch(ev->key()) {
        case Qt::Key_Space :
            m_angle += 2;
            if (m_angle >= 360) m_angle -= 360;
            update();
            break;
        case Qt::Key_F :
            m_angle -= 2;
            if (m_angle >= 360) m_angle -= 360;
            update();
            break;
        //raccourcis clavier permettant d'agir sur l'animation
        case Qt::Key_D :
             start();
          break;
        case Qt::Key_S :
          if (m_timer->isActive())
                 stop();
          break;
        case Qt::Key_A :
          if (m_timer->isActive()){
                //not working either...
                if ((ev->key()) == Qt::Key_CapsLock
                        || (ev->key()) == Qt::Key_Shift)
                    speedUp();
                else
                    slowDown();
          }
          break;
       case Qt::Key_T :
        if (m_timer->isActive())
               speedUp();
         break;
       case Qt::Key_R :
         if (ev->text() == "r")
                 setRadius(m_radius-0.05);
         else setRadius(m_radius+0.05);
         break;
       //NOT WORKING EITHER
       case (Qt::Key_Shift + Qt::Key_A):
        if (m_timer->isActive())
               speedUp();
         break;

}
      //NOT WORKING
      if((ev->key()) == Qt::Key_CapsLock && (ev->key()) == Qt::Key_A)
      {
          if (m_timer->isActive())
                 speedUp();
      }
}

void GLArea::keyReleaseEvent(QKeyEvent *ev)
{
    (void) ev;
    //qDebug() << __FUNCTION__ << ev->text();
}

void GLArea::mousePressEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

void GLArea::mouseReleaseEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y() << ev->button();
}

void GLArea::mouseMoveEvent(QMouseEvent *ev)
{
    qDebug() << __FUNCTION__ << ev->x() << ev->y();
}

//permet d'animer les objets de la scène
void GLArea::onTimeout()
{
    //qDebug() << __FUNCTION__ ;
    m_anim += m_step;
    if (m_anim > 1) m_anim = 0;
    update();
}

void GLArea::setRadius(double radius)
{
    qDebug() << __FUNCTION__ << radius << sender();
    if (radius != m_radius && radius > 0.01 && radius <= 10) {
        m_radius = radius;
        qDebug() << "  emit radiusChanged()";
        emit radiusChanged(radius);
        update();
    }
}

//méthodes permettant de modifier l'animation des objets de la scène
//modifient le pas utilisé dans onTimeout()
void GLArea::speedUp()
{
    m_step = m_step*1.5;
}

void GLArea::slowDown()
{
    m_step = m_step*0.5;
}

void GLArea::stop()
{
    m_timer->stop();
}

void GLArea::start()
{
    m_timer->start();
}
