#include "renderer.hpp"

#include "object.hpp"
#include "baseMaterial.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "utils.hpp"
#include "light.hpp"
#include "brdf.hpp"

void Renderer::render(const Scene& scene, const Camera& camera, char* &pixels)
{
  if(pixels) delete[] pixels;

  int len = 3*m_width*m_height;
  pixels = new char[len];
  float *data = new float[len];

  std::vector<std::shared_ptr<Object>> objects = scene.getObjects();
  std::vector<std::shared_ptr<Object>> areaLights;

  for(size_t i = 0; i < objects.size(); ++i)
  {
    if(objects[i]->material->isEmissive() && objects[i]->isFinite())
      areaLights.push_back(objects[i]);
  }

  Vector color;
  int i;
  float samples_factor = 1.0f/MC_SAMPLES;
  int s1 = std::sqrt(LIGHT_SAMPLES);
  int s2 = LIGHT_SAMPLES/s1;
  //Tone mapping
  float luma;
  float Lavg = 0, Lmax = 0, a = 0.21;
  for(unsigned int y = 0; y < m_height; ++y)
  {
    for(unsigned int x = 0; x < m_width; ++x)
    {
      color = Vector(0,0,0);
      for(unsigned int n = 0; n < MC_SAMPLES; ++n)
        color += sample(x, y, scene, areaLights, camera, s1, s2);
      color *= samples_factor;

      luma = color.dot(Vector(0.2126, 0.7152, 0.0722));
      Lavg += std::log(luma + 0.000001);
      if(luma > Lmax) Lmax = luma;

      i = 3*(y * m_width + x);
      data[i] = color.x;
      data[i+1] = color.y;
      data[i+2] = color.z;

      if(i % 500 == 0) std::cout << 100.0 * i / len << "%\n";
    }
  }

  Lavg = std::exp(Lavg/len);
  float L, Lfactor = a/Lavg;
  Lmax *= Lmax;
  for(i = 0; i < len; ++i)
  {
    L = data[i]*Lfactor;
    L *= (1 + L/Lmax)/(1.0+L);
    sRGBEncode(L);
    //+0.5 -- better approx. (lower quantization error)
    pixels[i] = 255*L + 0.5;
  }
}

Vector Renderer::sample(float x, float y, const Scene& scene, const std::vector<std::shared_ptr<Object>>& emissiveObjects, const Camera& camera, unsigned int s1, unsigned int s2)
{
  //[0, w] /w => [0, 1] *2 - 1 => [-1, 1]
  //            this + 0.5 is because we want to hit the middle of the pixel
  //x + jitterX = (x + 0.5) + (m_rng.get() - 0.5f) = x + m_rng.get()
  float rx = (2.0f*((x + m_rng.get()) / m_width) - 1.0f)*m_ar;
  float ry = 1.0f - 2.0f*((y + m_rng.get()) / m_height);

  Ray ray = camera.getCameraRay(rx, ry);

  return traceRay(ray, scene, emissiveObjects, m_rng, s1, s2);
}

Vector Renderer::traceRay(Ray &ray, const Scene &scene, const std::vector<std::shared_ptr<Object>> &areaLights, RNG &rng, unsigned int s1, unsigned int s2)
{
  Vector color;
  Vector intersectionPoint;
  Vector beta(1, 1, 1);
  std::shared_ptr<Object> object = nullptr;

  int nRealSamples = s1*s2;
  Vector *lightSamples = new Vector[nRealSamples];

  std::vector<std::shared_ptr<Light>> lights = scene.getLights();

  for (int bounces = 0;;++bounces)
  {
    float closestT;
    object = scene.intersect(ray, &closestT);

    if(!object) 
    {
      color += beta*scene.getEnvironmentMap().sample(ray.direction);
      break;
    }

    intersectionPoint = ray(closestT);                      
    Vector normal = object->getNormalAt(intersectionPoint);
    float u, v;
    object->getUVAt(intersectionPoint, u, v);

    Vector albedo = object->material->getColor(u, v);
    BRDF* brdf = object->material->getBRDF();

    Vector wo = -ray.direction, wi;

    //Direct illumination
    LightingInformation li;
    for(size_t i = 0; i < lights.size(); ++i)
    {
      lights[i]->getLightingInformation(intersectionPoint, normal, li);
      bool inShadow = scene.occlusionTest(li.shadowRay, li.occlusionLimit);
      if(!inShadow) 
      {
        wi = li.shadowRay.direction;
        color += beta*albedo*brdf->f(wo, wi) * li.diffuseColor * li.attenuation;
      }
    }

    //Area lights
    if(s1 > 0 && s2 > 0)
    {
      Vector samplePoint;
      Vector col;
      Vector normalAtSample;
      std::shared_ptr<Object> aLight;
      Ray shadowRay;
      for(size_t i = 0; i < areaLights.size(); ++i)
      {
        aLight = areaLights[i];
        if(object == aLight) continue;
        aLight->getSamples(rng, s1, s2, lightSamples);
        for(int n = 0; n < nRealSamples; ++n)
        {
          samplePoint = lightSamples[n];
          wi = samplePoint - intersectionPoint;
          float lenSq = wi.lengthSq();
          float limitT = sqrtf(lenSq);
          wi /= limitT;
          shadowRay = Ray(intersectionPoint + wi * 0.0001f, wi);
          if(!scene.occlusionTest(shadowRay, limitT * 0.999f))
          {
            normalAtSample = aLight->getNormalAt(samplePoint);
            float cosLight = saturate((-wi).dot(normalAtSample));
            float cosPoint = saturate(wi.dot(normal));
            float uLight, vLight;
            aLight->getUVAt(samplePoint, uLight, vLight);
            col += (1.0f/lenSq) * cosPoint * cosLight * brdf->f(wo, wi) * aLight->material->getEmittance(uLight, vLight);
          }
        }
        color += beta*albedo*col/(nRealSamples*aLight->getInversePDF());
      }
    }

    color += beta*object->material->getEmittance(u, v);
    
    //Indirect Illumination
    float pdf;
    Vector sample;
    float f = brdf->sample_f(wo, sample, rng, pdf);
    Vector tangent, bitangent;

    if(f < 0.0001 || pdf == 0) break;

    createOrthogonalSystem(normal, tangent, bitangent);
    wi = Vector(
      sample.x * tangent.x + sample.y * normal.x + sample.z * bitangent.x,
      sample.x * tangent.y + sample.y * normal.y + sample.z * bitangent.y,
      sample.x * tangent.z + sample.y * normal.z + sample.z * bitangent.z
    );

    beta *= albedo*f*std::fabs(wi.dot(normal))/pdf;
    ray = Ray(intersectionPoint + wi * 0.0001f, wi);

    //Russian roulette
    if(bounces > 0)
    {
      float q = 0.25;
      if (rng.get() < q) break;
      beta /= 1.0f - q;
    }
  }
  delete[] lightSamples;
  return color;
}