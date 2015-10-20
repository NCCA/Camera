#include <QMouseEvent>
#include <QGuiApplication>
#include <QFont>
#include <QDesktopWidget>
#include <QRect>

#include <QApplication>
#include "NGLScene.h"
#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
const static float INCREMENT=0.01;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
const static float ZOOM=0.1;

NGLScene::NGLScene()
{
  // Now set the initial GLWindow attributes to default values
  // Roate is false
  m_rotate=false;
  // mouse rotation values set to 0
  m_spinXFace=0;
  m_spinYFace=0;
  // now set the inital camera values
  m_cameraIndex=0;
  m_moveMode=MOVEEYE;
  m_drawHelp=true;
  m_fov=65.0;
  m_aspect=1024.0/768.0;
  m_rotation=0;
  startTimer(20);
  m_lights[0]=true;
  m_lights[1]=true;
  m_lights[2]=true;
  setTitle("ngl::Camera and ngl::Transformation demo");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
  delete m_text;
}

void NGLScene::createCameras()
{
  // create a load of cameras
  ngl::Camera cam;
  ngl::Camera Tcam;
  ngl::Camera Scam;
  ngl::Camera Fcam;
  // set the different vectors for the camera positions
  ngl::Vec3 eye(0.0f,5.0f,8.0f);
  ngl::Vec3 look=0.0f;
  ngl::Vec3 up(0,1,0);

  ngl::Vec3 topEye(0.0f,8.0f,0.0f);
  ngl::Vec3 topLook=0.0f;
  ngl::Vec3 topUp(0.0f,0.0f,1.0f);

  ngl::Vec3 sideEye(8.0f,0.0f,0.0f);
  ngl::Vec3 sideLook=0.0f;
  ngl::Vec3 sideUp(0.0f,1.0f,0.0f);

  ngl::Vec3 frontEye(5.0f,2.0f,8.0f);
  ngl::Vec3 frontLook(0.0f,0.0f,0.0f);
  ngl::Vec3 frontUp(0.0f,1.0f,0.0f);
  // finally set the cameras shape and position
  cam.set(eye,look,up);
  cam.setShape(m_fov,m_aspect, 0.2f,150.0f);
  m_cameras.push_back(cam);
  Tcam.set(topEye,topLook,topUp);
  Tcam.setShape(m_fov,m_aspect, 0.5f,150.0f);
  m_cameras.push_back(Tcam);
  Scam.set(sideEye,sideLook,sideUp);
  Scam.setShape(m_fov,m_aspect, 0.5f,150.0f);
  m_cameras.push_back(Scam);
  Fcam.set(frontEye,frontLook,frontUp);
  Fcam.setShape(m_fov,m_aspect, 0.5f,150.0f);
  m_cameras.push_back(Fcam);
}
void NGLScene::resizeGL(int _w, int _h)
{
  m_aspect=(float)_w/_h;
  // now set the camera values
  m_text->setScreenSize(_w,_h);

  float x,y;
  float mw=1490;
  float mh=900;
  x=1.1-float(mw-_w)/mw;
  y=1.1-float(mh-_h)/mh;
  std::cout<<_w<<" "<<_h<<"\n";
  m_text->setTransform(x,y);
  // set the viewport for openGL
  glViewport(0,0,_w*devicePixelRatio() ,_h*devicePixelRatio());
  // now set the camera size values as the screen size has changed
  update();

}


void NGLScene::initializeGL()
{
  ngl::NGLInit::instance();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  createCameras();
  // create an instance of the VBO primitive
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  // create a plane
  prim->createLineGrid("plane",20,20,30);
  prim->createSphere("sphere",1.0,150);

  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  shader->createShaderProgram("Phong");

  shader->attachShader("PhongVertex",ngl::ShaderType::VERTEX);
  shader->attachShader("PhongFragment",ngl::ShaderType::FRAGMENT);
  shader->loadShaderSource("PhongVertex","shaders/PhongVertex.glsl");
  shader->loadShaderSource("PhongFragment","shaders/PhongFragment.glsl");

  shader->compileShader("PhongVertex");
  shader->compileShader("PhongFragment");
  shader->attachShaderToProgram("Phong","PhongVertex");
  shader->attachShaderToProgram("Phong","PhongFragment");



  shader->linkProgramObject("Phong");
  (*shader)["Phong"]->use();

  // now pass the modelView and projection values to the shader
  shader->setShaderParam1i("Normalize",1);

  // now set the material and light values
  ngl::Material m(ngl::COPPER);
  m.loadToShader("material");


  shader->createShaderProgram("Colour");

  shader->attachShader("ColourVertex",ngl::ShaderType::VERTEX);
  shader->attachShader("ColourFragment",ngl::ShaderType::FRAGMENT);
  shader->loadShaderSource("ColourVertex","shaders/Colour.vs");
  shader->loadShaderSource("ColourFragment","shaders/Colour.fs");

  shader->compileShader("ColourVertex");
  shader->compileShader("ColourFragment");
  shader->attachShaderToProgram("Colour","ColourVertex");
  shader->attachShaderToProgram("Colour","ColourFragment");

  shader->bindAttribute("Colour",0,"inVert");
  shader->linkProgramObject("Colour");
  (*shader)["Colour"]->use();
  shader->setShaderParam4f("Colour",1,1,1,1);

  m_text = new  ngl::Text(QFont("Arial",18));
  m_text->setScreenSize(this->size().width(),this->size().height());
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());


}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_transform.getMatrix()*m_mouseGlobalTX;
  MV= M* m_cameras[m_cameraIndex].getViewMatrix();
  MVP=  MV*m_cameras[m_cameraIndex].getProjectionMatrix();
  normalMatrix=MV;
  normalMatrix.inverse();
  shader->setShaderParamFromMat4("MV",MV);
  shader->setShaderParamFromMat4("MVP",MVP);
  shader->setShaderParamFromMat3("normalMatrix",normalMatrix);
  shader->setShaderParamFromMat4("M",M);
  shader->setShaderParam3f("viewerPos",m_cameras[m_cameraIndex].getEye().m_x,m_cameras[m_cameraIndex].getEye().m_y,m_cameras[m_cameraIndex].getEye().m_z);

}

void NGLScene::paintGL()
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // Rotation based on the mouse position for our global
  // transform

  ngl::Transformation trans;
  // Rotation based on the mouse position for our global
  // transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_spinXFace);
  rotY.rotateY(m_spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // now set this value in the shader for the current Camera, we do all 3 elements as they
  // can all change per frame
  shader->use("Phong");
  ngl::Mat4 lt=m_cameras[m_cameraIndex].getViewMatrix();
  lt.transpose();

  ngl::Light L1(ngl::Vec3(-2,0,0),ngl::Colour(0.6,0.6,0.6,1),ngl::DIRECTIONALLIGHT);
  ngl::Light L2(ngl::Vec3(2,0,0),ngl::Colour(0.6,0.6,0.6,1),ngl::DIRECTIONALLIGHT);
  ngl::Light L3(ngl::Vec3(0,2,0),ngl::Colour(0.6 ,0.6,0.6,1),ngl::POINTLIGHT);
  L1.setSpecColour(ngl::Colour(1,1,1));
  L2.setSpecColour(ngl::Colour(1,1,1));
  L3.setSpecColour(ngl::Colour(1,1,1));
  L1.disable();
  L2.disable();
  L3.disable();
  if( m_lights[0] == true)
  {
    L1.setTransform(lt);
    L1.enable();
  }
  L1.loadToShader("light[0]");

  if( m_lights[1] == true)
  {
    L2.setTransform(lt);
    L2.enable();
  }
  L2.loadToShader("light[1]");

  if( m_lights[2] == true)
  {
    L3.setTransform(lt);

    L3.enable();
  }
  L3.loadToShader("light[2]");

  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  m_transform.setRotation(0,m_rotation,0);
  m_transform.setPosition(0.0f,0.0f,0.0f);

  loadMatricesToShader();
  prim->draw("teapot");

  // activate the shader
  shader->use("Phong");
  // push a transform
    ngl::Mat4 tp;
  m_transform.reset();
  {

    // translate into position
    m_transform.setPosition(-1.8,0.1,0);
    // rotate object
    m_transform.setRotation(m_rotation,m_rotation,m_rotation);
    // pass values to the shader
    loadMatricesToShader();
    // draw
    tp=m_transform.getMatrix();

    prim->draw("cube");
  }
  // now we draw the football
  m_transform.reset();
  {
    // set the transforms for pos and scale
    m_transform.setPosition(1.5,0,0);
    m_transform.setScale(0.5,0.5,0.5);
    m_transform.setRotation(m_rotation,m_rotation,m_rotation);
    loadMatricesToShader();
    // draw the football
    prim->draw("football");
  }
  // now we are going to draw the help

  // now we are going to draw the grid in wireframe mode
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  // load the colour  shader and set the values,
  shader->use("Colour");
  // now push the tx stack it helps to use { } to block off the transforms
  m_transform.reset();
  {
    // translate the grid down by -0.5 so it is at the base level of the objects
    m_transform.setPosition(0,-0.5,0);
    ngl::Mat4 MVP;
    MVP=m_mouseGlobalTX*
        m_transform.getMatrix() *
        m_cameras[m_cameraIndex].getViewMatrix() *
        m_cameras[m_cameraIndex].getProjectionMatrix();

    shader->setShaderParamFromMat4("MVP",MVP);

    // now we pass this modelling transform to the shader ModelMatrix
    // finally draw the plane
    prim->draw("plane");
  }
  // now go back to solid drawing
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);



  if (m_drawHelp==true)
  {

   // now render the text using the QT renderText helper function
   m_text->setColour(ngl::Colour(1,1,1));
   m_text->renderText(10,18,"Use Arrow Keys to move Camera i and o to move in and out");
   m_text->renderText(10,18*2,"Use keys 0-4 to switch cameras");
   m_text->renderText(10,18*3,"r roll, y yaw, p pitch");
   m_text->renderText(10,18*4,"e move eye mode");
   m_text->renderText(10,18*5,"l move look mode");
   m_text->renderText(10,18*6,"b move eye and look mode");
   m_text->renderText(10,18*7,"s slide mode" );
   m_text->renderText(10,18*8,"+/- change fov of Camera" );
   m_text->renderText(10,18*9,"z,x,c toggle lights on and off" );
   m_text->renderText(10,18*10,"h toggles help" );
   // now we are going to construct a string to draw the mode information
   QString mode;
   switch ( m_moveMode)
   {
     case MOVEEYE : mode=QString("Move Eye"); break;
     case MOVELOOK : mode=QString("Move Look"); break;
     case MOVEBOTH : mode=QString("Move Both"); break;
     case MOVESLIDE : mode=QString("Move Slide"); break;
   }
   const static int tp=880;
   QString text=QString("Active Camera %1 current mode=%2").arg(m_cameraIndex).arg(mode);
   m_text->renderText(tp,18,text );
   text=QString("FOV = %1 ").arg(m_fov);
   m_text->renderText(tp,18*2,text );
   m_text->renderText(tp,18*3,"ModelView Matrix" );
   // now we use the QString sprintf
   m_text->setColour(ngl::Colour(1,1,0));

   ngl::Mat4 m=m_cameras[m_cameraIndex].getViewMatrix();
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[0],m.openGL()[4],m.openGL()[8],m.openGL()[12]);
   m_text->renderText(tp,18*4,text );
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[1],m.openGL()[5],m.openGL()[9],m.openGL()[13]);
   m_text->renderText(tp,18*5,text );
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[2],m.openGL()[6],m.openGL()[10],m.openGL()[14]);
   m_text->renderText(tp,18*6,text );
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[3],m.openGL()[7],m.openGL()[11],m.openGL()[15]);
   m_text->renderText(tp,18*7,text );

   m_text->setColour(1,1,1);
   m_text->renderText(tp,18*8,"Projection Matrix" );
   // now we use the QString sprintf
   m_text->setColour(1,1,0);
   m=m_cameras[m_cameraIndex].getProjectionMatrix();
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[0],m.openGL()[4],m.openGL()[8],m.openGL()[12]);
   m_text->renderText(tp,18*9,text );
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[1],m.openGL()[5],m.openGL()[9],m.openGL()[13]);
   m_text->renderText(tp,18*10,text );
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[2],m.openGL()[6],m.openGL()[10],m.openGL()[14]);
   m_text->renderText(tp,18*11,text );
   text.sprintf("[ %+0.4f %+0.4f %+0.4f %+0.4f]",m.openGL()[3],m.openGL()[7],m.openGL()[11],m.openGL()[15]);
   m_text->renderText(tp,18*12,text );
   m_text->setColour(ngl::Colour(0,0,0));

   text=QString("Light 0 = %1 Light 1 %2 Light 2 %3").arg(m_lights[0] ? "On" : "Off").arg(m_lights[1] ? "On" : "Off").arg(m_lights[2] ? "On" : "Off");
   m_text->renderText(tp,18*13,text );

  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx=_event->x()-m_origX;
    int diffy=_event->y()-m_origY;
    m_spinXFace += (float) 0.5f * diffy;
    m_spinYFace += (float) 0.5f * diffx;
    m_origX = _event->x();
    m_origY = _event->y();
    update();

  }
        // right mouse translate code
  else if(m_translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = (int)(_event->x() - m_origXPos);
    int diffY = (int)(_event->y() - m_origYPos);
    m_origXPos=_event->x();
    m_origYPos=_event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();

   }
}

void NGLScene::setCameraShape()
{
  int cameraSize=m_cameras.size();
  for(int i=0; i<cameraSize; ++i)
  {
    m_cameras[i].setShape(m_fov,m_aspect, 0.2f,150.0f);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_origX = _event->x();
    m_origY = _event->y();
    m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_origXPos = _event->x();
    m_origYPos = _event->y();
    m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_rotate=false;
  }
        // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_translate=false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_modelPos.m_z-=ZOOM;
	}
	update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  const static float keyIncrement=0.1;
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  // find which key and do appropriate event
  switch (_event->key())
  {
  case Qt::Key_Escape : { QApplication::exit(EXIT_SUCCESS); break; }
  case Qt::Key_1 : { m_cameraIndex=0; break; }
  case Qt::Key_2 : { m_cameraIndex=1; break; }
  case Qt::Key_3 : { m_cameraIndex=2; break; }
  case Qt::Key_4 : { m_cameraIndex=3; break; }

  case Qt::Key_E : { m_moveMode=MOVEEYE; break; }
  case Qt::Key_L : { m_moveMode=MOVELOOK; break; }
  case Qt::Key_B : { m_moveMode=MOVEBOTH; break; }
  case Qt::Key_S : { m_moveMode=MOVESLIDE; break; }
  case Qt::Key_H : { m_drawHelp^=true; break; }
  case Qt::Key_Plus : { ++m_fov; setCameraShape(); break; }
  case Qt::Key_Minus :{ --m_fov; setCameraShape(); break; }
  case Qt::Key_R : { m_cameras[m_cameraIndex].roll(3);  break; }
  case Qt::Key_P : { m_cameras[m_cameraIndex].pitch(3); break; }
  case Qt::Key_Y : { m_cameras[m_cameraIndex].yaw(3);   break; }
  case Qt::Key_Z : { m_lights[0]^=true; break; }
  case Qt::Key_X : { m_lights[1]^=true; break; }
  case Qt::Key_C : { m_lights[2]^=true; break; }
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;

  case Qt::Key_Left :
  {
    switch (m_moveMode)
    {
      case MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(keyIncrement,0,0);  break;}
      case MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(keyIncrement,0,0); break;}
      case MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(keyIncrement,0,0); break;}
      case MOVESLIDE : { m_cameras[m_cameraIndex].slide(keyIncrement,0,0);    break;}
    }
  break;
  } // end move left
  case Qt::Key_Right :
  {
    switch (m_moveMode)
    {
      case MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(-keyIncrement,0,0);  break;}
      case MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(-keyIncrement,0,0); break;}
      case MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(-keyIncrement,0,0); break;}
      case MOVESLIDE : { m_cameras[m_cameraIndex].slide(-keyIncrement,0,0);    break;}
    }
  break;
  } // end move right
  case Qt::Key_Up :
  {
    switch (m_moveMode)
    {
      case MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,keyIncrement,0);  break;}
      case MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,keyIncrement,0); break;}
      case MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,keyIncrement,0); break;}
      case MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,keyIncrement,0);    break;}
    }
  break;
  } // end move up
  case Qt::Key_Down :
  {
    switch (m_moveMode)
    {
      case MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,-keyIncrement,0);  break;}
      case MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,-keyIncrement,0); break;}
      case MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,-keyIncrement,0); break;}
      case MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,-keyIncrement,0);    break;}
    }
  break;
  } // end move down
  case Qt::Key_O:
  {
    switch (m_moveMode)
    {
      case MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,0,keyIncrement);  break;}
      case MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,0,keyIncrement); break;}
      case MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,0,keyIncrement); break;}
      case MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,0,keyIncrement);    break;}
    }
  break;
  } // end move out
  case Qt::Key_I :
  {
    switch (m_moveMode)
    {
      case MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,0,-keyIncrement);  break;}
      case MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,0,-keyIncrement); break;}
      case MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,0,-keyIncrement); break;}
      case MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,0,-keyIncrement);    break;}
    }
  break;
  } // end move in
  default : break;
  }
  // finally update the GLWindow and re-draw
 update();
}

void NGLScene::timerEvent(QTimerEvent *_e)
{

  ++m_rotation;
  update();
}

