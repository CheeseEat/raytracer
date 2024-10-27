#ifndef VECTOR3_H
#define VECTOR3_H

#include "common_vars.h"
#include <cmath>

class Vector3 {
  public:
    
    Vector3() : values{0, 0, 0} {}
    Vector3(double x, double y, double z) : values{x, y, z} {}

    double x() const { return values[0]; }
    double y() const { return values[1]; }
    double z() const { return values[2]; }
    
    double getLength() const
    {
      return(sqrt(pow(values[0], 2) + pow(values[1], 2) + pow(values[2], 2)));
    }

    double getLengthSquared() const
    {
      return(pow(values[0], 2) + pow(values[1], 2) + pow(values[2], 2));
    }

    double operator[](int i) const { return values[i]; }
    double& operator[](int i) { return values[i]; }

    static Vector3 random()
    {
      return Vector3(random_double(), random_double(), random_double());
    }

    static Vector3 random(double min, double max)
    {
      return Vector3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

    bool near_zero() const {
      // Return true if the vector is close to zero in all dimensions.
      auto s = 1e-8;
      return (std::fabs(values[0]) < s) && (std::fabs(values[1]) < s) && (std::fabs(values[2]) < s);
    }


  private:
    
    // The values of the vector
    double values[3];
};

inline Vector3 multiply(const Vector3& first, const Vector3& second)
{
  return Vector3(first.x() * second.x(), first.y() * second.y(), first.z() * second.z());
}

inline Vector3 operator+(const Vector3& first, const Vector3& second)
{
  return Vector3(first.x() + second.x(), first.y() + second.y(), first.z() + second.z());
}

inline double operator*(const Vector3& first, const Vector3& second)
{
  return ((first.x() * second.x()) + (first.y() * second.y()) + (first.z() * second.z()));
}

inline Vector3 operator*(double constant, const Vector3& second)
{
  return Vector3(constant * second.x(), constant * second.y(), constant * second.z());
}

inline Vector3 operator*(const Vector3& second, double constant)
{
  return Vector3(constant * second.x(), constant * second.y(), constant * second.z());
}

inline Vector3 cross(const Vector3& first, const Vector3& second)
{
  return Vector3((first.y() * second.z()) - (first.z() * second.y()), (first.z() * second.x()) - (first.x() * second.z()), (first.x() * second.y()) - (first.y() * second.x()));
}

inline Vector3 operator/(const Vector3& second, double constant)
{
  return (1/constant * second);
}

inline Vector3 operator/(const Vector3& first, const Vector3& second)
{
  return Vector3(first.x() / second.x(), first.y() / second.y(), first.z() / second.z());
}

inline Vector3 operator-(const Vector3& first, const Vector3& second)
{
  return Vector3(first.x() - second.x(), first.y() - second.y(), first.z() - second.z());
}

inline Vector3 operator-(const Vector3& first)
{
  return Vector3(-1.0 * first);
}

inline Vector3 getUnit_Vector(Vector3 v)
{
  double length = v.getLength();
  return Vector3(v.x()/length, v.y()/length, v.z()/length);
}

inline Vector3 random_unit_vector()
{
  while(true)
  {
    Vector3 rand = Vector3::random(-1, 1);
    if(rand.getLength() <= 1)
    {
      return getUnit_Vector(rand);
    }
  }
}

inline Vector3 random_on_hemisphere(const Vector3& n)
{
  Vector3 unit_vector = random_unit_vector();
  if(unit_vector * n > 0.0)
  {
    return unit_vector;
  }
  else
  {
    return -unit_vector;
  }
}

inline Vector3 reflect(const Vector3& v, const Vector3& n)
{
  return v - 2 * (v * n) * n; // A bit confiusing, v*n is dot product, and the second * is scalar multiplication
}

inline Vector3 refract(const Vector3& uv, const Vector3& n, double etai_over_etat)
{
  auto cos_theta = std::fmin((-uv * n), 1.0);
  Vector3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
  Vector3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.getLengthSquared())) * n;
  return r_out_perp + r_out_parallel;
}

// Sample random poitn in box -1, 1 and keep sampling untill you land inside the unit circle
inline Vector3 random_in_unit_disk()
{
  while(true)
  {
    Vector3 p = Vector3(random_double(-1, 1), random_double(-1, 1), 0);
    if(p.getLengthSquared() < 1)
      return p;
  }
}

#endif 