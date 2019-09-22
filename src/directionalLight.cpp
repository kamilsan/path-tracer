#include "directionalLight.hpp"
#include "utils.hpp"

void DirectionalLight::getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const
{
  Vector d = -direction;
  float diffuseFactor = saturate(d.dot(normal)) * intensity;
  li.diffuseColor = color * diffuseFactor;
  li.attenuation = 1;
  li.shadowRay = Ray(point + d * 0.0001f, d);
  li.occlusionLimit = -1;
}