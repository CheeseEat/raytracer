#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector3.h"
#include "rtw_image.h"
#include "interval.h"

class texture
{
  public:
    virtual ~texture() = default;
    virtual Vector3 value(double u, double v, const Vector3& p) const = 0;
};

class solid_color : public texture
{
  public:
    solid_color(const Vector3& albedo) : albedo(albedo) {}
    solid_color(double r, double g, double b) : solid_color(Vector3(r,g,b)) {}

    Vector3 value(double u, double v, const Vector3& p) const override
    {
      return albedo;
    }

  private:
    Vector3 albedo;

};

class checkered : public texture
{
  public:
    checkered(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
    : inv_scale(1.0 / scale), even(even), odd(odd) {}
    checkered(double scale, const Vector3& c1, const Vector3& c2)
    : checkered(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

    Vector3 value(double u, double v, const Vector3& p) const override
    {
      auto xInteger = int(std::floor(inv_scale * p.x()));
      auto yInteger = int(std::floor(inv_scale * p.y()));
      auto zInteger = int(std::floor(inv_scale * p.z()));

      bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

      return isEven ? even->value(u, v, p) : odd->value(u, v, p);

    }

  private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

class image_texture : public texture
{
  
  public:
    image_texture(const char* filename) : image(filename) {}

    Vector3 value(double u, double v, const Vector3& p) const override
    {
      if(image.height() <= 0) return Vector3(0,1,1);
      u = interval(0,1).clamp(u);
      v = 1.0 - interval(0,1).clamp(v);

      int i = int(u * image.width());
      int j = int(v * image.height());
      auto pixel = image.pixel_data(i,j);
      auto color_scale = 1.0 / 255.0;
      return Vector3(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
    }

  private:
    rtw_image image;

};

#endif