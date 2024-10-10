#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"

class material
{

  public:
    virtual ~material() = default;

    virtual bool scatter(
      const Ray& ray_in, const hit_record& rec, Vector3& attenuation, Ray& scattered
    ) const{
      return false;
    }

};

class lambertian : public material
{
  public:
    lambertian(const Vector3& albedo) : albedo(albedo) {}

    bool scatter(const Ray& ray_in, const hit_record& rec, Vector3& attenuation, Ray& scattered)
    const override
    {
      auto scatter_dir = rec.normal + random_unit_vector();

      if(scatter_dir.near_zero())
      {
        scatter_dir = rec.normal;
      }

      scattered = Ray(rec.p, scatter_dir);
      attenuation = albedo;
      return true;
    } 

  private:
    Vector3 albedo;
};

class metal : public material
{
  public:
    metal(const Vector3& albedo) : albedo(albedo) {}

    bool scatter(const Ray& ray_in, const hit_record& rec, Vector3& attenuation, Ray& scattered)
    {
      Vector3 reflected = reflect(ray_in.getDirection(), rec.normal);
      scattered = Ray(rec.p, reflected);
      attenuation = albedo;
      return true;
    } 

  private:
    Vector3 albedo;
};

#endif