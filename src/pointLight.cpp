#include "pointLight.hpp"
#include "utils.hpp"

void PointLight::getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const
{
  Vector toLight = (position - point);
  float len = toLight.length();
  toLight /= len;

  float diffuseFactor = saturate(toLight.dot(normal)) * intensity;
  li.diffuseColor = color * diffuseFactor;
  li.attenuation =  1 / (len * len + 1);
  li.shadowRay = Ray(point + toLight * 0.0001f, toLight);
  li.occlusionLimit = len;
}