#include "sphere.hpp"

#include "core.hpp"

Sphere::Sphere(const Vector& c, const float radius): Object(), m_radius(radius), center(c)
{
  m_invPDF = 4.0f * radius*radius * M_PI;
}

Sphere::Sphere(const Vector& c, const float radius, std::shared_ptr<BaseMaterial> mat): Object(mat), m_radius(radius), center(c)
{
  m_invPDF = 4.0f * radius*radius * M_PI;
}

void Sphere::setRadius(float radius)
{
  m_radius = radius;
  m_invPDF = 4.0f * radius*radius * M_PI;
}

float Sphere::intersect(const Ray& ray) const
{
  Vector centerToOrigin = ray.origin - center;
  float b = 2.0f * ray.direction.dot(centerToOrigin);
  float c = centerToOrigin.dot(centerToOrigin) - m_radius * m_radius;
  float delta = b*b - 4.0f*c;
  if(delta < 0.0f)
    return -1.0f;

  delta = sqrt(delta);
  float t1 = (-b - delta) * 0.5f;
  float t2 = (-b + delta) * 0.5f;

  return (t1 < t2 ? t1 : t2);
}

Vector Sphere::getNormalAt(const Vector& point) const
{
  return (point - center) / m_radius;
}

void Sphere::getUVAt(const Vector& point, float& u, float& v) const
{
  Vector rp = point - center;
  float theta = acos(rp.y/m_radius);
  float phi = atan2(rp.z, rp.x);
  u = phi * 0.5 * M_1_PI + 0.5;
  v = theta * M_1_PI;
}

Vector Sphere::getSample(RNG& rng) const
{
  float cosT = 2.0f*rng.get() - 1.0f;
  float sinT = sqrtf(1.0f - cosT*cosT);
  float phi = 2.0f * M_PI * rng.get();
  return center + Vector(m_radius * sinT * cosf(phi), m_radius * cosT, m_radius * sinT * sinf(phi));
}

void Sphere::getSamples(RNG& rng, int s1, int s2, Vector* samples) const
{
  float invS1 = 1.0f/s1;
  float invS2 = 1.0f/s2;
  float r1, r2, cosT, sinT, phi;
  int i = 0;
  for(int ny = 0; ny < s2; ++ny)
  {
    for(int nx = 0; nx < s1; ++nx)
    {
      r1 = (nx + rng.get())*invS1;
      r2 = (ny + rng.get())*invS2;
      cosT = 2.0f*r1 - 1.0f;
      sinT = sqrtf(1.0f - cosT*cosT);
      phi = 2.0f * M_PI * r2;
      samples[i++] = center + Vector(m_radius * sinT * cosf(phi), m_radius * cosT, m_radius * sinT * sinf(phi));
    }
  }
}