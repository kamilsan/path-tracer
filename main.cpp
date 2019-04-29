#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include <cstring> //memcpy
#include <memory>

class Vector
{
public:
  float x, y, z;

  Vector():x(0), y(0), z(0) {}
  Vector(const float xx, const float yy, const float zz): x(xx), y(yy), z(zz) {}
  float lengthSq() const { return x * x + y * y + z * z; }
  float length() const { return sqrtf(lengthSq()); }
  float dot(const Vector &other) const { return x * other.x + y * other.y + z * other.z; }
  Vector cross(const Vector &other) const { return Vector(y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x); }
  Vector& normalize();
  Vector& clamp(float min, float max);
  Vector clone() const { return Vector(x, y, z); }

  Vector operator-() const { return Vector(-x, -y, -z); }
  Vector operator+(const Vector &other) const { return Vector(x + other.x, y + other.y, z + other.z); }
  Vector& operator+=(const Vector &other)
  {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
  }
  Vector operator-(const Vector &other) const { return Vector(x - other.x, y - other.y, z - other.z); }
  Vector& operator-=(const Vector &other)
  {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
  }
  Vector operator*(const float r) const { return Vector(x * r, y * r, z * r); }
  Vector& operator*=(const float r)
  {
    x *= r;
    y *= r;
    z *= r;
    return *this;
  }
  Vector& operator*=(const Vector& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }
  Vector operator*(const Vector &other) const { return Vector(x * other.x, y * other.y, z * other.z); }
  Vector operator/(const float r) const { float f = 1.0f/r; return Vector(x * f, y * f, z * f); }
  Vector& operator/=(const float r) 
  { 
    float f = 1.0f/r;
    x *= f;
    y *= f;
    z *= f;
    return *this; 
  }
  Vector& operator/=(const Vector& v) 
  { 
    x /= v.x;
    y /= v.y;
    z /= v.z;
    return *this; 
  }


  friend std::ostream& operator<<(std::ostream& os, const Vector& v)
  {
     return os << "<" << v.x << ", " << v.y << ", " << v.z << ">";
  }
  friend Vector operator*(const float r, const Vector& v)
  {
     return v*r;
  }
};

Vector& Vector::normalize()
{
  float len = length();
  if(len > 0.00001f)
  {
    float f = 1.0f/len;
    x *= f;
    y *= f;
    z *= f;
  }
  return *this;
}
Vector& Vector::clamp(float min = 0.0f, float max = 1.0f)
{
  if(x < min) x = min;
  else if(x > max) x = max;
  if(y < min) y = min;
  else if(y > max) y = max;
  if(z < min) z = min;
  else if(z > max) z = max;
  return *this;
}

class Ray
{
public:
  Vector origin, direction;

  Ray(): origin(Vector(0, 0, 0)), direction(Vector(0, 0, 1)) {}
  Ray(const Vector& orig, const Vector& dir): origin(orig), direction(dir) {}

  Vector operator()(float t) const { return origin + t*direction; };
};

class RNG
{
private:
  std::mt19937 mt;
  std::uniform_real_distribution<float> dist;
public:
  RNG()
  {
    mt.seed(std::chrono::system_clock::now().time_since_epoch().count());
  }
  RNG(unsigned seed)
  {
    mt.seed(seed);
  }

  float get() { return dist(mt); }
};

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
  Texture(const Texture& texture): m_width(texture.m_width), m_height(texture.m_height), m_len(texture.m_len), m_valid(texture.m_valid), m_flip_v(texture.m_flip_v)
  {
    if(m_valid)
    {
      //delete[] m_data;
      m_data = new float[m_len];
      std::memcpy(m_data, texture.m_data, m_len*sizeof(float));
    }
  }
  Texture(int width, int height, const float *data, bool flip_v = false): m_width(width), m_height(height), m_flip_v(flip_v)
  {
    m_len = 3*width*height;
    std::memcpy(m_data, data, m_len*sizeof(float));
    m_valid = true;
  }
  Texture(const Vector& color = Vector(0.0f, 0.0f, 0.0f), bool flip_v = false): m_flip_v(flip_v)
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
  Texture(const char* fileName, bool flip_v = false): m_flip_v(flip_v)
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
  ~Texture()
  {
    delete[] m_data;
  }

  void setVFlipping(bool flipV) { m_flip_v = flipV; };
  bool flipV() const { return m_flip_v; }
  bool isValid() const { return m_valid; }
  Vector sample(float u, float v) const;

  Texture& operator=(const Texture& other)
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
};

Vector Texture::sample(float u, float v) const
{
  if(!m_valid) return Vector();
  if(m_flip_v) v = 1.0 - v;
  int x = (int)(u*m_width) % m_width;
  int y = (int)(v*m_height) % m_height;
  int i = 3*(y*m_width + x);
  return Vector(m_data[i], m_data[i+1], m_data[i+2]);
}

class EnvironmentMap
{
private:
  Texture m_texture;
public:
  EnvironmentMap(const Texture& texture): m_texture(texture) {}
  EnvironmentMap(const Vector& color = Vector(0.0f, 0.0f, 0.0f)): m_texture(color) {}
  EnvironmentMap(const char* fileName): m_texture(fileName) {}

  Vector sample(const Vector& v) const;
  bool isValid() const { return m_texture.isValid(); }
};
Vector EnvironmentMap::sample(const Vector& vec) const
{
  float theta = acos(vec.y);
  float phi = atan2(vec.x, vec.z);
  float u = phi * 0.5 * M_1_PI + 0.5;
  float v = theta * M_1_PI;

  return m_texture.sample(u, v);
}

class BRDF
{
public:
  virtual float f(const Vector& wo, const Vector& wi) const = 0;
  virtual float sample_f(const Vector& wo, Vector& wi, RNG& rng, float& pdf) const = 0;
  virtual ~BRDF() {};
};

class LambertBRDF : public BRDF
{
public:
  float diffuseFactor;
  LambertBRDF(): diffuseFactor(0.2) {}
  LambertBRDF(float diffuseFactor): diffuseFactor(diffuseFactor) {}

  float f(const Vector& wo, const Vector& wi) const override
  {
    return diffuseFactor*M_1_PI;
  }

  float sample_f(const Vector& wo, Vector& wi, RNG& rng, float& pdf) const override
  {
    float sinT = sqrtf(rng.get());
    float cosT = sqrtf(1 - sinT * sinT);
    float phi = 2*M_PI*rng.get();
    wi = Vector(sinT * cosf(phi), cosT, sinT * sinf(phi));
    pdf = cosT*M_1_PI;
    return diffuseFactor*M_1_PI;
  }
  
};

class BaseMaterial
{
public:
  virtual Vector getColor(float u, float v) const = 0;
  virtual Vector getEmittance(float u, float v) const = 0;
  virtual bool isEmissive() const = 0;
  virtual BRDF* getBRDF() const = 0;
};

class SolidMaterial : public BaseMaterial
{
public:
  Vector color;
  Vector emittance;
  LambertBRDF* brdf;
  float diffuseFactor;

  SolidMaterial(): BaseMaterial(), color(Vector(1, 1, 1)), emittance(Vector(0,0,0)) 
  {
    brdf = new LambertBRDF();
  }
  SolidMaterial(const Vector& col, float kDiffuse): BaseMaterial(), color(col), emittance(Vector(0,0,0)) 
  {
    brdf = new LambertBRDF(kDiffuse);
  }
  SolidMaterial(const Vector& col, float kDiffuse, const Vector& emit): BaseMaterial(), color(col), emittance(emit) 
  {
    brdf = new LambertBRDF(kDiffuse);
  }
  ~SolidMaterial()
  {
    delete brdf;
  }

  Vector getColor(float u, float v) const override { return color; }
  Vector getEmittance(float u, float v) const override { return emittance; }
  bool isEmissive() const override { return emittance.lengthSq() > 0.1; }
  BRDF* getBRDF() const override { return brdf; }
};

class TexturedMaterial : public BaseMaterial
{
private:
  float m_emittanceIntensity;
  bool m_emissive;
public:
  Texture texture;
  Texture emittance;
  LambertBRDF* brdf;

  TexturedMaterial(): BaseMaterial(), m_emittanceIntensity(0), m_emissive(false), texture(Vector(1, 1, 1)), emittance(Vector(0,0,0))
  {
    brdf = new LambertBRDF();
  }
  TexturedMaterial(const Vector& col, float kDiffuse): BaseMaterial(), m_emittanceIntensity(0), m_emissive(false), texture(col), emittance(Vector(0,0,0))
  {
    brdf = new LambertBRDF(kDiffuse);
  }
  TexturedMaterial(const Vector& col, float kDiffuse, const Vector& emit, float emittanceIntensity): BaseMaterial(), m_emittanceIntensity(emittanceIntensity), texture(col), emittance(emit)
  {
    brdf = new LambertBRDF(kDiffuse);
    m_emissive = (emittanceIntensity * emit.lengthSq()) > 0.1;
  }
  TexturedMaterial(const Texture& tex, float kDiffuse): BaseMaterial(), m_emittanceIntensity(0), m_emissive(false), texture(tex), emittance(Vector(0,0,0))
  {
    brdf = new LambertBRDF(kDiffuse);
  }
  TexturedMaterial(const Texture& tex, float kDiffuse, const Vector& emit, float emittanceIntensity): BaseMaterial(), m_emittanceIntensity(emittanceIntensity), texture(tex), emittance(emit)
  {
    brdf = new LambertBRDF(kDiffuse);
    m_emissive = (emittanceIntensity * emit.lengthSq()) > 0.1;
  }
  TexturedMaterial(const Texture& tex, float kDiffuse, const Texture& emit, float emittanceIntensity): BaseMaterial(),  m_emittanceIntensity(emittanceIntensity), texture(tex), emittance(emit)
  {
    brdf = new LambertBRDF(kDiffuse);
    m_emissive = emittanceIntensity > 0.1;
  }
  ~TexturedMaterial()
  {
    delete brdf;
  }

  float getEmittanceIntensity() {return m_emittanceIntensity; }
  void setEmittanceIntensity(float emitIntensity) { m_emittanceIntensity = std::max(emitIntensity, 0.0f); m_emissive = m_emittanceIntensity > 0.1; }

  Vector getColor(float u, float v) const override { return texture.sample(u, v); }
  Vector getEmittance(float u, float v) const override { return m_emittanceIntensity*emittance.sample(u, v); }
  bool isEmissive() const override { return m_emissive; }
  BRDF* getBRDF() const override { return brdf; }
};

//Objects
class Object
{
public:
  std::shared_ptr<BaseMaterial> material;

  Object(): material(new SolidMaterial()) {}
  Object(std::shared_ptr<BaseMaterial> mat): material(mat) {}

  virtual float intersect(const Ray& ray) const = 0;
  virtual Vector getNormalAt(const Vector &point) const = 0;
  virtual void getUVAt(const Vector &point, float& u, float& v) const = 0;
  virtual bool isFinite() const = 0;
  virtual Vector getSample(RNG& rng) const = 0;
  virtual void getSamples(RNG& rng, int s1, int s2, Vector* samples) const = 0;
  virtual float getInversePDF() const = 0;
};

class Sphere : public Object
{
private:
  float m_invPDF;
  float m_radius;
public:
  Vector center;

  Sphere(const Vector& c, const float radius): Object(), m_radius(radius), center(c)
  {
    m_invPDF = 4.0f * radius*radius * M_PI;
  }
  Sphere(const Vector& c, const float radius, std::shared_ptr<BaseMaterial> mat): Object(mat), m_radius(radius), center(c)
  {
    m_invPDF = 4.0f * radius*radius * M_PI;
  }

  float getRadius() const { return m_radius; }
  void setRadius(float radius);

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector& point) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return true; }
  Vector getSample(RNG& rng) const override;
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override;
  float getInversePDF() const override { return m_invPDF; }
};
void Sphere::setRadius(float radius)
{
  m_radius = radius;
  m_invPDF = 4.0f * radius*radius * M_PI;
}
float Sphere::intersect(const Ray& ray) const
{
  Vector centerToOrigin = ray.origin - center;
  float b = 2.0f * ray.direction.dot(centerToOrigin);
  float c = centerToOrigin.dot(centerToOrigin) - m_radius * m_radius;
  float delta = b*b - 4.0f*c;
  if(delta < 0.0f)
    return -1.0f;

  delta = sqrt(delta);
  float t1 = (-b - delta) * 0.5f;
  float t2 = (-b + delta) * 0.5f;

  return (t1 < t2 ? t1 : t2);
}
Vector Sphere::getNormalAt(const Vector& point) const
{
  return (point - center) / m_radius;
}
void Sphere::getUVAt(const Vector& point, float& u, float& v) const
{
  Vector rp = point - center;
  float theta = acos(rp.y/m_radius);
  float phi = atan2(rp.z, rp.x);
  u = phi * 0.5 * M_1_PI + 0.5;
  v = theta * M_1_PI;
}
Vector Sphere::getSample(RNG& rng) const
{
  float cosT = 2.0f*rng.get() - 1.0f;
  float sinT = sqrtf(1.0f - cosT*cosT);
  float phi = 2.0f * M_PI * rng.get();
  return center + Vector(m_radius * sinT * cosf(phi), m_radius * cosT, m_radius * sinT * sinf(phi));
}
void Sphere::getSamples(RNG& rng, int s1, int s2, Vector* samples) const
{
  float invS1 = 1.0f/s1;
  float invS2 = 1.0f/s2;
  float r1, r2, cosT, sinT, phi;
  int i = 0;
  for(int ny = 0; ny < s2; ++ny)
  {
    for(int nx = 0; nx < s1; ++nx)
    {
      r1 = (nx + rng.get())*invS1;
      r2 = (ny + rng.get())*invS2;
      cosT = 2.0f*r1 - 1.0f;
      sinT = sqrtf(1.0f - cosT*cosT);
      phi = 2.0f * M_PI * r2;
      samples[i++] = center + Vector(m_radius * sinT * cosf(phi), m_radius * cosT, m_radius * sinT * sinf(phi));
    }
  }
}

class Plane : public Object
{
public:
  Vector point;
  Vector normal;

  Plane(const Vector& pt, const Vector& nor): Object(), point(pt), normal(nor) {}
  Plane(const Vector& pt, const Vector& nor, std::shared_ptr<BaseMaterial> mat): Object(mat), point(pt), normal(nor) {}

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector& point) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return false; }
  Vector getSample(RNG& rng) const override { return Vector(0,0,0); }
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override
  {
    for(int i = 0; i < s1*s2; ++i)
    {
      samples[i] = Vector(0,0,0);
    }
  }
  float getInversePDF() const override { return -1; }
};
float Plane::intersect(const Ray& ray) const
{
  float don = ray.direction.dot(this->normal);
  if(don > -0.00001f && don < 0.00001f) return -1;

  return -(ray.origin - point).dot(this->normal) / don;
}
Vector Plane::getNormalAt(const Vector& point) const
{
  return normal;
}
void Plane::getUVAt(const Vector& point, float& u, float& v) const
{
  u = 0;
  v = 0;
}

class Rectangle : public Object
{
private:
  Vector m_normal;
  Vector m_tangent;
  Vector m_bitangent;
  float m_invPDF;
  float m_sizeTangent, m_sizeBitangent;
public:
  Vector point;

  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, const Vector& bitangent, float sizeTangent, float sizeBitangent): Object(), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, const Vector& bitangent, float sizeTangent, float sizeBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, float sizeTangent, float sizeBitangent): Object(), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent);
  }
  Rectangle(const Vector& pt, const Vector& normal, const Vector& tangent, float sizeTangent, float sizeBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_sizeTangent(sizeTangent), m_sizeBitangent(sizeBitangent), point(pt)
  {
    m_invPDF = sizeTangent * sizeBitangent;
    resetAxis(normal, tangent);
  }

  void resetAxis(const Vector& normal, const Vector& tangent, const Vector& bitangent)
  {
    m_normal = normal;
    m_tangent = tangent;
    m_bitangent = bitangent;
    m_normal.normalize();
    m_tangent.normalize();
    m_bitangent.normalize();
  }
  void resetAxis(const Vector& normal, const Vector& tangent)
  {
    m_tangent = tangent;
    m_tangent.normalize();
    m_normal = normal;
    m_normal.normalize();
    m_bitangent = m_normal.cross(m_tangent).normalize();
    m_normal = m_tangent.cross(m_bitangent).normalize();
  }

  float getSizeTangent() const { return m_sizeTangent; }
  float getSizeBitangent() const { return m_sizeBitangent; }
  void setSizeTangent(float sizeTangent);
  void setSizeBitangent(float sizeBitangent);

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector& point) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return true; }
  Vector getSample(RNG& rng) const override;
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override;
  float getInversePDF() const override { return m_invPDF; }
};
void Rectangle::setSizeTangent(float sizeTangent)
{
  m_sizeTangent = sizeTangent;
  m_invPDF = m_sizeTangent * m_sizeBitangent;
}
void Rectangle::setSizeBitangent(float sizeBitangent)
{
  m_sizeBitangent = sizeBitangent;
  m_invPDF = m_sizeTangent * m_sizeBitangent;
}
float Rectangle::intersect(const Ray& ray) const
{
  float don = ray.direction.dot(m_normal);
  if(don > -0.00001f && don < 0.00001f) return -1;
  float t = -(ray.origin - point).dot(m_normal) / don;
  if(t < 0.0f) return -1;

  Vector relPoint = ray(t) - point;
  float distT = relPoint.dot(m_tangent);
  float distB = relPoint.dot(m_bitangent);

  if(distT < 0.0f || distT > m_sizeTangent || distB < 0.0f || distB > m_sizeBitangent) return -1;

  return t;
}
Vector Rectangle::getNormalAt(const Vector& point) const
{
  return m_normal;
}
void Rectangle::getUVAt(const Vector& point, float& u, float& v) const
{
  Vector relPoint = point - this->point;
  float distT = relPoint.dot(m_tangent);
  float distB = relPoint.dot(m_bitangent);
  u = distT / m_sizeTangent;
  v = distB / m_sizeBitangent;
}
Vector Rectangle::getSample(RNG& rng) const
{
  return point + m_tangent*rng.get()*m_sizeTangent + m_bitangent*rng.get()*m_sizeBitangent;
}
void Rectangle::getSamples(RNG& rng, int s1, int s2, Vector* samples) const
{
  float invS1 = 1.0f/s1;
  float invS2 = 1.0f/s2;
  float r1, r2;
  int i = 0;
  for(int ny = 0; ny < s2; ++ny)
  {
    for(int nx = 0; nx < s1; ++nx)
    {
      r1 = (nx + rng.get())*invS1;
      r2 = (ny + rng.get())*invS2;
      samples[i++] = point + m_tangent*r1*m_sizeTangent + m_bitangent*r2*m_sizeBitangent;
    }
  }
}

class Ellipse : public Object
{
private:
  Vector m_normal;
  Vector m_axisT;
  Vector m_axisB;
  float m_invPDF;
  float m_semiTangent, m_semiBitangent;
public:
  Vector center;

  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, const Vector& bitangent, float semiTangent, float semiBitangent): Object(), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, const Vector& bitangent, float semiTangent, float semiBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent, bitangent);
  }
  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, float semiTangent, float semiBitangent): Object(), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent);
  }
  Ellipse(const Vector& ct, const Vector& normal, const Vector& tangent, float semiTangent, float semiBitangent, std::shared_ptr<BaseMaterial> mat): Object(mat), m_semiTangent(semiTangent), m_semiBitangent(semiBitangent), center(ct)
  {
    m_invPDF = M_PI * semiTangent * semiBitangent;
    resetAxis(normal, tangent);
  }

  void resetAxis(const Vector& normal, const Vector& tangent, const Vector& bitangent)
  {
    m_normal = normal;
    m_axisT = tangent;
    m_axisB = bitangent;
    m_normal.normalize();
    m_axisT.normalize();
    m_axisB.normalize();
  }
  void resetAxis(const Vector& normal, const Vector& tangent)
  {
    m_axisT = tangent;
    m_axisT.normalize();
    m_normal = normal;
    m_normal.normalize();
    m_axisB = m_normal.cross(m_axisT).normalize();
    m_normal = m_axisT.cross(m_axisB).normalize();
  }

  float getSemiTangent() const { return m_semiTangent; }
  float getSemiBitangent() const { return m_semiBitangent; }
  void setSemiTangent(float semiTangent);
  void setSemiBitangent(float semiBitangent);

  float intersect(const Ray& ray) const override;
  Vector getNormalAt(const Vector& point) const override;
  void getUVAt(const Vector& point, float& u, float& v) const override;
  bool isFinite() const override { return true; }
  Vector getSample(RNG& rng) const override;
  void getSamples(RNG& rng, int s1, int s2, Vector* samples) const override;
  float getInversePDF() const override { return m_invPDF; }
};
void Ellipse::setSemiTangent(float semiTangent)
{
  m_semiTangent = semiTangent;
  m_invPDF = M_PI * m_semiTangent * m_semiBitangent;
}
void Ellipse::setSemiBitangent(float semiBitangent)
{
  m_semiBitangent = semiBitangent;
  m_invPDF = M_PI * m_semiTangent * m_semiBitangent;
}
float Ellipse::intersect(const Ray& ray) const
{
  float don = ray.direction.dot(m_normal);
  if(don > -0.00001f && don < 0.00001f) return -1;
  float t = -(ray.origin - center).dot(m_normal) / don;
  if(t < 0.0f) return -1;

  Vector relPoint = ray(t) - center;
  float distT = relPoint.dot(m_axisT);
  float distB = relPoint.dot(m_axisB);
  distT *= (distT / (m_semiTangent*m_semiTangent));
  distB *= (distB / (m_semiBitangent*m_semiBitangent));

  if(distT + distB > 1.0f) return -1;

  return t;
}
Vector Ellipse::getNormalAt(const Vector& point) const
{
  return m_normal;
}
void Ellipse::getUVAt(const Vector& point, float& u, float& v) const
{
  Vector relPoint = point - center;
  float distT = relPoint.dot(m_axisT);
  float distB = relPoint.dot(m_axisB);
  u = (distT / 2*m_semiTangent) + 0.5;
  v = (distB / 2*m_semiBitangent) + 0.5;
}
Vector Ellipse::getSample(RNG& rng) const
{
  float r = sqrtf(rng.get());
  float theta = 2.0f*M_PI*rng.get();
  float x = r*cosf(theta)*m_semiTangent;
  float y = r*sinf(theta)*m_semiBitangent;
  return x*m_axisT + y*m_axisB + center;
}
void Ellipse::getSamples(RNG& rng, int s1, int s2, Vector* samples) const
{
  float invS1 = 1.0f/s1;
  float invS2 = 1.0f/s2;
  float r1, r2, r, theta, x, y;
  int i = 0;
  for(int ny = 0; ny < s2; ++ny)
  {
    for(int nx = 0; nx < s1; ++nx)
    {
      r1 = (nx + rng.get())*invS1;
      r2 = (ny + rng.get())*invS2;
      r = sqrt(r1);
      theta = 2.0f*M_PI*r2;
      x = r*cosf(theta)*m_semiTangent;
      y = r*sinf(theta)*m_semiBitangent;
      samples[i++] = x*m_axisT + y*m_axisB + center;
    }
  }
}

struct LightingInformation
{
  Vector diffuseColor;
  float attenuation;
  Ray shadowRay;
  float occlusionLimit;
};

class Light
{
public:
  Vector color;
  float intensity;

  Light(): color(Vector(1, 1, 1)), intensity(1) {}
  Light(const Vector& col, float inten): color(col), intensity(inten) {}
  virtual ~Light() {}

  virtual void getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const = 0;
};

class DirectionalLight : public Light
{
public:
  Vector direction;

  DirectionalLight(const Vector& dir): Light(), direction(dir) {}
  DirectionalLight(const Vector& dir, const Vector& col, float inten): Light(col, inten), direction(dir) {}

  void getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const;
};
void DirectionalLight::getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const
{
  Vector d = -direction;
  float diffuseFactor = saturate(d.dot(normal)) * intensity;
  li.diffuseColor = color * diffuseFactor;
  li.attenuation = 1;
  li.shadowRay = Ray(point + d * 0.0001f, d);
  li.occlusionLimit = -1;
}

class PointLight : public Light
{
public:
  Vector position;

  PointLight(const Vector& pos): Light(), position(pos) {}
  PointLight(const Vector& pos, const Vector& col, float inten): Light(col, inten), position(pos) {}

  void getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const;
};
void PointLight::getLightingInformation(const Vector &point, const Vector &normal, LightingInformation &li) const
{
  Vector toLight = (position - point);
  float len = toLight.length();
  toLight /= len;

  float diffuseFactor = saturate(toLight.dot(normal)) * intensity;
  li.diffuseColor = color * diffuseFactor;
  li.attenuation =  1 / (len * len + 1);
  li.shadowRay = Ray(point + toLight * 0.0001f, toLight);
  li.occlusionLimit = len;
}

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

class Camera
{
private:
  Vector m_forward, m_up, m_right;
  float m_tanhalfFOV;
public:
  Vector position;

  Camera(float fov, const Vector& pos, const Vector& forward, const Vector& up);
  Camera(float fov, const Vector& pos): m_forward(Vector(0, 0, 1)), m_up(Vector(0, 1, 0)), m_right(Vector(1, 0, 0)), position(pos)
  {
    m_tanhalfFOV = tan(fov * M_PI / 360.0);
  }
  Camera(): m_forward(Vector(0, 0, 1)), m_up(Vector(0, 1, 0)), m_right(Vector(1, 0, 0)), m_tanhalfFOV(1.0f), position(Vector(0, 0, -1)) {}

  Ray getCameraRay(float x, float y) const;
};
Camera::Camera(float fov, const Vector& pos, const Vector& forward, const Vector& up): position(pos)
{
  m_tanhalfFOV = tan(fov * M_PI / 360.0);
  m_forward = forward;
  m_forward.normalize();
  m_right = up.cross(m_forward).normalize();
  m_up = m_forward.cross(m_right).normalize();
}
Ray Camera::getCameraRay(float x, float y) const
{
  //           [rx][ry][rz]
  //[x][y][1]  [ux][uy][uz]
  //           [fx][fy][fz]
  //x' = xrx + yux + fx
  //y' = xry + yuy + fy
  //z' = xyz + yuz + fz

  x *= m_tanhalfFOV;
  y *= m_tanhalfFOV;

  Vector dir = Vector(x * m_right.x + y * m_up.x + m_forward.x,
                      x * m_right.y + y * m_up.y + m_forward.y,
                      x * m_right.z + y * m_up.z + m_forward.z).normalize();
  return Ray(position.clone(), dir);
}

//Method by Tom Duff et al.
void createOrthogonalSystem(const Vector& normal, Vector& tangent, Vector& bitangent)
{
  float sign = copysignf(1.0f, normal.z);
  const float a = -1.0f / (sign + normal.z);
  const float b = normal.x * normal.y * a;
  tangent = Vector(1.0f + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
  bitangent = Vector(b, sign + normal.y * normal.y * a, -normal.y);
}

Vector traceRay(Ray &ray, const Scene &scene, const std::vector<std::shared_ptr<Object>> &areaLights, RNG &rng, unsigned int s1, unsigned int s2)
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

  void render(const Scene& scene, const Camera& camera, char* &pixels);
};
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
Vector Renderer::sample(float x, float y, const Scene& scene, const std::vector<std::shared_ptr<Object>> &emissiveObjects, const Camera& camera, unsigned int s1, unsigned int s2)
{
  //[0, w] /w => [0, 1] *2 - 1 => [-1, 1]
  //            this + 0.5 is because we want to hit the middle of the pixel
  //x + jitterX = (x + 0.5) + (m_rng.get() - 0.5f) = x + m_rng.get()
  float rx = (2.0f*((x + m_rng.get()) / m_width) - 1.0f)*m_ar;
  float ry = 1.0f - 2.0f*((y + m_rng.get()) / m_height);

  Ray ray = camera.getCameraRay(rx, ry);

  return traceRay(ray, scene, emissiveObjects, m_rng, s1, s2);
}

int main()
{
  int width = 1366, height = 768;
  char *pixels = nullptr;

  Renderer renderer(width, height);
  renderer.MC_SAMPLES = 24;
  renderer.LIGHT_SAMPLES = 16;

  Camera camera(90, Vector(0, 0, -1), Vector(0, 0, 1), Vector(0, 1, 0));
  Scene scene;
  Texture wallTexture("uv.ppm", true);
  Texture wallTexture2("uv.ppm", false);
  Texture floorTexture("floor.ppm");
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
