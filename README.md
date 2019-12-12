# Path Tracer
Monte Carlo path tracer capable of rendering scenes with multiple light sources (including area lights) and diffuse objects. It implements some techniques to reduce noise, such as stratified and importance sampling.

As for today, this renderer is not capable of reading scene description from an external file. This implies that user have to modify the source code to change the scene.

Example scene code:
```cpp
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
```
Render of scene described above (32 BRDF and 32 area light samples):
![render](https://i.imgur.com/FPqvQcb.png)