#include "texturedMaterial.hpp"

#include "vector.hpp"
#include "lambertBrdf.hpp"

TexturedMaterial::TexturedMaterial(): BaseMaterial(), m_emittanceIntensity(0), m_emissive(false), texture(Vector(1, 1, 1)), emittance(Vector(0,0,0))
{
  brdf = new LambertBRDF();
}

TexturedMaterial::TexturedMaterial(const Vector& col, float kDiffuse): BaseMaterial(), m_emittanceIntensity(0), m_emissive(false), texture(col), emittance(Vector(0,0,0))
{
  brdf = new LambertBRDF(kDiffuse);
}

TexturedMaterial::TexturedMaterial(const Vector& col, float kDiffuse, const Vector& emit, float emittanceIntensity): BaseMaterial(), m_emittanceIntensity(emittanceIntensity), texture(col), emittance(emit)
{
  brdf = new LambertBRDF(kDiffuse);
  m_emissive = (emittanceIntensity * emit.lengthSq()) > 0.1;
}

TexturedMaterial::TexturedMaterial(const Texture& tex, float kDiffuse): BaseMaterial(), m_emittanceIntensity(0), m_emissive(false), texture(tex), emittance(Vector(0,0,0))
{
  brdf = new LambertBRDF(kDiffuse);
}

TexturedMaterial::TexturedMaterial(const Texture& tex, float kDiffuse, const Vector& emit, float emittanceIntensity): BaseMaterial(), m_emittanceIntensity(emittanceIntensity), texture(tex), emittance(emit)
{
  brdf = new LambertBRDF(kDiffuse);
  m_emissive = (emittanceIntensity * emit.lengthSq()) > 0.1;
}

TexturedMaterial::TexturedMaterial(const Texture& tex, float kDiffuse, const Texture& emit, float emittanceIntensity): BaseMaterial(),  m_emittanceIntensity(emittanceIntensity), texture(tex), emittance(emit)
{
  brdf = new LambertBRDF(kDiffuse);
  m_emissive = emittanceIntensity > 0.1;
}

TexturedMaterial::~TexturedMaterial()
{
  delete brdf;
}

float TexturedMaterial::getEmittanceIntensity() const { return m_emittanceIntensity; }
void TexturedMaterial::setEmittanceIntensity(float emitIntensity) { m_emittanceIntensity = std::max(emitIntensity, 0.0f); m_emissive = m_emittanceIntensity > 0.1; }

Vector TexturedMaterial::getColor(float u, float v) const { return texture.sample(u, v); }
Vector TexturedMaterial::getEmittance(float u, float v) const { return m_emittanceIntensity*emittance.sample(u, v); }
bool TexturedMaterial::isEmissive() const { return m_emissive; }
BRDF* TexturedMaterial::getBRDF() const { return brdf; }