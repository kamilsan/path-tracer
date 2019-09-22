#include "camera.hpp"
#include "core.hpp"

Camera::Camera(float fov, const Vector& pos, const Vector& forward, const Vector& up): position(pos)
{
  m_tanhalfFOV = tan(fov * M_PI / 360.0);
  m_forward = forward;
  m_forward.normalize();
  m_right = up.cross(m_forward).normalize();
  m_up = m_forward.cross(m_right).normalize();
}

Ray Camera::getCameraRay(float x, float y) const
{
  //           [rx][ry][rz]
  //[x][y][1]  [ux][uy][uz]
  //           [fx][fy][fz]
  //x' = xrx + yux + fx
  //y' = xry + yuy + fy
  //z' = xyz + yuz + fz

  x *= m_tanhalfFOV;
  y *= m_tanhalfFOV;

  Vector dir = Vector(x * m_right.x + y * m_up.x + m_forward.x,
                      x * m_right.y + y * m_up.y + m_forward.y,
                      x * m_right.z + y * m_up.z + m_forward.z).normalize();
  return Ray(position.clone(), dir);
}