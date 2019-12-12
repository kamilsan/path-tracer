#pragma once

#include "baseMaterial.hpp"
#include "vector.hpp"

class LambertBRDF;

class SolidMaterial : public BaseMaterial
{
public:
  Vector color;
  Vector emittance;
  LambertBRDF* brdf;
  float diffuseFactor;

  SolidMaterial();
  SolidMaterial(const Vector& col, float kDiffuse);
  SolidMaterial(const Vector& col, float kDiffuse, const Vector& emit);

  ~SolidMaterial();

  Vector getColor(float, float) const override;
  Vector getEmittance(float, float) const override;
  bool isEmissive() const override;
  BRDF* getBRDF() const override;
};