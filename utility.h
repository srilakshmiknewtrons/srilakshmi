#pragma once
#include <vector>
#include <fstream>
#define STBI_NO_FAILURE_STRINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdlib> 
#include <fmt/core.h>
#include <glm/glm.hpp>


namespace utility
{
   
  std::vector<glm::vec3> createImage(int width, int height, const glm::vec3& color)
    {
        return std::vector<glm::vec3>(width * height, color);
    }
     void writeBMP(const std::vector<glm::vec3>& pixels, int width, int height, const char* filename) {
        std::vector<unsigned char> data(width * height * 3);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                const glm::vec3& pixel = pixels[y * width + x];
                int index = ((height - 1 - y) * width + x) * 3;
                data[index] = (unsigned char)(glm::clamp(pixel.r, 0.0f, 1.0f) * 255.0f);
                data[index + 1] = (unsigned char)(glm::clamp(pixel.g, 0.0f, 1.0f) * 255.0f);
                data[index + 2] = (unsigned char)(glm::clamp(pixel.b, 0.0f, 1.0f) * 255.0f);
            }
        }
        stbi_write_bmp(filename, width, height, 3, data.data());
    }

     std::vector<glm::vec3> loadImage(const char* filename, int& width, int& height)
     {
         int channels;
         unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);
         if (!data)
         {
             return {}; // return empty vector
         }

         std::vector<glm::vec3> pixels(width * height);
         const float inv255 = 1.0f / 255.0f;
         for (int i = 0, j = 0; i < width * height; ++i, j += 3)
         {
             pixels[i] = glm::vec3(
                 data[j] * inv255,
                 data[j + 1] * inv255,
                 data[j + 2] * inv255);
         }
         stbi_image_free(data);
         return pixels;
     }

    struct Image2D {
        std::vector<glm::vec3> pixels;
        int width = 0;
        int height = 0;
        Image2D(int w, int h, glm::vec3 color) : width(w), height(h), pixels(w* h, color) {}
        void setPixel(int x, int y, glm::vec3 color) {
            if (x >= 0 && x < width && y >= 0 && y < height)
                pixels[y * width + x] = color;
        }
    };
    struct Vertex {
         glm::vec2 position;
         glm::vec3 color;
         glm::vec2 textcods;
         Vertex() = default;
       
        Vertex(glm::vec2 pos, glm::vec3 col, glm::vec2 text) : position(pos), color(col), textcods(text) {}
      
    };

    struct Texture {
        int width = 0;
        int height = 0;
        std::vector<glm::vec3> pixels;
        Texture() = default;
        Texture(int w, int h, std::vector<glm::vec3> p) : width(w), height(h), pixels(std::move(p)) {}
        glm::vec3 getColor(glm::vec2 uv) const {
            uv = glm::fract(uv);
            uv = glm::clamp(uv, 0.0f, 1.0f);
            int x = static_cast<int>(uv.x * (width - 1));
            int y = static_cast<int>(uv.y * (height - 1));
            return pixels[y * width + x];
        }
    };






   int randomIndex(glm::vec2 uv)
    {
        int tileX = static_cast<int>(uv.x);
        int tileY = static_cast<int>(uv.y);

        unsigned int seed = tileX * 12582917u ^ tileY * 40503u;
        std::srand(seed);

        int id = std::rand() % 3;
        return id;
    }
  }




 




 

