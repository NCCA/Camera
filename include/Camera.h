/*
  Copyright (C) 2009 Jon Macey

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CAMERA_H_
#define CAMERA_H_
/// @file Camera.h
/// @brief a simple camera class based on the Hill Book
// must include types.h first for ngl::Real and GLEW if required
#include <ngl/AABB.h>
#include <ngl/Plane.h>
#include <ngl/Vec4.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <cmath>



//----------------------------------------------------------------------------------------------------------------------
/// @enum used to describe intercects with fustrum
enum  class CameraIntercept : char {OUTSIDE, INTERSECT, INSIDE};

//----------------------------------------------------------------------------------------------------------------------
/// @class Camera
/// @brief simple camera class to allow movement in an opengl scene.
///  a lot of this stuff is from the HILL book Computer Graphics with OpenGL 2nd Ed Prentice Hall
///  a very good book
/// fustrum culling modified from http://www.lighthouse3d.com/opengl/viewfrustum/index.php?defvf
///  @author Jonathan Macey
///  @version 5.0
///  @date
/// Last Update added fustrum calculation
/// Revision History : \n
/// 27/08/08 Added experimental projection modes
/// 28/09/09 Updated to NCCA Coding standard \n
///  @todo Finish off the different projections modes at present persp and ortho work
//----------------------------------------------------------------------------------------------------------------------

class  Camera
{

public :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief default constructor
  //----------------------------------------------------------------------------------------------------------------------
  Camera() noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief constructor setting the  camera using eye look and up
  /// @param[in] _eye the eye position of the Camera
  /// @param[in] _look where the camera is  looking
  /// @param[in] _up the notion of upwardness for the  camers (get re-calculated from the other vectors)
  //----------------------------------------------------------------------------------------------------------------------
  Camera(const ngl::Vec3& _eye, const ngl::Vec3& _look,const ngl::Vec3& _up) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief roll the cameara around the  Z axis
  /// @param[in] _angle the angle to roll in degrees
  //----------------------------------------------------------------------------------------------------------------------
  void roll(ngl::Real _angle) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief roll the cameara around the  x axis
  /// @param[in] _angle the angle to roll in degrees
  //----------------------------------------------------------------------------------------------------------------------
  void pitch(ngl::Real _angle) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief roll the cameara around the  y axis
  /// @param[in] _angle the angle to roll in degrees
  //----------------------------------------------------------------------------------------------------------------------
  void yaw(ngl::Real _angle) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief slide the camera around  the U V and N axis
  /// @param[in] _du ammount to slide in the U
  /// @param[in] _dv ammount to slide in the V
  /// @param[in] _dn ammount to slide in the N
  //----------------------------------------------------------------------------------------------------------------------
  void slide(ngl::Real _du,ngl::Real _dv,ngl::Real _dn) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set the Camera position using eye look and up vectors
  /// @param[in] _eye the new  eye position
  /// @param[in] _look the new look position
  /// @param[in] _up the notional up direction of the Camera
  //----------------------------------------------------------------------------------------------------------------------
  void set(const ngl::Vec3 &_eye, const ngl::Vec3 &_look, const ngl::Vec3 &_up) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set the shape of the Camera
  /// @param[in] _viewAngle the view angle  of the camera from the eye
  /// @param[in] _aspect the aspect ratio of the camera can be  calculated from Width/height
  /// @param[in] _near the near clipping plane
  /// @param[in] _far the far clipping plane
  //----------------------------------------------------------------------------------------------------------------------
  void setShape(ngl::Real _viewAngle, ngl::Real _aspect, ngl::Real _near, ngl::Real _far) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief re-set the aspect ratio of the camera
  /// @param[in] _asp the new aspect ratio
  //----------------------------------------------------------------------------------------------------------------------
  void setAspect(ngl::Real _asp) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief sets some nice default camera values
  //----------------------------------------------------------------------------------------------------------------------
  void setDefaultCamera() noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief move the camera in world space (i.e. add a translation to each of the axis
  /// @param[in] _dx the translation in the x
  /// @param[in] _dy the translation in the y
  /// @param[in] _dz the translation in the z
  //----------------------------------------------------------------------------------------------------------------------
  void move(ngl::Real _dx,ngl::Real _dy,ngl::Real _dz) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief move both the eye and the look at the same time
  /// @param[in] _dx ammount to move in the x
  /// @param[in] _dy ammount to move in the y
  /// @param[in] _dz ammount to move in the z
  //----------------------------------------------------------------------------------------------------------------------
  void moveBoth( ngl::Real _dx, ngl::Real _dy, ngl::Real _dz) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief move the eye position
  /// @param[in] _dx ammount to move the eye in the x
  /// @param[in] _dy ammount to move the eye in the y
  /// @param[in] _dz ammount to move the eye in the z
  //----------------------------------------------------------------------------------------------------------------------
  void moveEye(ngl::Real _dx,  ngl::Real _dy, ngl::Real _dz) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief move the look position
  /// @param[in] _dx ammount to move the look in the x
  /// @param[in] _dy ammount to move the look in the y
  /// @param[in] _dz ammount to move the look in the z
  //----------------------------------------------------------------------------------------------------------------------
  void moveLook(ngl::Real _dx, ngl::Real _dy, ngl::Real _dz) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set the viewangle for the Camera
  /// @param[in] _angle new angle
  //----------------------------------------------------------------------------------------------------------------------
  void setViewAngle(ngl::Real _angle) noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Use is called to make this the current camera and set the MODELVIEW and PROJECTION matrices in OpenGL
  //----------------------------------------------------------------------------------------------------------------------
  void update() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to do a Yaw based on Euler rotation with normalised values
  /// @param[in] _angle the angle to rotate in degrees
  //----------------------------------------------------------------------------------------------------------------------
  void normalisedYaw(ngl::Real _angle) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to do a Pitch based on Euler rotation with normalised values
  /// @param[in] _angle the angle to rotate in degrees
  //----------------------------------------------------------------------------------------------------------------------
  void normalisedPitch(ngl::Real _angle) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to do a Roll based on Euler rotation with normalised values
  /// @param[in] _angle the angle to rotate in degrees
  //----------------------------------------------------------------------------------------------------------------------
  void normalisedRoll(ngl::Real _angle) noexcept;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accesor to get the view matrix
  /// @returns the current view matrix
  //----------------------------------------------------------------------------------------------------------------------
  const ngl::Mat4 & getViewMatrix() noexcept{return m_viewMatrix;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief accesor to get the projection matrix
  /// @returns the current projection matrix
  //----------------------------------------------------------------------------------------------------------------------
  const ngl::Mat4 & getProjectionMatrix() noexcept{return m_projectionMatrix;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the View * Projection matrix used as part of the MVP calculations
  /// @returns the m_projectionMatrix*m_viewMatrix
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4  getVPMatrix()const  noexcept{return m_projectionMatrix*m_viewMatrix;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the eye position
  /// @returns the current eye pos
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4  getEye() const noexcept{return m_eye;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set the eye position and re-calculate the matrices
  //----------------------------------------------------------------------------------------------------------------------
  void setEye(ngl::Vec4 _e) noexcept{m_eye=_e; update();}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set the look position and re-calculate the matrices
  //----------------------------------------------------------------------------------------------------------------------
  void setLook(ngl::Vec4 _e) noexcept{m_look=_e; update();}
  //----------------------------------------------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the look position
  /// @returns the current look pos
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 getLook() const noexcept{return m_look;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the up vector
  /// @returns the current up vector
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 getUp() const noexcept{return m_up;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the u vector
  /// @returns the current u vector
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 getU() const noexcept{return m_u;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the v vector
  /// @returns the current v vector
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 getV()const  noexcept{return m_v;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the n vector
  /// @returns the current n vector
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 getN() const  noexcept{return m_n;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the fov value
  /// @returns the current fov value
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real getFOV() const  noexcept{return m_fov;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the aspect value
  /// @returns the current aspect value
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real getAspect() const  noexcept{return m_aspect;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the near clip value
  /// @returns the current near clip value
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real getNear() const  noexcept{return m_zNear;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief get the far clip value
  /// @returns the current far clip value
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real getFar() const  noexcept{return m_zFar;}
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief calculate the frustum for clipping etc
  //----------------------------------------------------------------------------------------------------------------------
  void calculateFrustum() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief draw the frustum for clipping etc
  //----------------------------------------------------------------------------------------------------------------------
  void drawFrustum() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief check to see if the point passed in is within the frustum
  /// @param _p the point to check
  /// @returns the result of the test (inside outside intercept)
  //----------------------------------------------------------------------------------------------------------------------
  CameraIntercept isPointInFrustum(const ngl::Vec3 &_p) const noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief check to see if the sphere passed in is within the frustum
  /// @param[in] _p the center of the sphere
  /// @param[in] _radius the radius of the sphere
  /// @returns the result of the test (inside outside intercept)
  //----------------------------------------------------------------------------------------------------------------------
  CameraIntercept isSphereInFrustum(const ngl::Vec3 &_p, ngl::Real _radius) const noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief check to see if the AABB passed in is within the frustum
  /// @param[in] AABB to test
  /// @returns the result of the test (inside outside intercept)
  //----------------------------------------------------------------------------------------------------------------------
  CameraIntercept boxInFrustum(const ngl::AABB &b) const noexcept;

protected :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief function used to set the perspective projection matrix values
  //----------------------------------------------------------------------------------------------------------------------
  void setPerspProjection() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief internal function to calculate the  new rotation vectors for the camera after a roll, pitch or yaw
  /// @param[in,out] io_a the first vector to be rotated
  /// @param[in,out] io_b the second vector to be rotated
  /// @param[in] _angle the angle to rotate
  //----------------------------------------------------------------------------------------------------------------------
  void rotAxes(ngl::Vec4& io_a, ngl::Vec4& io_b, const ngl::Real _angle) noexcept;

protected :

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief vector for the  Camera local cord frame
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 m_u;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief vector for the  Camera local cord frame
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 m_v;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief vector for the  Camera local cord frame
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 m_n;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  the position of the Camera  used to calculate the local cord frame
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 m_eye;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  where the camera is looking to,  used with Eye to calculate the Vector m_n
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 m_look;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  gives a general indication of which way up the camera is
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec4 m_up;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief  the width of the display image used for some perspective projection calculations
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real m_width;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the height of the display image used for some perspective projection calculations
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real m_height;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the current camera aspect ratio (can be derived from Width / Height
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real m_aspect;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the near clipping plane of the camera view volume
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real m_zNear;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the far clipping plane of the camera view volume
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real m_zFar;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the feild of view of the camera view volume, measured from the eye
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Real m_fov;
  //----------------------------------------------------------------------------------------------------------------------
  ///  @brief a Matrix to hold  the combined modelling and viewing matrix to load into OpenGL
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_viewMatrix;
  //----------------------------------------------------------------------------------------------------------------------
  ///  @brief array of planes for fustrum
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Plane m_planes[6];
  //----------------------------------------------------------------------------------------------------------------------
  ///  @brief index values for the planes array
  //----------------------------------------------------------------------------------------------------------------------
  enum class ProjPlane : char { TOP = 0,BOTTOM,LEFT,RIGHT,NEARP,FARP};
  //----------------------------------------------------------------------------------------------------------------------
  ///  @brief points for the fustrum drawing
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_ntl,m_ntr,m_nbl,m_nbr,m_ftl,m_ftr,m_fbl,m_fbr;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Projection a Matrix to hold the perspective transfomatio matrix for the camera
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_projectionMatrix;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to set the modelview matrix values for the current  camera, this method load the matrix Modelview into OpenGL
  //----------------------------------------------------------------------------------------------------------------------
  void setViewMatrix() noexcept;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief set the projection matrix
  //----------------------------------------------------------------------------------------------------------------------
  void setProjectionMatrix() noexcept;

};

#endif // end of  class

//----------------------------------------------------------------------------------------------------------------------

