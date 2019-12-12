#include <cstring>
#include <iostream>

#include "texture.hpp"
#include "vector.hpp"
#include "utils.hpp"

Texture::Texture(const Texture& texture): m_width(texture.m_width), m_height(texture.m_height), m_len(texture.m_len), m_valid(texture.m_valid), m_flip_v(texture.m_flip_v)
{
  if(m_valid)
  {
    m_data = new float[m_len];
    std::memcpy(m_data, texture.m_data, m_len*sizeof(float));
  }
}

Texture::Texture(int width, int height, const float *data, bool flip_v): m_width(width), m_height(height), m_flip_v(flip_v)
{
  m_len = 3*width*height;
  m_data = new float[m_len];
  std::memcpy(m_data, data, m_len*sizeof(float));
  m_valid = true;
}

Texture::Texture(const Vector& color, bool flip_v): m_flip_v(flip_v)
{
  m_len = 3;
  m_data = new float[3];
  m_data[0] = color.x;
  m_data[1] = color.y;
  m_data[2] = color.z;
  m_width = 1;
  m_height = 1;
  m_valid = true;
}

Texture::Texture(const char* fileName, bool flip_v): m_flip_v(flip_v)
{
  char* temp = nullptr;
  m_valid = loadPPM(fileName, m_width, m_height, temp);
  if(m_valid)
  {
    m_len = 3 * m_width * m_height;
    m_data = new float[m_len];
    float factor = 1.0f/255.0f;
    for(int i = 0; i < m_len; ++i)
    {
      m_data[i] = (unsigned char)temp[i] * factor;
      sRGBDecode(m_data[i]);
    }

    delete[] temp;
  }
  else
  {
    std::cout << "ERROR: Texture (" << fileName << ") could not be loaded!\n";
  }
}

Texture::~Texture()
{
  delete[] m_data;
}

void Texture::setVFlipping(bool flipV) { m_flip_v = flipV; };
bool Texture::flipV() const { return m_flip_v; }
bool Texture::isValid() const { return m_valid; }

Vector Texture::sample(float u, float v) const
{
  if(!m_valid) return Vector();
  if(m_flip_v) v = 1.0 - v;
  int x = (int)(u*m_width) % m_width;
  int y = (int)(v*m_height) % m_height;
  int i = 3*(y*m_width + x);
  return Vector(m_data[i], m_data[i+1], m_data[i+2]);
}

Texture& Texture::operator=(const Texture& other)
{
  if(this == &other)
    return *this;

  m_width = other.m_width;
  m_height = other.m_height;
  m_valid = other.m_valid;
  m_len = other.m_len;
  m_flip_v = other.m_flip_v;

  if(m_valid)
  {
    delete[] m_data;
    m_data = new float[m_len];
    std::memcpy(m_data, other.m_data, m_len*sizeof(float));
  }

  return *this;
}

