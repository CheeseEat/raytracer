#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"
#include "color.h"
#include "texture.h"

class material
{

  public:
    virtual ~material() = default;

    virtual bool scatter(
      const Ray& ray_in, const hit_record& rec, Vector3& attenuation, Ray& scattered
    ) const{
      return false;
    }

    virtual Vector3 emitted(double u, double v, const Vector3& p) const {
        return Vector3(0,0,0);
    }

};

class lambertian : public material
{
  public:
    lambertian(const Vector3& albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(const Ray& ray_in, const hit_record& rec, Vector3& attenuation, Ray& scattered)
    const override
    {
      auto scatter_dir = rec.normal + random_unit_vector();

      if(scatter_dir.near_zero())
      {
        scatter_dir = rec.normal;
      }

      scattered = Ray(rec.p, scatter_dir, ray_in.getTime());
      attenuation = tex->value(rec.u, rec.v, rec.p);
      return true;
    } 

  private:
    shared_ptr<texture> tex;
};

class metal : public material
{
  public:
    metal(const Vector3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray& ray_in, const hit_record& rec, Vector3& attenuation, Ray& scattered)
    const override
    {
      Vector3 reflected = reflect(ray_in.getDirection(), rec.normal);
      reflected = getUnit_Vector(reflected) + (fuzz * random_unit_vector());
      scattered = Ray(rec.p, reflected, ray_in.getTime());
      attenuation = albedo;
      return (scattered.getDirection() * rec.normal) > 0;
    } 

  private:
    Vector3 albedo;
    double fuzz;
};

class dielectric : public material 
{
  public:
    
    dielectric(double refractionIndex) : refractionIndex(refractionIndex) {}

    bool scatter(const Ray& r_in, const hit_record& rec, Vector3& attenuation, Ray& scattered) const override
    {
      attenuation = Vector3(1.0, 1.0, 1.0);
      double ri = rec.front_face ? (1.0/refractionIndex) : refractionIndex;

      Vector3 unitDirection = getUnit_Vector(r_in.getDirection());
      double cos_theta = std::fmin((-unitDirection * rec.normal), 1.0);
      double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

      bool cannot_refract = ri * sin_theta > 1.0;
      Vector3 direction;

      if (cannot_refract || reflectance(cos_theta, ri) > random_double())
          direction = reflect(unitDirection, rec.normal);
      else
          direction = refract(unitDirection, rec.normal, ri);

      scattered = Ray(rec.p, direction, r_in.getTime());
      return true;

    }
  
  private:
    double refractionIndex;
    static double reflectance(double cosine, double refraction_index) {
        // Use Schlick's approximation for reflectance.
        auto r0 = (1 - refraction_index) / (1 + refraction_index);
        r0 = r0*r0;
        return r0 + (1-r0)*std::pow((1 - cosine),5);
    }
    
};

class diffuse_light : public material {
  public:
    diffuse_light(shared_ptr<texture> tex) : tex(tex) {}
    diffuse_light(const Vector3& emit) : tex(make_shared<solid_color>(emit)) {}

    Vector3 emitted(double u, double v, const Vector3& p) const override {
        return tex->value(u, v, p);
    }

  private:
    shared_ptr<texture> tex;
};

#endif