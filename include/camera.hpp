#pragma once

#include "vector.hpp"

class Ray;

class Camera
{
private:
  Vector m_forward, m_up, m_right;
  float m_tanhalfFOV;
public:
  Vector position;

  Camera(float fov, const Vector& pos, const Vector& forward, const Vector& up);
  Camera(float fov, const Vector& pos): m_forward(Vector(0, 0, 1)), m_up(Vector(0, 1, 0)), m_right(Vector(1, 0, 0)), position(pos)
  {
    m_tanhalfFOV = tan(fov * M_PI / 360.0);
  }
  Camera(): m_forward(Vector(0, 0, 1)), m_up(Vector(0, 1, 0)), m_right(Vector(1, 0, 0)), m_tanhalfFOV(1.0f), position(Vector(0, 0, -1)) {}

  Ray getCameraRay(float x, float y) const;
};
