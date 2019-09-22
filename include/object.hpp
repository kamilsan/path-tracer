#pragma once

#include <memory>

class BaseMaterial;
class Vector;
class RNG;
class Ray;

class Object
{
public:
  std::shared_ptr<BaseMaterial> material;

  Object();
  Object(std::shared_ptr<BaseMaterial> mat): material(mat) {}

  virtual float intersect(const Ray& ray) const = 0;
  virtual Vector getNormalAt(const Vector &point) const = 0;
  virtual void getUVAt(const Vector &point, float& u, float& v) const = 0;
  virtual bool isFinite() const = 0;
  virtual Vector getSample(RNG& rng) const = 0;
  virtual void getSamples(RNG& rng, int s1, int s2, Vector* samples) const = 0;
  virtual float getInversePDF() const = 0;
};