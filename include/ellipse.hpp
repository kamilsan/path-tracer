#pragma once

#include "object.hpp"
#include "vector.hpp"

class BaseMaterial;
class Ray;
class RNG;

class Ellipse : public Object
{
private:
  Vector m_normal;
  Vector m_axisT;
  Vector m_axisB;
  float m_invPDF;
  float m_semiTangent, m_semiBitangent;
public:
  Vector center;

  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, const Vector& bitangent, float semiTangent, float semiBitangent): Object(), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, const Vector& bitangent, float semiTangent, float semiBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, float semiTangent, float semiBitangent): Object(), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent);
  }
  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, float semiTangent, float semiBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent);
  }

  void resetAxis(const Vector& normal, const Vector& tangent, const Vector& bitangent)
  {
    m_normal = normal;
    m_axisT = tangent;
    m_axisB = bitangent;
    m_normal.normalize();
    m_axisT.normalize();
    m_axisB.normalize();
  }
  void resetAxis(const Vector& normal, const Vector& tangent)
  {
    m_axisT = tangent;
    m_axisT.normalize();
    m_normal = normal;
    m_normal.normalize();
    m_axisB = m_normal.cross(m_axisT).normalize();
    m_normal = m_axisT.cross(m_axisB).normalize();
  }

  float getSemiTangent() const { return m_semiTangent; }
  float getSemiBitangent() const { return m_semiBitangent; }
  void setSemiTangent(float semiTangent);
  void setSemiBitangent(float semiBitangent);

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector&) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return true; }
  Vector getSample(RNG& rng) const override;
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override;
  float getInversePDF() const override { return m_invPDF; }
};
