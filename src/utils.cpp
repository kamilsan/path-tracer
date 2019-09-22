#include "utils.hpp"

#include <fstream>
#include <cmath>

#include "vector.hpp"

bool loadPPM(const char *fileName, int &width, int &height, char*& pixels)
{
  std::ifstream file;
  file.open(fileName, std::ios::binary);
  if(!file.is_open()) return false;

  char *h = new char[3];
  file.read(h, 3);
  if(h[0] != 'P' || h[1] != '6') return false;
  delete[] h;

  while(file.get() == '#')
  {
    while(file.get() != '\n');
  }
  file.unget();

  int max;
  file >> width >> height >> max;
  file.get();

  int len = 3*width*height;
  pixels = new char[len];
  file.read(pixels, len);

  file.close();
  return true;
}

bool savePPM(const char *fileName, int width, int height, const char *pixels)
{
  std::ofstream file;
  file.open(fileName, std::ios::binary);
  if(!file.is_open()) return false;

  int len = 3*width*height;
  file << "P6\n" << width << "\n" << height << "\n255\n";
  file.write(pixels, len);

  file.close();

  return true;
}

float saturate(float val)
{
  if(val > 1.0f) return 1.0f;
  else if(val < 0.0f) return 0.0f;
  return val;
}

void sRGBEncode(float& c)
{
  float y = 1.0f/2.4f;

  if(c <= 0.0031308f) c *= 12.92f;
  else c = 1.055f * powf(c, y) - 0.055f;

}

void sRGBDecode(float& c)
{
  if(c <= 0.04045f) c /= 12.92f;
  else c = powf((c+0.055f)/1.055f, 2.4f);
}

void sRGBEncode(Vector& color)
{
  sRGBEncode(color.x);
  sRGBEncode(color.y);
  sRGBEncode(color.z);
}

void sRGBDecode(Vector& color)
{
  sRGBDecode(color.x);
  sRGBDecode(color.y);
  sRGBDecode(color.z);
}

void createOrthogonalSystem(const Vector& normal, Vector& tangent, Vector& bitangent)
{
  float sign = copysignf(1.0f, normal.z);
  const float a = -1.0f / (sign + normal.z);
  const float b = normal.x * normal.y * a;
  tangent = Vector(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  bitangent = Vector(b, sign + normal.y * normal.y * a, -normal.y);
}