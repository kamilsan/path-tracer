#pragma once

#include <vector>
#include <memory>

class Object;
class Light;
class Ray;

#include "environmentMap.hpp"

class Scene
{
private:
  std::vector<std::shared_ptr<Object>> m_objects;
  std::vector<std::shared_ptr<Light>> m_lights;
  EnvironmentMap m_envMap;
public:
  Scene(): m_objects(std::vector<std::shared_ptr<Object>>()), m_lights(std::vector<std::shared_ptr<Light>>()), m_envMap(EnvironmentMap()) {}
  Scene(const EnvironmentMap& envMap): m_objects(std::vector<std::shared_ptr<Object>>()), m_lights(std::vector<std::shared_ptr<Light>>())
  {
    m_envMap = envMap;
  }

  void addObject(std::shared_ptr<Object> object) { m_objects.push_back(object); }
  void addLight(std::shared_ptr<Light> light) { m_lights.push_back(light); }
  void setEnvironmentMap(const EnvironmentMap& envMap) { m_envMap = envMap; }

  std::shared_ptr<Object> intersect(const Ray& ray, float* closestT) const;
  bool occlusionTest(const Ray& ray, float maxT) const;

  std::vector<std::shared_ptr<Object>> getObjects() const { return m_objects; }
  std::vector<std::shared_ptr<Light>> getLights() const { return m_lights; }
  const EnvironmentMap& getEnvironmentMap() const { return m_envMap; }
};
