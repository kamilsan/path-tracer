#pragma once

#include <vector>
#include <memory>
#include "core.hpp"

class Scene;
class Object;
class Camera;

class Renderer
{
private:
  unsigned int m_width, m_height;
  float m_ar;
  RNG m_rng;

  Vector sample(float x, float y, const Scene& scene, const std::vector<std::shared_ptr<Object>> &emissiveObjects, const Camera& camera, unsigned int s1, unsigned int s2);
public:
  unsigned int MC_SAMPLES, LIGHT_SAMPLES;

  Renderer(unsigned int width, unsigned int height): m_width(width), m_height(height)
  {
    m_ar = (float)width / height;

    MC_SAMPLES = 16;
    LIGHT_SAMPLES = 8;
  }

  void reset(unsigned int width, unsigned int height)
  {
    m_width = width;
    m_height = height;
    m_ar = (float)width / height;
  }

  unsigned int getWidth() { return m_width; }
  unsigned int getHeight() { return m_height; }

  void setWidth(unsigned int width)
  {
    m_width = width;
    m_ar = (float)width/m_height;
  }
  void setHeight(unsigned int height)
  {
    m_height = height;
    m_ar = (float)m_width/height;
  }

  Vector traceRay(Ray &ray, const Scene &scene, const std::vector<std::shared_ptr<Object>> &areaLights, RNG &rng, unsigned int s1, unsigned int s2);
  void render(const Scene& scene, const Camera& camera, char* &pixels);
};
