#pragma once

#include "light.hpp"

class PointLight : public Light
{
public:
  Vector position;

  PointLight(const Vector& pos): Light(), position(pos) {}
  PointLight(const Vector& pos, const Vector& col, float inten): Light(col, inten), position(pos) {}

  void getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const;
};