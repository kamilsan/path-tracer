#pragma once

#include "vector.hpp"
#include "core.hpp"

struct LightingInformation
{
  Vector diffuseColor;
  float attenuation;
  Ray shadowRay;
  float occlusionLimit;
};

class Light
{
public:
  Vector color;
  float intensity;

  Light(): color(Vector(1, 1, 1)), intensity(1) {}
  Light(const Vector& col, float inten): color(col), intensity(inten) {}
  virtual ~Light() {}

  virtual void getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const = 0;
};