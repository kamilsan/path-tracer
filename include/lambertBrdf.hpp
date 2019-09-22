#pragma once

#include "brdf.hpp"

class LambertBRDF : public BRDF
{
public:
  float diffuseFactor;
  LambertBRDF(): diffuseFactor(0.2) {}
  LambertBRDF(float diffuseFactor): diffuseFactor(diffuseFactor) {}

  float f(const Vector& wo, const Vector& wi) const override;
  float sample_f(const Vector& wo, Vector& wi, RNG& rng, float& pdf) const override;
};