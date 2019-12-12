#pragma once

#include "object.hpp"
#include "vector.hpp"

class BaseMaterial;
class Ray;
class RNG;

class Rectangle : public Object
{
private:
  Vector m_normal;
  Vector m_tangent;
  Vector m_bitangent;
  float m_invPDF;
  float m_sizeTangent, m_sizeBitangent;
public:
  Vector point;

  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, const Vector& bitangent, float sizeTangent, float sizeBitangent): Object(), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, const Vector& bitangent, float sizeTangent, float sizeBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, float sizeTangent, float sizeBitangent): Object(), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent);
  }
  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, float sizeTangent, float sizeBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent);
  }

  void resetAxis(const Vector& normal, const Vector& tangent, const Vector& bitangent)
  {
    m_normal = normal;
    m_tangent = tangent;
    m_bitangent = bitangent;
    m_normal.normalize();
    m_tangent.normalize();
    m_bitangent.normalize();
  }
  void resetAxis(const Vector& normal, const Vector& tangent)
  {
    m_tangent = tangent;
    m_tangent.normalize();
    m_normal = normal;
    m_normal.normalize();
    m_bitangent = m_normal.cross(m_tangent).normalize();
    m_normal = m_tangent.cross(m_bitangent).normalize();
  }

  float getSizeTangent() const { return m_sizeTangent; }
  float getSizeBitangent() const { return m_sizeBitangent; }
  void setSizeTangent(float sizeTangent);
  void setSizeBitangent(float sizeBitangent);

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector&) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return true; }
  Vector getSample(RNG& rng) const override;
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override;
  float getInversePDF() const override { return m_invPDF; }
};
