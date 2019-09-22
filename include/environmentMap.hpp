#pragma once

#include <memory>
#include "texture.hpp"

class Vector;

class EnvironmentMap
{
private:
  Texture m_texture;
public:
  EnvironmentMap();
  EnvironmentMap(const Texture& texture): m_texture(texture) {}
  EnvironmentMap(const Vector& color): m_texture(color) {}
  EnvironmentMap(const char* fileName): m_texture(fileName) {}

  Vector sample(const Vector& v) const;
  bool isValid() const { return m_texture.isValid(); }
};
