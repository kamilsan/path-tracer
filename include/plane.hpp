#pragma once

#include "object.hpp"
#include "vector.hpp"

class BaseMaterial;
class Ray;
class RNG;

class Plane : public Object
{
public:
  Vector point;
  Vector normal;

  Plane(const Vector& pt, const Vector& nor): Object(), point(pt), normal(nor) {}
  Plane(const Vector& pt, const Vector& nor, std::shared_ptr<BaseMaterial> mat): Object(mat), point(pt), normal(nor) {}

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector&) const override;
  void getUVAt(const Vector&, float& u, float& v) const override;
  bool isFinite() const override { return false; }
  Vector getSample(RNG&) const override { return Vector(0,0,0); }
  void getSamples(RNG&, int s1, int s2, Vector* samples) const override
  {
    for(int i = 0; i < s1*s2; ++i)
    {
      samples[i] = Vector(0,0,0);
    }
  }
  float getInversePDF() const override { return -1; }
};
