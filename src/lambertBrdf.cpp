#include "lambertBrdf.hpp"

#include <cmath>
#include "vector.hpp"
#include "core.hpp"

float LambertBRDF::f(const Vector&, const Vector&) const
{
  return diffuseFactor*M_1_PI;
}

float LambertBRDF::sample_f(const Vector&, Vector& wi, RNG& rng, float& pdf) const
{
  float sinT = sqrtf(rng.get());
  float cosT = sqrtf(1 - sinT * sinT);
  float phi = 2*M_PI*rng.get();
  wi = Vector(sinT * cosf(phi), cosT, sinT * sinf(phi));
  pdf = cosT*M_1_PI;
  return diffuseFactor*M_1_PI;
}