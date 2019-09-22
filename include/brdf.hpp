#pragma once

class Vector;
class RNG;

class BRDF
{
public:
  virtual float f(const Vector& wo, const Vector& wi) const = 0;
  virtual float sample_f(const Vector& wo, Vector& wi, RNG& rng, float& pdf) const = 0;
  virtual ~BRDF() {};
};
