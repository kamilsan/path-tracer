#include "environmentMap.hpp"

#include "vector.hpp"

EnvironmentMap::EnvironmentMap(): m_texture(Vector(0, 0, 0)) {}

Vector EnvironmentMap::sample(const Vector& vec) const
{
  float theta = acos(vec.y);
  float phi = atan2(vec.x, vec.z);
  float u = phi * 0.5 * M_1_PI + 0.5;
  float v = theta * M_1_PI;

  return m_texture.sample(u, v);
}
