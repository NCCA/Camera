#include <QMouseEvent>
#include <QGuiApplication>
#include <QFont>
#include <QDesktopWidget>
#include <QRect>

#include <QApplication>
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/fmt/format.h>
constexpr auto shaderProgram="PBR";
NGLScene::NGLScene()
{
  // Now set the initial GLWindow attributes to default values

  // now set the inital camera values
  m_cameraIndex=0;
  m_moveMode=CamMode::MOVEEYE;
  m_drawHelp=true;
  m_fov=65.0f;
  m_aspect=1024.0f/768.0f;
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
}

void NGLScene::createCameras()
{
  // create a load of cameras
  Camera cam;
  Camera Tcam;
  Camera Scam;
  Camera Fcam;
  // set the different vectors for the camera positions
  ngl::Vec3 eye(0.0f,5.0f,8.0f);
  ngl::Vec3 look=ngl::Vec3::zero();
  ngl::Vec3 up(0,1,0);

  ngl::Vec3 topEye(0.0f,8.0f,0.0f);
  ngl::Vec3 topLook=ngl::Vec3::zero();
  ngl::Vec3 topUp(0.0f,0.0f,1.0f);

  ngl::Vec3 sideEye(8.0f,0.0f,0.0f);
  ngl::Vec3 sideLook=ngl::Vec3::zero();
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


void NGLScene::resizeGL( int _w, int _h )
{
  m_aspect=static_cast<float>(_w/_h);
  m_win.width=static_cast<int>(_w*devicePixelRatio());
  m_win.height=static_cast<int>(_h*devicePixelRatio());
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
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  // we are creating a shader called PBR to save typos
  // in the code create some constexpr
  constexpr auto vertexShader  = "PBRVertex";
  constexpr auto fragShader    = "PBRFragment";
  // create the shader program
  shader->createShaderProgram( shaderProgram );
  // now we are going to create empty shaders for Frag and Vert
  shader->attachShader( vertexShader, ngl::ShaderType::VERTEX );
  shader->attachShader( fragShader, ngl::ShaderType::FRAGMENT );
  // attach the source
  shader->loadShaderSource( vertexShader, "shaders/PBRVertex.glsl" );
  shader->loadShaderSource( fragShader, "shaders/PBRFragment.glsl" );
  // compile the shaders
  shader->compileShader( vertexShader );
  shader->compileShader( fragShader );
  // add them to the program
  shader->attachShaderToProgram( shaderProgram, vertexShader );
  shader->attachShaderToProgram( shaderProgram, fragShader );
  // now we have associated that data we can link the shader
  shader->linkProgramObject( shaderProgram );
  // and make it active ready to load values
  ( *shader )[ shaderProgram ]->use();
 // We now create our view matrix for a static camera
  ngl::Vec3 from( 0.0f, 2.0f, 2.0f );
  ngl::Vec3 to( 0.0f, 0.0f, 0.0f );
  ngl::Vec3 up( 0.0f, 1.0f, 0.0f );
  // setup the default shader material and light porerties
  // these are "uniform" so will retain their values
  shader->setUniform("lightColour[0]",100.0f,100.0f,100.0f);
  shader->setUniform("lightColour[1]",100.0f,100.0f,100.0f);
  shader->setUniform("lightColour[2]",100.0f,100.0f,100.0f);
  shader->setUniform("exposure",2.2f);
  shader->setUniform("albedo",0.950f, 0.71f, 0.29f);

  shader->setUniform("metallic",1.02f);
  shader->setUniform("roughness",0.38f);
  shader->setUniform("ao",0.2f);
  shader->setUniform("lightPosition[0]",-2.0f,0.0f,1.0f);
  shader->setUniform("lightPosition[1]", 2.0f,0.0f,1.0f);
  shader->setUniform("lightPosition[2]", 0.0f,2.0f,1.0f);

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
  shader->setUniform("Colour",1.0f,1.0f,1.0f,1.0f);

  m_text.reset(new  ngl::Text(QFont("Arial",18)));
  m_text->setScreenSize(width(),height());
}


void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  shader->use("PBR");
  struct transform
  {
    ngl::Mat4 MVP;
    ngl::Mat4 normalMatrix;
    ngl::Mat4 M;
  };

   transform t;
   t.M= m_mouseGlobalTX*m_transform.getMatrix();

   t.MVP=m_cameras[m_cameraIndex].getVPMatrix()*t.M;
   t.normalMatrix=t.M;
   t.normalMatrix.inverse().transpose();
   shader->setUniformBuffer("TransformUBO",sizeof(transform),&t.MVP.m_00);
   shader->setUniform("camPos",m_cameras[m_cameraIndex].getEye().toVec3());
   // Now do lights
   const float on=100.0f;
   const float off=0.0f;
   for(size_t i=0; i<3; ++i)
   {
     std::string light = fmt::format("lightColour[{0}]",i);
     if( m_lights[i] == true)
     {
       shader->setUniform(light,on,on,on);
     }
     else
     {
       shader->setUniform(light,off,off,off);
     }
   }
}

void NGLScene::paintGL()
{
  // grab an instance of the shader manager
  ngl::ShaderLib *shader=ngl::ShaderLib::instance();
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  // set the text transforms
  float x,y;
  float mw=1490;
  float mh=900;
  x=1.1f-float(mw-m_win.width)/mw;
  y=1.1f-float(mh-m_win.height)/mh;
  m_text->setScreenSize(width(),height());
  m_text->setTransform(x,y);

  // Rotation based on the mouse position for our global
  // transform

  // Rotation based on the mouse position for our global
  // transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;


  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  m_transform.setRotation(0,m_rotation,0);
  m_transform.setPosition(0.0f,0.0f,0.0f);

  loadMatricesToShader();
  prim->draw("teapot");

  ngl::Mat4 tp;
  m_transform.reset();
  {

    // translate into position
    m_transform.setPosition(-1.8f,0.1f,0);
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
    MVP=m_cameras[m_cameraIndex].getVPMatrix() *
        m_mouseGlobalTX *
        m_transform.getMatrix() ;
    shader->setUniform("MVP",MVP);

    // now we pass this modelling transform to the shader ModelMatrix
    // finally draw the plane
    prim->draw("plane");
  }
  // now go back to solid drawing
  glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);



  if (m_drawHelp==true)
  {

   // now render the text using the QT renderText helper function
   m_text->setColour(1.0f,1.0f,1.0f);
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
     case CamMode::MOVEEYE : mode=QString("Move Eye"); break;
     case CamMode::MOVELOOK : mode=QString("Move Look"); break;
     case CamMode::MOVEBOTH : mode=QString("Move Both"); break;
     case CamMode::MOVESLIDE : mode=QString("Move Slide"); break;
   }
   const static int tp=880;
   QString text=QString("Active Camera %1 current mode=%2").arg(m_cameraIndex).arg(mode);
   m_text->renderText(tp,18,text );
   text=QString("FOV = %1 ").arg(m_fov);
   m_text->renderText(tp,18*2,text );
   m_text->renderText(tp,18*3,"ModelView Matrix" );
   // now we use the QString sprintf
   m_text->setColour((1.0f,1.0f,0.0f));

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
   m_text->setColour(0.0f,0.0f,0.0f);

   text=QString("Light 0 = %1 Light 1 %2 Light 2 %3").arg(m_lights[0] ? "On" : "Off").arg(m_lights[1] ? "On" : "Off").arg(m_lights[2] ? "On" : "Off");
   m_text->renderText(tp,18*13,text );

  }
}

void NGLScene::setCameraShape()
{
  for(auto &cam : m_cameras)
  {
    cam.setShape(m_fov,m_aspect, 0.2f,150.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if ( m_win.rotate && _event->buttons() == Qt::LeftButton )
  {
    int diffx = _event->x() - m_win.origX;
    int diffy = _event->y() - m_win.origY;
    m_win.spinXFace += static_cast<int>( 0.5f * diffy );
    m_win.spinYFace += static_cast<int>( 0.5f * diffx );
    m_win.origX = _event->x();
    m_win.origY = _event->y();
    update();
  }
  // right mouse translate code
  else if ( m_win.translate && _event->buttons() == Qt::RightButton )
  {
    int diffX      = static_cast<int>( _event->x() - m_win.origXPos );
    int diffY      = static_cast<int>( _event->y() - m_win.origYPos );
    m_win.origXPos = _event->x();
    m_win.origYPos = _event->y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.origX  = _event->x();
    m_win.origY  = _event->y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if ( _event->button() == Qt::RightButton )
  {
    m_win.origXPos  = _event->x();
    m_win.origYPos  = _event->y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if ( _event->button() == Qt::RightButton )
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{

  // check the diff of the wheel position (0 means no change)
  if ( _event->delta() > 0 )
  {
    m_modelPos.m_z += ZOOM;
  }
  else if ( _event->delta() < 0 )
  {
    m_modelPos.m_z -= ZOOM;
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

  case Qt::Key_E : { m_moveMode=CamMode::MOVEEYE; break; }
  case Qt::Key_L : { m_moveMode=CamMode::MOVELOOK; break; }
  case Qt::Key_B : { m_moveMode=CamMode::MOVEBOTH; break; }
  case Qt::Key_S : { m_moveMode=CamMode::MOVESLIDE; break; }
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
      case CamMode::MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(keyIncrement,0,0);  break;}
      case CamMode::MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(keyIncrement,0,0); break;}
      case CamMode::MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(keyIncrement,0,0); break;}
      case CamMode::MOVESLIDE : { m_cameras[m_cameraIndex].slide(keyIncrement,0,0);    break;}
    }
  break;
  } // end move left
  case Qt::Key_Right :
  {
    switch (m_moveMode)
    {
      case CamMode::MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(-keyIncrement,0,0);  break;}
      case CamMode::MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(-keyIncrement,0,0); break;}
      case CamMode::MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(-keyIncrement,0,0); break;}
      case CamMode::MOVESLIDE : { m_cameras[m_cameraIndex].slide(-keyIncrement,0,0);    break;}
    }
  break;
  } // end move right
  case Qt::Key_Up :
  {
    switch (m_moveMode)
    {
      case CamMode::MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,keyIncrement,0);  break;}
      case CamMode::MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,keyIncrement,0); break;}
      case CamMode::MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,keyIncrement,0); break;}
      case CamMode::MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,keyIncrement,0);    break;}
    }
  break;
  } // end move up
  case Qt::Key_Down :
  {
    switch (m_moveMode)
    {
      case CamMode::MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,-keyIncrement,0);  break;}
      case CamMode::MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,-keyIncrement,0); break;}
      case CamMode::MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,-keyIncrement,0); break;}
      case CamMode::MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,-keyIncrement,0);    break;}
    }
  break;
  } // end move down
  case Qt::Key_O:
  {
    switch (m_moveMode)
    {
      case CamMode::MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,0,keyIncrement);  break;}
      case CamMode::MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,0,keyIncrement); break;}
      case CamMode::MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,0,keyIncrement); break;}
      case CamMode::MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,0,keyIncrement);    break;}
    }
  break;
  } // end move out
  case Qt::Key_I :
  {
    switch (m_moveMode)
    {
      case CamMode::MOVEEYE :   { m_cameras[m_cameraIndex].moveEye(0,0,-keyIncrement);  break;}
      case CamMode::MOVELOOK :  { m_cameras[m_cameraIndex].moveLook(0,0,-keyIncrement); break;}
      case CamMode::MOVEBOTH :  { m_cameras[m_cameraIndex].moveBoth(0,0,-keyIncrement); break;}
      case CamMode::MOVESLIDE : { m_cameras[m_cameraIndex].slide(0,0,-keyIncrement);    break;}
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

