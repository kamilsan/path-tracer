#include <iostream>

#include "vector.hpp"

Vector& Vector::normalize()
{
  float len = length();
  if(len > 0.00001f)
  {
    float f = 1.0f/len;
    x *= f;
    y *= f;
    z *= f;
  }
  return *this;
}

Vector& Vector::clamp(float min = 0.0f, float max = 1.0f)
{
  if(x < min) x = min;
  else if(x > max) x = max;
  if(y < min) y = min;
  else if(y > max) y = max;
  if(z < min) z = min;
  else if(z > max) z = max;
  return *this;
}