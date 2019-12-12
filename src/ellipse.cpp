#include "ellipse.hpp"
#include "core.hpp"

void Ellipse::setSemiTangent(float semiTangent)
{
  m_semiTangent = semiTangent;
  m_invPDF = M_PI * m_semiTangent * m_semiBitangent;
}

void Ellipse::setSemiBitangent(float semiBitangent)
{
  m_semiBitangent = semiBitangent;
  m_invPDF = M_PI * m_semiTangent * m_semiBitangent;
}

float Ellipse::intersect(const Ray& ray) const
{
  float don = ray.direction.dot(m_normal);
  if(don > -0.00001f && don < 0.00001f) return -1;
  float t = -(ray.origin - center).dot(m_normal) / don;
  if(t < 0.0f) return -1;

  Vector relPoint = ray(t) - center;
  float distT = relPoint.dot(m_axisT);
  float distB = relPoint.dot(m_axisB);
  distT *= (distT / (m_semiTangent*m_semiTangent));
  distB *= (distB / (m_semiBitangent*m_semiBitangent));

  if(distT + distB > 1.0f) return -1;

  return t;
}

Vector Ellipse::getNormalAt(const Vector&) const
{
  return m_normal;
}

void Ellipse::getUVAt(const Vector& point, float& u, float& v) const
{
  Vector relPoint = point - center;
  float distT = relPoint.dot(m_axisT);
  float distB = relPoint.dot(m_axisB);
  u = (distT / 2*m_semiTangent) + 0.5;
  v = (distB / 2*m_semiBitangent) + 0.5;
}

Vector Ellipse::getSample(RNG& rng) const
{
  float r = sqrtf(rng.get());
  float theta = 2.0f*M_PI*rng.get();
  float x = r*cosf(theta)*m_semiTangent;
  float y = r*sinf(theta)*m_semiBitangent;
  return x*m_axisT + y*m_axisB + center;
}

void Ellipse::getSamples(RNG& rng, int s1, int s2, Vector* samples) const
{
  float invS1 = 1.0f/s1;
  float invS2 = 1.0f/s2;
  float r1, r2, r, theta, x, y;
  int i = 0;
  for(int ny = 0; ny < s2; ++ny)
  {
    for(int nx = 0; nx < s1; ++nx)
    {
      r1 = (nx + rng.get())*invS1;
      r2 = (ny + rng.get())*invS2;
      r = sqrt(r1);
      theta = 2.0f*M_PI*r2;
      x = r*cosf(theta)*m_semiTangent;
      y = r*sinf(theta)*m_semiBitangent;
      samples[i++] = x*m_axisT + y*m_axisB + center;
    }
  }
}