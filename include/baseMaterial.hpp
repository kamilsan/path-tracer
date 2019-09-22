#pragma once

class BRDF;
class Vector;

class BaseMaterial
{
public:
  virtual Vector getColor(float u, float v) const = 0;
  virtual Vector getEmittance(float u, float v) const = 0;
  virtual bool isEmissive() const = 0;
  virtual BRDF* getBRDF() const = 0;
};