#include "rectangle.hpp"
#include "core.hpp"

void Rectangle::setSizeTangent(float sizeTangent)
{
  m_sizeTangent = sizeTangent;
  m_invPDF = m_sizeTangent * m_sizeBitangent;
}

void Rectangle::setSizeBitangent(float sizeBitangent)
{
  m_sizeBitangent = sizeBitangent;
  m_invPDF = m_sizeTangent * m_sizeBitangent;
}

float Rectangle::intersect(const Ray& ray) const
{
  float don = ray.direction.dot(m_normal);
  if(don > -0.00001f && don < 0.00001f) return -1;
  float t = -(ray.origin - point).dot(m_normal) / don;
  if(t < 0.0f) return -1;

  Vector relPoint = ray(t) - point;
  float distT = relPoint.dot(m_tangent);
  float distB = relPoint.dot(m_bitangent);

  if(distT < 0.0f || distT > m_sizeTangent || distB < 0.0f || distB > m_sizeBitangent) return -1;

  return t;
}

Vector Rectangle::getNormalAt(const Vector&) const
{
  return m_normal;
}

void Rectangle::getUVAt(const Vector& point, float& u, float& v) const
{
  Vector relPoint = point - this->point;
  float distT = relPoint.dot(m_tangent);
  float distB = relPoint.dot(m_bitangent);
  u = distT / m_sizeTangent;
  v = distB / m_sizeBitangent;
}

Vector Rectangle::getSample(RNG& rng) const
{
  return point + m_tangent*rng.get()*m_sizeTangent + m_bitangent*rng.get()*m_sizeBitangent;
}

void Rectangle::getSamples(RNG& rng, int s1, int s2, Vector* samples) const
{
  float invS1 = 1.0f/s1;
  float invS2 = 1.0f/s2;
  float r1, r2;
  int i = 0;
  for(int ny = 0; ny < s2; ++ny)
  {
    for(int nx = 0; nx < s1; ++nx)
    {
      r1 = (nx + rng.get())*invS1;
      r2 = (ny + rng.get())*invS2;
      samples[i++] = point + m_tangent*r1*m_sizeTangent + m_bitangent*r2*m_sizeBitangent;
    }
  }
}