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
#include "Camera.h"
#include <ngl/Util.h>
#include <ngl/NGLassert.h>
#include <ngl/VAOFactory.h>
#include <ngl/SimpleVAO.h>
#include <ngl/RibExport.h>
#include <vector>
#include <ngl/Vec3.h>
#include <iostream>
#include <cmath>
#include <memory>
//----------------------------------------------------------------------------------------------------------------------
/// @file Camera.cpp
/// @brief implementation files for Camera class
//----------------------------------------------------------------------------------------------------------------------
// a lot of this stuff is from the HILL book Computer Graphics with OpenGL 2nd Ed Prentice Hall
// a very good book
constexpr ngl::Real CAMERANEARLIMIT = 0.00001f;

//----------------------------------------------------------------------------------------------------------------------

Camera::Camera() noexcept
{

  m_zNear = 0.0001f;
  m_zFar = 350.0f;
  m_aspect = 720.0f / 576.0f;
  m_fov = 45.0f;
  m_width = 720;
  m_height = 576;
  m_eye.set(1, 1, 1);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera ::setDefaultCamera() noexcept
{
  // make default camera
  m_eye.set(1.0f,1.0f,1.0f);
  m_look.set(0.0f,0.0f,0.0f);
  m_up.set(ngl::Vec4::up());
  m_fov = 45.0;
  m_zNear = 0.0001f;
  m_zFar = 350.0f;
  m_aspect = 720.0f / 576.0f;
  m_fov = 45.0f;

  setShape(m_fov, m_aspect, m_zNear, m_zFar); // good default values here
  set(ngl::Vec3(5.0, 5.0, 5.0), ngl::Vec3(0.0, 0.0, 0.0), ngl::Vec3(0, 1, 0));
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::set(const ngl::Vec3 &_eye, const ngl::Vec3 &_look, const ngl::Vec3 &_up) noexcept
{
  // make U, V, N vectors
  m_eye = _eye;
  m_look = _look;
  m_up = _up;
  m_n = m_eye - m_look;
  m_u = m_up.cross(m_n);
  m_v = m_n.cross(m_u);
  m_u.normalize();
  m_v.normalize();
  m_n.normalize();
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
Camera::Camera(const ngl::Vec3 &_eye, const ngl::Vec3 &_look, const ngl::Vec3 &_up) noexcept
{
  setDefaultCamera();
  set(_eye, _look, _up);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setViewMatrix() noexcept
{
  // grab a pointer to the matrix so we can index is quickly
  auto M = &m_viewMatrix.m_m[0][0];
  M[0] = m_u.m_x;
  M[1] = m_v.m_x;
  M[2] = m_n.m_x;
  M[3] = 0.0;
  M[4] = m_u.m_y;
  M[5] = m_v.m_y;
  M[6] = m_n.m_y;
  M[7] = 0.0;
  M[8] = m_u.m_z;
  M[9] = m_v.m_z;
  M[10] = m_n.m_z;
  M[11] = 0.0;
  M[12] = -m_eye.dot(m_u);
  M[13] = -m_eye.dot(m_v);
  M[14] = -m_eye.dot(m_n);
  M[15] = 1.0;

  calculateFrustum();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setPerspProjection() noexcept
{
  // note 1/tan == cotangent
  ngl::Real f = 1.0f / tanf(ngl::radians(m_fov) / 2.0f);
  m_projectionMatrix.identity();

  m_projectionMatrix.m_m[0][0] = f / m_aspect;
  m_projectionMatrix.m_m[1][1] = f;

  m_projectionMatrix.m_m[2][2] = (m_zFar + m_zNear) / (m_zNear - m_zFar);
  m_projectionMatrix.m_m[3][2] = (2 * m_zFar * m_zNear) / (m_zNear - m_zFar);

  m_projectionMatrix.m_m[2][3] = -1.0f;
  m_projectionMatrix.m_m[3][3] = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setProjectionMatrix() noexcept
{
  m_projectionMatrix.null();
  setPerspProjection();
}
//----------------------------------------------------------------------------------------------------------------------

void Camera::setShape(ngl::Real _viewAngle, ngl::Real _aspect, ngl::Real _near, ngl::Real _far) noexcept

{ // load projection matrix and camera values
  if (_viewAngle > 180.0f)
  {
    _viewAngle = 180.0f;
  }
  NGL_ASSERT(_far > _near);
  NGL_ASSERT(_near > CAMERANEARLIMIT);
  if (_near < CAMERANEARLIMIT)
  {
    _near = CAMERANEARLIMIT;
  }
  m_fov = _viewAngle; // viewangle in degrees - must be < 180
  m_aspect = _aspect;
  m_zNear = _near;
  m_zFar = _far;
  setProjectionMatrix();
  // calculateFrustum();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setAspect(ngl::Real _asp) noexcept
{
  m_aspect = _asp;
  setShape(m_fov, m_aspect, m_zNear, m_zFar);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::setViewAngle(ngl::Real _angle) noexcept
{
  m_fov = _angle;
  setShape(_angle, m_aspect, m_zNear, m_zFar);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::slide(ngl::Real _du, ngl::Real _dv, ngl::Real _dn) noexcept
{
  // slide eye by amount du * u + dv * v + dn * n;
  m_eye.m_x += _du * m_u.m_x + _dv * m_v.m_x + _dn * m_n.m_x;
  m_eye.m_y += _du * m_u.m_y + _dv * m_v.m_y + _dn * m_n.m_y;
  m_eye.m_z += _du * m_u.m_z + _dv * m_v.m_z + _dn * m_n.m_z;
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::move(ngl::Real _dx, ngl::Real _dy, ngl::Real _dz) noexcept
{
  // simply add the translation to the current eye point
  m_eye.m_x += _dx;
  m_eye.m_y += _dy;
  m_eye.m_z += _dz;
  setViewMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
void Camera::moveBoth(ngl::Real _dx, ngl::Real _dy, ngl::Real _dz) noexcept
{
  m_eye.m_x += _dx;
  m_eye.m_y += _dy;
  m_eye.m_z += _dz;
  m_look.m_x += _dx;
  m_look.m_y += _dy;
  m_look.m_z += _dz;
  m_n = m_eye - m_look;
  m_u = m_up.cross(m_n);
  m_v.set(m_n.cross(m_u));
  // normalize vectors
  m_u.normalize();
  m_v.normalize();
  m_n.normalize();
  // pass to OpenGL
  setViewMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
void Camera::rotAxes(ngl::Vec4 &io_a, ngl::Vec4 &io_b, const ngl::Real _angle) noexcept
{
  // rotate orthogonal vectors a (like x axis) and b(like y axis) through angle degrees
  // convert to radians
  ngl::Real ang = ngl::radians(_angle);
  // pre-calc cos and sine
  ngl::Real c = cosf(ang);
  ngl::Real s = sinf(ang);
  // tmp for io_a vector
  ngl::Vec4 t(c * io_a.m_x + s * io_b.m_x, c * io_a.m_y + s * io_b.m_y, c * io_a.m_z + s * io_b.m_z);
  // now set to new rot value
  io_b.set(-s * io_a.m_x + c * io_b.m_x, -s * io_a.m_y + c * io_b.m_y, -s * io_a.m_z + c * io_b.m_z);
  // put tmp into _a'
  io_a.set(t.m_x, t.m_y, t.m_z);
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::roll(ngl::Real _angle) noexcept
{
  rotAxes(m_u, m_v, -_angle);
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::pitch(ngl::Real _angle) noexcept
{
  rotAxes(m_n, m_v, _angle);
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::yaw(ngl::Real _angle) noexcept
{
  rotAxes(m_u, m_n, _angle);
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::moveEye(ngl::Real _dx, ngl::Real _dy, ngl::Real _dz) noexcept
{
  m_eye.m_x += _dx;
  m_eye.m_y += _dy;
  m_eye.m_z += _dz;
  m_n = m_eye - m_look;
  m_u.set(m_up.cross(m_n));
  m_v.set(m_n.cross(m_u));
  // normalize the vectors
  m_u.normalize();
  m_v.normalize();
  m_n.normalize();
  // pass to OpenGL
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::moveLook(ngl::Real _dx, ngl::Real _dy, ngl::Real _dz) noexcept
{
  m_look.m_x += _dx;
  m_look.m_y += _dy;
  m_look.m_z += _dz;
  m_n = m_eye - m_look;
  m_u.set(m_up.cross(m_n));
  m_v.set(m_n.cross(m_u));
  // normalise vectors
  m_u.normalize();
  m_v.normalize();
  m_n.normalize();
  // pass to OpenGL
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::update() noexcept
{
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::normalisedYaw(ngl::Real _angle) noexcept
{
  // build a rotation matrix around the y axis
  auto mat = ngl::Mat4::rotateY(_angle);
  // multiply all three local coord vectors by the matrix
  m_u = m_u * mat;
  m_v = m_v * mat;
  m_n = m_n * mat;
  // reset the modelview matrix
  setViewMatrix();
}
//----------------------------------------------------------------------------------------------------------------------
void Camera::normalisedRoll(ngl::Real _angle) noexcept
{
  // build a rotation matrix around the y axis
  auto mat = ngl::Mat4::rotateZ(_angle);
  // multiply all three local coord vectors by the matrix
  m_u = m_u * mat;
  m_v = m_v * mat;
  m_n = m_n * mat;
  // reset the modelview matrix
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
void Camera::normalisedPitch(ngl::Real _angle) noexcept
{
  // build a rotation matrix around the y axis
  auto mat = ngl::Mat4::rotateX(_angle);
  // multiply all three local coord vectors by the matrix
  m_u = m_u * mat;
  m_v = m_v * mat;
  m_n = m_n * mat;
  // reset the modelview matrix
  setViewMatrix();
}

//----------------------------------------------------------------------------------------------------------------------
/// Code modified from http://www.lighthouse3d.com/opengl/viewfrustum/index.php?intro
///
void Camera::calculateFrustum() noexcept
{

  ngl::Real tang = tanf(ngl::radians(m_fov) * 0.5f);
  ngl::Real nh = m_zNear * tang;
  ngl::Real nw = nh * m_aspect;
  ngl::Real fh = m_zFar * tang;
  ngl::Real fw = fh * m_aspect;

  ngl::Vec3 nc = (m_eye - m_n * m_zNear).toVec3();
  ngl::Vec3 fc = (m_eye - m_n * m_zFar).toVec3();

  m_ntl = nc + m_v.toVec3() * nh - m_u.toVec3() * nw;
  m_ntr = nc + m_v.toVec3() * nh + m_u.toVec3() * nw;
  m_nbl = nc - m_v.toVec3() * nh - m_u.toVec3() * nw;
  m_nbr = nc - m_v.toVec3() * nh + m_u.toVec3() * nw;

  m_ftl = fc + m_v.toVec3() * fh - m_u.toVec3() * fw;
  m_ftr = fc + m_v.toVec3() * fh + m_u.toVec3() * fw;
  m_fbl = fc - m_v.toVec3() * fh - m_u.toVec3() * fw;
  m_fbr = fc - m_v.toVec3() * fh + m_u.toVec3() * fw;
  m_planes[static_cast<int>(ProjPlane::TOP)].setPoints(m_ntr, m_ntl, m_ftl);
  m_planes[static_cast<int>(ProjPlane::BOTTOM)].setPoints(m_nbl, m_nbr, m_fbr);
  m_planes[static_cast<int>(ProjPlane::LEFT)].setPoints(m_ntl, m_nbl, m_fbl);
  m_planes[static_cast<int>(ProjPlane::RIGHT)].setPoints(m_nbr, m_ntr, m_fbr);
  m_planes[static_cast<int>(ProjPlane::NEARP)].setPoints(m_ntl, m_ntr, m_nbr);
  m_planes[static_cast<int>(ProjPlane::FARP)].setPoints(m_ftr, m_ftl, m_fbl);
}

void Camera::drawFrustum() noexcept
{
  std::vector<ngl::Vec3> points;

  // draw the sides as lines
  points.push_back(m_ntl);
  points.push_back(m_ftl);

  points.push_back(m_ntr);
  points.push_back(m_ftr);

  points.push_back(m_nbl);
  points.push_back(m_fbl);

  points.push_back(m_nbr);
  points.push_back(m_fbr);
  // near plane lines
  points.push_back(m_ntr);
  points.push_back(m_ntl);
  points.push_back(m_nbr);
  points.push_back(m_nbl);
  points.push_back(m_ntr);
  points.push_back(m_nbr);
  points.push_back(m_ntl);
  points.push_back(m_nbl);
  // far plane lines
  points.push_back(m_ftr);
  points.push_back(m_ftl);
  points.push_back(m_fbr);
  points.push_back(m_fbl);
  points.push_back(m_ftr);
  points.push_back(m_fbr);
  points.push_back(m_ftl);
  points.push_back(m_fbl);

  // now we create a VAO to store the data
  std::unique_ptr<ngl::AbstractVAO> vao(ngl::VAOFactory::createVAO("simpleVAO", GL_LINES));
  // bind it so we can set values
  vao->bind();
  // set the vertex data (4 for x,y,z)
  vao->setData(ngl::SimpleVAO::VertexData(points.size() * sizeof(ngl::Vec3), points[0].m_x));
  // now we set the attribute pointer to be 0 (as this matches vertIn in our shader)
  vao->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(ngl::Vec3), 0);
  // say how many indecis to be rendered
  vao->setNumIndices(points.size());
  vao->draw();
  // now unbind
  vao->unbind();
  vao->removeVAO();
}

CameraIntercept Camera::isPointInFrustum(const ngl::Vec3 &_p) const noexcept
{
  CameraIntercept result = CameraIntercept::INSIDE;
  for (int i = 0; i < 6; ++i)
  {

    if (m_planes[i].distance(_p) < 0)
    {
      return CameraIntercept::OUTSIDE;
    }
  }

  return result;
}

CameraIntercept Camera::isSphereInFrustum(const ngl::Vec3 &_p, ngl::Real _radius) const noexcept
{

  CameraIntercept result = CameraIntercept::INSIDE;
  ngl::Real distance;

  for (int i = 0; i < 6; ++i)
  {
    distance = m_planes[i].distance(_p);
    if (distance < -_radius)
    {
      return CameraIntercept::OUTSIDE;
    }
    else if (distance < _radius)
    {
      result = CameraIntercept::INTERSECT;
    }
  }
  return result;
}

CameraIntercept Camera::boxInFrustum(const ngl::AABB &b) const noexcept
{

  CameraIntercept result = CameraIntercept::INSIDE;
  for (int i = 0; i < 6; i++)
  {

    if (m_planes[i].distance(b.getVertexP(m_planes[i].getNormal())) < 0)
      return CameraIntercept::OUTSIDE;
    else if (m_planes[i].distance(b.getVertexN(m_planes[i].getNormal())) < 0)
      result = CameraIntercept::INTERSECT;
  }
  return (result);
}

/// end citation http://www.lighthouse3d.com/opengl/viewfrustum/index.php?intro
