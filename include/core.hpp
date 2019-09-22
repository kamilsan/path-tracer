#pragma once

#include "vector.hpp"

#include <chrono>
#include <random>

class Ray
{
public:
  Vector origin, direction;

  Ray(): origin(Vector(0, 0, 0)), direction(Vector(0, 0, 1)) {}
  Ray(const Vector& orig, const Vector& dir): origin(orig), direction(dir) {}

  Vector operator()(float t) const { return origin + t*direction; };
};

class RNG
{
private:
  std::mt19937 mt;
  std::uniform_real_distribution<float> dist;
public:
  RNG()
  {
    mt.seed(std::chrono::system_clock::now().time_since_epoch().count());
  }
  RNG(unsigned seed)
  {
    mt.seed(seed);
  }

  float get() { return dist(mt); }
};