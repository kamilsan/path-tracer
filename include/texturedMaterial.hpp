#pragma once

class Vector;
class LambertBRDF;

#include "baseMaterial.hpp"
#include "texture.hpp"

class TexturedMaterial : public BaseMaterial
{
private:
  float m_emittanceIntensity;
  bool m_emissive;
public:
  Texture texture;
  Texture emittance;
  LambertBRDF* brdf;

  TexturedMaterial();
  TexturedMaterial(const Vector& col, float kDiffuse);
  TexturedMaterial(const Vector& col, float kDiffuse, const Vector& emit, float emittanceIntensity);
  TexturedMaterial(const Texture& tex, float kDiffuse);
  TexturedMaterial(const Texture& tex, float kDiffuse, const Vector& emit, float emittanceIntensity);
  TexturedMaterial(const Texture& tex, float kDiffuse, const Texture& emit, float emittanceIntensity);

  ~TexturedMaterial();

  float getEmittanceIntensity() const;
  void setEmittanceIntensity(float emitIntensity);

  Vector getColor(float u, float v) const override;
  Vector getEmittance(float u, float v) const override;
  bool isEmissive() const override;
  BRDF* getBRDF() const override;
};