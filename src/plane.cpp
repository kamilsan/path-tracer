#include "plane.hpp"
#include "core.hpp"

float Plane::intersect(const Ray& ray) const
{
  float don = ray.direction.dot(this->normal);
  if(don > -0.00001f && don < 0.00001f) return -1;

  return -(ray.origin - point).dot(this->normal) / don;
}

Vector Plane::getNormalAt(const Vector& point) const
{
  return normal;
}

void Plane::getUVAt(const Vector& point, float& u, float& v) const
{
  u = 0;
  v = 0;
}