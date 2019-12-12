#pragma once

#include "brdf.hpp"

class LambertBRDF : public BRDF
{
public:
  float diffuseFactor;
  LambertBRDF(): diffuseFactor(0.2) {}
  LambertBRDF(float diffuseFactor): diffuseFactor(diffuseFactor) {}

  float f(const Vector&, const Vector&) const override;
  float sample_f(const Vector&, Vector& wi, RNG& rng, float& pdf) const override;
};