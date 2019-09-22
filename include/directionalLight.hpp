#pragma once

#include "light.hpp"

class DirectionalLight : public Light
{
public:
  Vector direction;

  DirectionalLight(const Vector& dir): Light(), direction(dir) {}
  DirectionalLight(const Vector& dir, const Vector& col, float inten): Light(col, inten), direction(dir) {}

  void getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const;
};
