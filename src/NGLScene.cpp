#include <QMouseEvent>
#include <QGuiApplication>
#include <QFont>
#include <QRect>

#include <QApplication>
#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <fmt/format.h>
#include <memory>
#include <iostream>
constexpr auto shaderProgram = "PBR";
NGLScene::NGLScene()
{
  // Now set the initial GLWindow attributes to default values

  // now set the inital camera values
  m_cameraIndex = 0;
  m_moveMode = CamMode::MOVEEYE;
  m_drawHelp = true;
  m_fov = 65.0f;
  m_aspect = 1024.0f / 768.0f;
  m_rotation = 0;
  startTimer(20);
  m_lights[0] = true;
  m_lights[1] = true;
  m_lights[2] = true;
  setTitle("ngl::Camera and ngl::Transformation demo");
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::createCameras()
{
  // create a load of cameras
  Camera cam;
  Camera Tcam;
  Camera Scam;
  Camera Fcam;
  // set the different vectors for the camera positions
  ngl::Vec3 eye(0.0f, 5.0f, 8.0f);
  ngl::Vec3 look = ngl::Vec3::zero();
  ngl::Vec3 up(0, 1, 0);

  ngl::Vec3 topEye(0.0f, 8.0f, 0.0f);
  ngl::Vec3 topLook = ngl::Vec3::zero();
  ngl::Vec3 topUp(0.0f, 0.0f, 1.0f);

  ngl::Vec3 sideEye(8.0f, 0.0f, 0.0f);
  ngl::Vec3 sideLook = ngl::Vec3::zero();
  ngl::Vec3 sideUp(0.0f, 1.0f, 0.0f);

  ngl::Vec3 frontEye(5.0f, 2.0f, 8.0f);
  ngl::Vec3 frontLook(0.0f, 0.0f, 0.0f);
  ngl::Vec3 frontUp(0.0f, 1.0f, 0.0f);
  // finally set the cameras shape and position
  cam.set(eye, look, up);
  cam.setShape(m_fov, m_aspect, 0.2f, 150.0f);
  m_cameras.push_back(cam);
  Tcam.set(topEye, topLook, topUp);
  Tcam.setShape(m_fov, m_aspect, 0.5f, 150.0f);
  m_cameras.push_back(Tcam);
  Scam.set(sideEye, sideLook, sideUp);
  Scam.setShape(m_fov, m_aspect, 0.5f, 150.0f);
  m_cameras.push_back(Scam);
  Fcam.set(frontEye, frontLook, frontUp);
  Fcam.setShape(m_fov, m_aspect, 0.5f, 150.0f);
  m_cameras.push_back(Fcam);
}

void NGLScene::resizeGL(int _w, int _h)
{
  m_aspect = static_cast<float>(_w / _h);
  m_win.width = static_cast<int>(_w * devicePixelRatio());
  m_win.height = static_cast<int>(_h * devicePixelRatio());
  m_text->setScreenSize(_w, _h);
}

void NGLScene::initializeGL()
{
  ngl::NGLInit::initialize();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  createCameras();
  // create a plane and sphere
  ngl::VAOPrimitives::createLineGrid("plane", 20, 20, 30);
  ngl::VAOPrimitives::createSphere("sphere", 1.0, 150);

  // now to load the shader and set the values
  // we are creating a shader called PBR to save typos
  // in the code create some constexpr
  constexpr auto vertexShader = "PBRVertex";
  constexpr auto fragShader = "PBRFragment";
  // create the shader program
  ngl::ShaderLib::createShaderProgram(shaderProgram);
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader(vertexShader, ngl::ShaderType::VERTEX);
  ngl::ShaderLib::attachShader(fragShader, ngl::ShaderType::FRAGMENT);
  // attach the source
  ngl::ShaderLib::loadShaderSource(vertexShader, "shaders/PBRVertex.glsl");
  ngl::ShaderLib::loadShaderSource(fragShader, "shaders/PBRFragment.glsl");
  // compile the shaders
  ngl::ShaderLib::compileShader(vertexShader);
  ngl::ShaderLib::compileShader(fragShader);
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram(shaderProgram, vertexShader);
  ngl::ShaderLib::attachShaderToProgram(shaderProgram, fragShader);
  // now we have associated that data we can link the shader
  ngl::ShaderLib::linkProgramObject(shaderProgram);
  // and make it active ready to load values
  ngl::ShaderLib::use(shaderProgram);
  // We now create our view matrix for a static camera
  ngl::Vec3 from(0.0f, 2.0f, 2.0f);
  ngl::Vec3 to(0.0f, 0.0f, 0.0f);
  ngl::Vec3 up(0.0f, 1.0f, 0.0f);
  // setup the default shader material and light porerties
  // these are "uniform" so will retain their values
  ngl::ShaderLib::setUniform("lightColour[0]", 100.0f, 100.0f, 100.0f);
  ngl::ShaderLib::setUniform("lightColour[1]", 100.0f, 100.0f, 100.0f);
  ngl::ShaderLib::setUniform("lightColour[2]", 100.0f, 100.0f, 100.0f);
  ngl::ShaderLib::setUniform("exposure", 2.2f);
  ngl::ShaderLib::setUniform("albedo", 0.950f, 0.71f, 0.29f);

  ngl::ShaderLib::setUniform("metallic", 1.02f);
  ngl::ShaderLib::setUniform("roughness", 0.38f);
  ngl::ShaderLib::setUniform("ao", 0.2f);
  ngl::ShaderLib::setUniform("lightPosition[0]", -2.0f, 0.0f, 1.0f);
  ngl::ShaderLib::setUniform("lightPosition[1]", 2.0f, 0.0f, 1.0f);
  ngl::ShaderLib::setUniform("lightPosition[2]", 0.0f, 2.0f, 1.0f);

  ngl::ShaderLib::createShaderProgram("Colour");

  ngl::ShaderLib::attachShader("ColourVertex", ngl::ShaderType::VERTEX);
  ngl::ShaderLib::attachShader("ColourFragment", ngl::ShaderType::FRAGMENT);
  ngl::ShaderLib::loadShaderSource("ColourVertex", "shaders/Colour.vs");
  ngl::ShaderLib::loadShaderSource("ColourFragment", "shaders/Colour.fs");

  ngl::ShaderLib::compileShader("ColourVertex");
  ngl::ShaderLib::compileShader("ColourFragment");
  ngl::ShaderLib::attachShaderToProgram("Colour", "ColourVertex");
  ngl::ShaderLib::attachShaderToProgram("Colour", "ColourFragment");

  ngl::ShaderLib::bindAttribute("Colour", 0, "inVert");
  ngl::ShaderLib::linkProgramObject("Colour");
  ngl::ShaderLib::use("Colour");
  ngl::ShaderLib::setUniform("Colour", 1.0f, 1.0f, 1.0f, 1.0f);

  m_text = std::make_unique<ngl::Text>("fonts/Arial.ttf", 18);
  m_text->setScreenSize(1024, 720);
}

void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib::use("PBR");
  struct transform
  {
    ngl::Mat4 MVP;
    ngl::Mat4 normalMatrix;
    ngl::Mat4 M;
  };

  transform t;
  t.M = m_mouseGlobalTX * m_transform.getMatrix();

  t.MVP = m_cameras[m_cameraIndex].getVPMatrix() * t.M;
  t.normalMatrix = t.M;
  t.normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniformBuffer("TransformUBO", sizeof(transform), &t.MVP.m_00);
  ngl::ShaderLib::setUniform("camPos", m_cameras[m_cameraIndex].getEye().toVec3());
  // Now do lights
  const float on = 100.0f;
  const float off = 0.0f;
  for (size_t i = 0; i < 3; ++i)
  {
    std::string light = fmt::format("lightColour[{0}]", i);
    if (m_lights[i] == true)
    {
      ngl::ShaderLib::setUniform(light, on, on, on);
    }
    else
    {
      ngl::ShaderLib::setUniform(light, off, off, off);
    }
  }
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, m_win.width, m_win.height);
  // set the text transforms

  // Rotation based on the mouse position for our global
  // transform
  auto rotX = ngl::Mat4::rotateX(m_win.spinXFace);
  auto rotY = ngl::Mat4::rotateY(m_win.spinYFace);
  // multiply the rotations
  m_mouseGlobalTX = rotY * rotX;

  m_transform.setRotation(0, m_rotation, 0);
  m_transform.setPosition(0.0f, 0.0f, 0.0f);

  loadMatricesToShader();
  ngl::VAOPrimitives::draw("teapot");

  ngl::Mat4 tp;
  m_transform.reset();
  {

    // translate into position
    m_transform.setPosition(-1.8f, 0.1f, 0);
    // rotate object
    m_transform.setRotation(m_rotation, m_rotation, m_rotation);
    // pass values to the shader
    loadMatricesToShader();
    // draw
    tp = m_transform.getMatrix();

    ngl::VAOPrimitives::draw("cube");
  }
  // now we draw the football
  m_transform.reset();
  {
    // set the transforms for pos and scale
    m_transform.setPosition(1.5, 0, 0);
    m_transform.setScale(0.5, 0.5, 0.5);
    m_transform.setRotation(m_rotation, m_rotation, m_rotation);
    loadMatricesToShader();
    // draw the football
    ngl::VAOPrimitives::draw("football");
  }
  // now we are going to draw the help

  // now we are going to draw the grid in wireframe mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  // load the colour  shader and set the values,
  ngl::ShaderLib::use("Colour");
  // now push the tx stack it helps to use { } to block off the transforms
  m_transform.reset();
  {
    // translate the grid down by -0.5 so it is at the base level of the objects
    m_transform.setPosition(0, -0.5, 0);
    ngl::Mat4 MVP;
    MVP = m_cameras[m_cameraIndex].getVPMatrix() *
          m_mouseGlobalTX *
          m_transform.getMatrix();
    ngl::ShaderLib::setUniform("MVP", MVP);

    // now we pass this modelling transform to the shader ModelMatrix
    // finally draw the plane
    ngl::VAOPrimitives::draw("plane");
  }
  // now go back to solid drawing
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if (m_drawHelp == true)
  {

    // now render the text using the QT renderText helper function
    m_text->setColour(1.0f, 1.0f, 0.0f);
    int pos = 700;
    m_text->renderText(10, 700, "Use Arrow Keys to move Camera i and o to move in and out");
    m_text->renderText(10, (pos -= 19), "Use keys 0-4 to switch cameras");
    m_text->renderText(10, (pos -= 19), "r roll, y yaw, p pitch");
    m_text->renderText(10, (pos -= 19), "e move eye mode");
    m_text->renderText(10, (pos -= 19), "l move look mode");
    m_text->renderText(10, (pos -= 19), "b move eye and look mode");
    m_text->renderText(10, (pos -= 19), "s slide mode");
    m_text->renderText(10, (pos -= 19), "+/- change fov of Camera");
    m_text->renderText(10, (pos -= 19), "z,x,c toggle lights on and off");
    m_text->renderText(10, (pos -= 19), "h toggles help");
    // now we are going to construct a string to draw the mode information
    std::string mode;
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
      mode = "Move Eye";
      break;
    case CamMode::MOVELOOK:
      mode = "Move Look";
      break;
    case CamMode::MOVEBOTH:
      mode = "Move Both";
      break;
    case CamMode::MOVESLIDE:
      mode = "Move Slide";
      break;
    }
    pos = 700;
    const static int tp = 700;
    std::string text = fmt::format("Active Camera {} current mode {}", m_cameraIndex, mode);
    m_text->renderText(tp, pos, text);
    text = fmt::format("FOV = {} ", m_fov);
    m_text->renderText(tp, (pos -= 18), text);
    m_text->renderText(tp, (pos -= 18), "ModelView Matrix");
    // now we use the QString sprintf
    m_text->setColour(1.0f, 1.0f, 0.0f);

    ngl::Mat4 m = m_cameras[m_cameraIndex].getViewMatrix();

    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[0], m.openGL()[4], m.openGL()[8], m.openGL()[12]);
    m_text->renderText(tp, (pos -= 18), text);
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[1], m.openGL()[5], m.openGL()[9], m.openGL()[13]);
    m_text->renderText(tp, (pos -= 18), text);
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[2], m.openGL()[6], m.openGL()[10], m.openGL()[14]);
    m_text->renderText(tp, (pos -= 18), text);
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[3], m.openGL()[7], m.openGL()[11], m.openGL()[15]);
    m_text->renderText(tp, (pos -= 18), text);

    m_text->setColour(1, 1, 1);
    m_text->renderText(tp, (pos -= 18), "Projection Matrix");
    // now we use the QString sprintf
    m_text->setColour(1, 1, 0);
    m = m_cameras[m_cameraIndex].getProjectionMatrix();
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[0], m.openGL()[4], m.openGL()[8], m.openGL()[12]);
    m_text->renderText(tp, (pos -= 18), text);
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[1], m.openGL()[5], m.openGL()[9], m.openGL()[13]);
    m_text->renderText(tp, (pos -= 18), text);
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[2], m.openGL()[6], m.openGL()[10], m.openGL()[14]);
    m_text->renderText(tp, (pos -= 18), text);
    text = fmt::format("[ {:+0.4f} {:+0.4f} {:+0.4f} {:+0.4f}]", m.openGL()[3], m.openGL()[7], m.openGL()[11], m.openGL()[15]);
    m_text->renderText(tp, (pos -= 18), text);
    m_text->setColour(0.0f, 0.0f, 0.0f);

    text = fmt::format("Light 0 = {} Light 1 {} Light 2 {}", m_lights[0] ? "On" : "Off", m_lights[1] ? "On" : "Off", m_lights[2] ? "On" : "Off");
    m_text->renderText(tp, (pos -= 18), text);
  }
}

void NGLScene::setCameraShape()
{
  for (auto &cam : m_cameras)
  {
    cam.setShape(m_fov, m_aspect, 0.2f, 150.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent(QMouseEvent *_event)
{
// note the method buttons() is the button state when event was called
// that is different from button() which is used to check which button was
// pressed when the mousePress/Release event is generated
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif
  if (m_win.rotate && _event->buttons() == Qt::LeftButton)
  {
    int diffx = position.x() - m_win.origX;
    int diffy = position.y() - m_win.origY;
    m_win.spinXFace += static_cast<int>(0.5f * diffy);
    m_win.spinYFace += static_cast<int>(0.5f * diffx);
    m_win.origX = position.x();
    m_win.origY = position.y();
    update();
  }
  // right mouse translate code
  else if (m_win.translate && _event->buttons() == Qt::RightButton)
  {
    int diffX = static_cast<int>(position.x() - m_win.origXPos);
    int diffY = static_cast<int>(position.y() - m_win.origYPos);
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_modelPos.m_x += INCREMENT * diffX;
    m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent(QMouseEvent *_event)
{
// that method is called when the mouse button is pressed in this case we
// store the value where the maouse was clicked (x,y) and set the Rotate flag to true
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
  auto position = _event->position();
#else
  auto position = _event->pos();
#endif

  if (_event->button() == Qt::LeftButton)
  {
    m_win.origX = position.x();
    m_win.origY = position.y();
    m_win.rotate = true;
  }
  // right mouse translate mode
  else if (_event->button() == Qt::RightButton)
  {
    m_win.origXPos = position.x();
    m_win.origYPos = position.y();
    m_win.translate = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent(QMouseEvent *_event)
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if (_event->button() == Qt::LeftButton)
  {
    m_win.rotate = false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_win.translate = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  // check the diff of the wheel position (0 means no change)
  if (_event->angleDelta().x() > 0)
  {
    m_modelPos.m_z += ZOOM;
  }
  else if (_event->angleDelta().x() < 0)
  {
    m_modelPos.m_z -= ZOOM;
  }
  update();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  const static float keyIncrement = 0.1;
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  // find which key and do appropriate event
  switch (_event->key())
  {
  case Qt::Key_Escape:
  {
    QApplication::exit(EXIT_SUCCESS);
    break;
  }
  case Qt::Key_1:
  {
    m_cameraIndex = 0;
    break;
  }
  case Qt::Key_2:
  {
    m_cameraIndex = 1;
    break;
  }
  case Qt::Key_3:
  {
    m_cameraIndex = 2;
    break;
  }
  case Qt::Key_4:
  {
    m_cameraIndex = 3;
    break;
  }

  case Qt::Key_E:
  {
    m_moveMode = CamMode::MOVEEYE;
    break;
  }
  case Qt::Key_L:
  {
    m_moveMode = CamMode::MOVELOOK;
    break;
  }
  case Qt::Key_B:
  {
    m_moveMode = CamMode::MOVEBOTH;
    break;
  }
  case Qt::Key_S:
  {
    m_moveMode = CamMode::MOVESLIDE;
    break;
  }
  case Qt::Key_H:
  {
    m_drawHelp ^= true;
    break;
  }
  case Qt::Key_Plus:
  {
    ++m_fov;
    setCameraShape();
    break;
  }
  case Qt::Key_Minus:
  {
    --m_fov;
    setCameraShape();
    break;
  }
  case Qt::Key_R:
  {
    m_cameras[m_cameraIndex].roll(3);
    break;
  }
  case Qt::Key_P:
  {
    m_cameras[m_cameraIndex].pitch(3);
    break;
  }
  case Qt::Key_Y:
  {
    m_cameras[m_cameraIndex].yaw(3);
    break;
  }
  case Qt::Key_Z:
  {
    m_lights[0] ^= true;
    break;
  }
  case Qt::Key_X:
  {
    m_lights[1] ^= true;
    break;
  }
  case Qt::Key_C:
  {
    m_lights[2] ^= true;
    break;
  }
  case Qt::Key_W:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;

  case Qt::Key_Left:
  {
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
    {
      m_cameras[m_cameraIndex].moveEye(keyIncrement, 0, 0);
      break;
    }
    case CamMode::MOVELOOK:
    {
      m_cameras[m_cameraIndex].moveLook(keyIncrement, 0, 0);
      break;
    }
    case CamMode::MOVEBOTH:
    {
      m_cameras[m_cameraIndex].moveBoth(keyIncrement, 0, 0);
      break;
    }
    case CamMode::MOVESLIDE:
    {
      m_cameras[m_cameraIndex].slide(keyIncrement, 0, 0);
      break;
    }
    }
    break;
  } // end move left
  case Qt::Key_Right:
  {
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
    {
      m_cameras[m_cameraIndex].moveEye(-keyIncrement, 0, 0);
      break;
    }
    case CamMode::MOVELOOK:
    {
      m_cameras[m_cameraIndex].moveLook(-keyIncrement, 0, 0);
      break;
    }
    case CamMode::MOVEBOTH:
    {
      m_cameras[m_cameraIndex].moveBoth(-keyIncrement, 0, 0);
      break;
    }
    case CamMode::MOVESLIDE:
    {
      m_cameras[m_cameraIndex].slide(-keyIncrement, 0, 0);
      break;
    }
    }
    break;
  } // end move right
  case Qt::Key_Up:
  {
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
    {
      m_cameras[m_cameraIndex].moveEye(0, keyIncrement, 0);
      break;
    }
    case CamMode::MOVELOOK:
    {
      m_cameras[m_cameraIndex].moveLook(0, keyIncrement, 0);
      break;
    }
    case CamMode::MOVEBOTH:
    {
      m_cameras[m_cameraIndex].moveBoth(0, keyIncrement, 0);
      break;
    }
    case CamMode::MOVESLIDE:
    {
      m_cameras[m_cameraIndex].slide(0, keyIncrement, 0);
      break;
    }
    }
    break;
  } // end move up
  case Qt::Key_Down:
  {
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
    {
      m_cameras[m_cameraIndex].moveEye(0, -keyIncrement, 0);
      break;
    }
    case CamMode::MOVELOOK:
    {
      m_cameras[m_cameraIndex].moveLook(0, -keyIncrement, 0);
      break;
    }
    case CamMode::MOVEBOTH:
    {
      m_cameras[m_cameraIndex].moveBoth(0, -keyIncrement, 0);
      break;
    }
    case CamMode::MOVESLIDE:
    {
      m_cameras[m_cameraIndex].slide(0, -keyIncrement, 0);
      break;
    }
    }
    break;
  } // end move down
  case Qt::Key_O:
  {
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
    {
      m_cameras[m_cameraIndex].moveEye(0, 0, keyIncrement);
      break;
    }
    case CamMode::MOVELOOK:
    {
      m_cameras[m_cameraIndex].moveLook(0, 0, keyIncrement);
      break;
    }
    case CamMode::MOVEBOTH:
    {
      m_cameras[m_cameraIndex].moveBoth(0, 0, keyIncrement);
      break;
    }
    case CamMode::MOVESLIDE:
    {
      m_cameras[m_cameraIndex].slide(0, 0, keyIncrement);
      break;
    }
    }
    break;
  } // end move out
  case Qt::Key_I:
  {
    switch (m_moveMode)
    {
    case CamMode::MOVEEYE:
    {
      m_cameras[m_cameraIndex].moveEye(0, 0, -keyIncrement);
      break;
    }
    case CamMode::MOVELOOK:
    {
      m_cameras[m_cameraIndex].moveLook(0, 0, -keyIncrement);
      break;
    }
    case CamMode::MOVEBOTH:
    {
      m_cameras[m_cameraIndex].moveBoth(0, 0, -keyIncrement);
      break;
    }
    case CamMode::MOVESLIDE:
    {
      m_cameras[m_cameraIndex].slide(0, 0, -keyIncrement);
      break;
    }
    }
    break;
  } // end move in
  default:
    break;
  }
  // finally update the GLWindow and re-draw
  update();
}

void NGLScene::timerEvent(QTimerEvent *_e)
{
  ++m_rotation;
  update();
}
