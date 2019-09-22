#pragma once

#include "object.hpp"
#include "vector.hpp"

class Sphere : public Object
{
private:
  float m_invPDF;
  float m_radius;
public:
  Vector center;

  Sphere(const Vector& c, const float radius);
  Sphere(const Vector& c, const float radius, std::shared_ptr<BaseMaterial> mat);

  float getRadius() const { return m_radius; }
  void setRadius(float radius);

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector& point) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return true; }
  Vector getSample(RNG& rng) const override;
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override;
  float getInversePDF() const override { return m_invPDF; }
};
