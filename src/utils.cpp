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

Vector toXYZ(const Vector& color)
{
  Vector result;
  result.x = 0.4123865632529917*color.x + 0.35759149092062537*color.y + 0.18045049120356368*color.z;
  result.y = 0.21263682167732384*color.x + 0.7151829818412507*color.y + 0.07218019648142547*color.z;
  result.z = 0.019330620152483987*color.z + 0.11919716364020845*color.y + 0.9503725870054354*color.z;

  return result;
}

Vector fromXYZ(const Vector& color)
{
  Vector result;
  result.x = 3.2410032329763587*color.x - 1.5373989694887855*color.y - 0.4986158819963629*color.z;
  result.y = -0.9692242522025166*color.x + 1.875929983695176*color.y + 0.041554226340084724*color.z;
  result.z = 0.055639419851975444*color.z - 0.20401120612390997*color.y + 1.0571489771875335*color.z;

  return result;
}

void createOrthogonalSystem(const Vector& normal, Vector& tangent, Vector& bitangent)
{
  float sign = copysignf(1.0f, normal.z);
  const float a = -1.0f / (sign + normal.z);
  const float b = normal.x * normal.y * a;
  tangent = Vector(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  bitangent = Vector(b, sign + normal.y * normal.y * a, -normal.y);
}