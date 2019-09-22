#pragma once

class Vector;

class Texture
{
private:
  int m_width;
  int m_height;
  int m_len;
  float* m_data;
  bool m_valid;
  bool m_flip_v;
public:
  Texture(const Texture& texture);
  Texture(int width, int height, const float *data, bool flip_v = false);
  Texture(const Vector& color, bool flip_v = false);
  Texture(const char* fileName, bool flip_v = false);
  
  ~Texture();

  void setVFlipping(bool flipV);
  bool flipV() const;
  bool isValid() const;
  Vector sample(float u, float v) const;

  Texture& operator=(const Texture& other);
};

