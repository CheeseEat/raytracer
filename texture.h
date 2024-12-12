#ifndef TEXTURE_H
#define TEXTURE_H

#include "Vector3.h"
#include "rtw_image.h"
#include "interval.h"
#include "perlin.h"

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

    Vector3 value(double u, double v, const Vector3& p) const override {
        if (image.height() <= 0) return Vector3(0, 1, 1); 

        u = clamp_uv(u);
        v = 1.0 - clamp_uv(v); 

        int i = static_cast<int>(u * image.width());
        int j = static_cast<int>(v * image.height());

        const float* pixel = image.pixel_data(i, j);
        return Vector3(pixel[0], pixel[1], pixel[2]); 
    }

    void apply_exposure(float exposure) {
        image.apply_exposure(exposure);
    }

  private:
    rtw_image image;

    static double clamp_uv(double x) {
        if (x < 0.0) return 0.0;
        if (x > 1.0) return 1.0;
        return x;
    }

};

class noise_texture : public texture {
  public:
    noise_texture(double scale) : scale(scale) {}

    Vector3 value(double u, double v, const Vector3& p) const override {
      return Vector3(.5, .5, .5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p, 7)));
    }

  private:
    perlin noise;
    double scale;
};

class cube_map_texture : public texture {
public:
    cube_map_texture(const char* posx, const char* negx,
                     const char* posy, const char* negy,
                     const char* posz, const char* negz) {
        faces[0] = std::make_shared<image_texture>(posx); // Positive X
        faces[1] = std::make_shared<image_texture>(negx); // Negative X
        faces[2] = std::make_shared<image_texture>(posy); // Positive Y
        faces[3] = std::make_shared<image_texture>(negy); // Negative Y
        faces[4] = std::make_shared<image_texture>(posz); // Positive Z
        faces[5] = std::make_shared<image_texture>(negz); // Negative Z
    }

    Vector3 value(double u, double v, const Vector3& dir) const override {
        double abs_x = fabs(dir.x()), abs_y = fabs(dir.y()), abs_z = fabs(dir.z());
        int face_idx;
        double u_coord, v_coord;

        if (abs_x >= abs_y && abs_x >= abs_z) {
            // X-dominant
            face_idx = (dir.x() > 0) ? 0 : 1; // Positive or negative X
            u_coord = (dir.z() / abs_x + 1.0) * 0.5;
            v_coord = (dir.y() / abs_x + 1.0) * 0.5;
        } else if (abs_y >= abs_x && abs_y >= abs_z) {
            // Y-dominant
            face_idx = (dir.y() > 0) ? 2 : 3; // Positive or negative Y
            u_coord = (dir.x() / abs_y + 1.0) * 0.5;
            v_coord = (dir.z() / abs_y + 1.0) * 0.5;
        } else {
            // Z-dominant
            face_idx = (dir.z() > 0) ? 4 : 5; // Positive or negative Z
            u_coord = (dir.x() / abs_z + 1.0) * 0.5;
            v_coord = (dir.y() / abs_z + 1.0) * 0.5;
        }

        // Sample the correct face
        return faces[face_idx]->value(u_coord, v_coord, dir);
    }

private:
    std::shared_ptr<image_texture> faces[6];
};

#endif