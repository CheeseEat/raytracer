#ifndef RTW_IMAGE_H
#define RTW_IMAGE_H

#ifdef _MSC_VER
  #pragma warning (push, 0)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "external/stb_image.h"

#include <cstdlib>
#include <iostream>

class rtw_image
{
  public:
    rtw_image(){}

    rtw_image(const char* image_filename)
    {
      auto filename = std::string(image_filename);
      auto imagedir = getenv("RTW_IMAGES");

      if(imagedir && load(std::string(imagedir) + "/" + image_filename)) 
      {
        return;
      }
      if(load(filename)){
        return;
      }
      std::cerr << "ERROR: Failed to load image file\n";
    }

    ~rtw_image() {
        delete[] bdata;
        STBI_FREE(fdata);
    }

    bool load(const std:: string& filename)
    {
      auto n = bytes_per_pixel;
      fdata = stbi_loadf(filename.c_str(), &image_width, &image_height, &n, bytes_per_pixel);
      if(fdata == nullptr)
      {
        return false;
      } 

      bytes_per_scanline = image_width * bytes_per_pixel;
      convert_to_bytes();
      return true;
    }

    int width() const {return(fdata == nullptr) ? 0 : image_width;}
    int height() const {return (fdata == nullptr) ? 0 : image_height;}

    const float* pixel_data(int x, int y) const {
        static float magenta[] = {1.0f, 0.0f, 1.0f};
        if (!fdata) return magenta;

        x = clamp(x, 0, image_width - 1);
        y = clamp(y, 0, image_height - 1);

        return &fdata[(y * image_width + x) * desired_channels];
    }

    void apply_exposure(float exposure) {
        if (!fdata) return;

        for (int i = 0; i < image_width * image_height * desired_channels; ++i) {
            fdata[i] = fdata[i] * std::pow(2.0f, exposure);
        }
    }
  
  private:
    const int bytes_per_pixel = 3;
    float *fdata = nullptr;
    unsigned char *bdata = nullptr;
    int image_width = 0;
    int image_height = 0;
    int bytes_per_scanline = 0;
    static constexpr int desired_channels = 3;

    static int clamp(int x, int low, int high)
    {
      if(x < low) return low;
      if(x < high) return x;
      return high - 1;
    }

    static unsigned char float_to_byte(float value)
    {
      if(value <= 0.0) return 0;
      if(1.0 <= value) return 255;
      return static_cast<unsigned char>(256.0 * value);
    }

    void convert_to_bytes()
    {
      std::cout << image_width << " " << image_height << " " << bytes_per_pixel << "\n";
      int total_bytes = image_width*image_height*bytes_per_pixel;;
      bdata = new unsigned char[total_bytes];

      auto *bptr = bdata;
      auto *fptr = fdata;
      for(auto i=0; i< total_bytes; i++, fptr++, bptr++) *bptr = float_to_byte(*fptr);
    }

};

#ifdef _MSC_VER
    #pragma warning (pop)
#endif

#endif