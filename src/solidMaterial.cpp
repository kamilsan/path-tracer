#include "solidMaterial.hpp"
#include "lambertBrdf.hpp"

SolidMaterial::SolidMaterial(): BaseMaterial(), color(Vector(1, 1, 1)), emittance(Vector(0,0,0)) 
{
  brdf = new LambertBRDF();
}

SolidMaterial::SolidMaterial(const Vector& col, float kDiffuse): BaseMaterial(), color(col), emittance(Vector(0,0,0)) 
{
  brdf = new LambertBRDF(kDiffuse);
}

SolidMaterial::SolidMaterial(const Vector& col, float kDiffuse, const Vector& emit): BaseMaterial(), color(col), emittance(emit) 
{
  brdf = new LambertBRDF(kDiffuse);
}

SolidMaterial::~SolidMaterial()
{
  delete brdf;
}

Vector SolidMaterial::getColor(float, float) const { return color; }
Vector SolidMaterial::getEmittance(float, float) const { return emittance; }
bool SolidMaterial::isEmissive() const { return emittance.lengthSq() > 0.1; }
BRDF* SolidMaterial::getBRDF() const { return brdf; }