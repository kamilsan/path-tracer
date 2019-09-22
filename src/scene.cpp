
#include "scene.hpp"
#include "object.hpp"

std::shared_ptr<Object> Scene::intersect(const Ray& ray, float* intersectionT) const
{
  std::shared_ptr<Object> object = nullptr;
  float closestT = 10e6;
  float t;
  for(size_t i = 0; i < m_objects.size(); ++i)
  {
    t = m_objects[i]->intersect(ray);
    if(t > 0.0f && t < closestT)
    {
      object = m_objects[i];
      closestT = t;
    }
  }
  *intersectionT = closestT;
  return object;
}

bool Scene::occlusionTest(const Ray& ray, float maxT) const
{
  float t;
  for(size_t i = 0; i < m_objects.size(); ++i)
  {
    t = m_objects[i]->intersect(ray);
    if(t > 0.0f && (t < maxT || maxT < 0.0f))
      return true;
  }
  return false;
}