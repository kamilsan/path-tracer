#include <iostream>
#include <memory>
#include <chrono>

#include "utils.hpp"
#include "renderer.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "texture.hpp"
#include "solidMaterial.hpp"
#include "texturedMaterial.hpp"
#include "rectangle.hpp"
#include "sphere.hpp"

int main()
{
  int width = 1920, height = 1080;
  char *pixels = nullptr;

  Renderer renderer(width, height);
  renderer.MC_SAMPLES = 32;
  renderer.LIGHT_SAMPLES = 32;

  Camera camera(90, Vector(0, 0, -1), Vector(0, 0, 1), Vector(0, 1, 0));
  Scene scene;
  Texture wallTexture("textures/uv.ppm", true);
  Texture wallTexture2("textures/uv.ppm", false);
  Texture floorTexture("textures/floor.ppm");
  std::shared_ptr<BaseMaterial> wallMaterial1 = std::make_shared<TexturedMaterial>(wallTexture, 0.81f);
  std::shared_ptr<BaseMaterial> wallMaterial2 = std::make_shared<TexturedMaterial>(wallTexture2, 0.81f);
  std::shared_ptr<BaseMaterial> floorMaterial = std::make_shared<SolidMaterial>(Vector(1.0f, 1.0f, 1.0f), 0.81f);
  std::shared_ptr<BaseMaterial> floorMaterial2 = std::make_shared<TexturedMaterial>(floorTexture, 0.81f);
  std::shared_ptr<BaseMaterial> ceilingMaterial = std::make_shared<TexturedMaterial>(floorTexture, 0.0f, wallTexture, 0.9);
  std::shared_ptr<BaseMaterial> lampMaterial = std::make_shared<SolidMaterial>(Vector(1, 1, 1), 0.1, Vector(5, 5, 5));

  scene.addObject(std::make_shared<Rectangle>(Vector(-2, -1, -1), Vector(1, 0, 0), Vector(0, 0, 1), Vector(0, 1, 0), 3, 3, wallMaterial1));
  scene.addObject(std::make_shared<Rectangle>(Vector(2, 2, 2), Vector(-1, 0, 0), Vector(0, 0, -1), Vector(0, -1, 0), 3, 3, wallMaterial2));
  scene.addObject(std::make_shared<Rectangle>(Vector(-2, -1, -1), Vector(0, 1, 0), Vector(1, 0, 0), Vector(0, 0, 1), 4, 3, floorMaterial2));
  scene.addObject(std::make_shared<Rectangle>(Vector(-2, 2, -1), Vector(0, -1, 0), Vector(1, 0, 0), Vector(0, 0, 1), 4, 3, floorMaterial));
  scene.addObject(std::make_shared<Rectangle>(Vector(-2, -1, 2), Vector(0, 0, -1), Vector(1, 0, 0), Vector(0, 1, 0), 4, 3, floorMaterial));
  scene.addObject(std::make_shared<Rectangle>(Vector(-2, -1, -1), Vector(0, 0, 1), Vector(1, 0, 0), Vector(0, 1, 0), 4, 3, floorMaterial));
  scene.addObject(std::make_shared<Sphere>(Vector(-0.8f, -0.5f, 0.8f), 0.5f, floorMaterial));
  scene.addObject(std::make_shared<Sphere>(Vector(0.6f, -0.5f, 0.3f), 0.5f, floorMaterial));

  scene.addObject(std::make_shared<Rectangle>(
    Vector(-0.7f, 2.0f, 1.3f),
    Vector(-1, 0, 0),
    Vector(0, 0, -1),
    0.4f, 0.12f,
    floorMaterial
  ));
  scene.addObject(std::make_shared<Rectangle>(
    Vector(0.7f, 2.0f, 1.3f),
    Vector(1, 0, 0),
    Vector(0, 0, -1),
    0.4f, 0.12f,
    floorMaterial
  ));
  scene.addObject(std::make_shared<Rectangle>(
    Vector(-0.7f, 2.0f, 1.3f),
    Vector(0, 0, 1),
    Vector(1, 0, 0),
    1.2f, 0.12f,
    floorMaterial
  ));
  scene.addObject(std::make_shared<Rectangle>(
    Vector(-0.7f, 2.0f, 0.9f),
    Vector(0, 0, -1),
    Vector(1, 0, 0),
    1.2f, 0.12f,
    floorMaterial
  ));
  scene.addObject(std::make_shared<Rectangle>(
    Vector(-0.7f, 1.88f, 0.9f),
    Vector(0, -1, 0),
    Vector(1, 0, 0),
    1.4f, 0.4f,
    floorMaterial
  ));

  scene.addObject(std::make_shared<Rectangle>(
    Vector(-0.6f, 1.849999f, 0.9f),
    Vector(0, -1, 0),
    Vector(1, 0, 0),
    1.2f, 0.3f,
    lampMaterial
  ));

  std::cout << "Rendering...\n";

  std::chrono::time_point<std::chrono::system_clock> start, end;
  start = std::chrono::system_clock::now();

  renderer.render(scene, camera, pixels);

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed = end-start;

  float sec = elapsed.count();
  int min = sec / 60;
  sec -= min * 60;
  int hours = min / 60;
  min -= hours * 60;
  std::cout << "\aFinished rendering in ";
  if(hours > 0) std::cout << hours << "h ";
  if(min > 0) std::cout << min << "m ";
  std::cout << sec << "s\n";

  savePPM("render.ppm", width, height, pixels);
  delete[] pixels;
  return 0;
}
