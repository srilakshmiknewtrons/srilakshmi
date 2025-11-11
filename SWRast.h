	#pragma once
#include <glm/glm.hpp>
#include <vector>




struct Image2D {
    std::vector<glm::vec3> pixels;
    int width;
    int height;
    // Image2D(int w, int h, glm::vec3 color): width(w), height(h), pixels(w* h, color) {  } // Parameterized constructor
    Image2D(int w, int h, glm::vec3 color)
        : width(w), height(h), pixels(w* h, color)
    {
    }
    //pixels is a single long vector, not a 2D array.
    void setPixel(int x, int y, glm::vec3 color)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
            //condition  to check the pixel is in boundaries
            pixels[y * width + x] = color;
    }


};


struct Vertex
{
    const glm::vec2 position;
    const glm::vec3 color;
    const glm::vec2 textcoords;
    Vertex(glm::vec2 pos, glm::vec3 col, glm::vec2 text) :position(pos), color(col), textcoords(text) {}

};


struct Texture {
    int width = 0;
    int height = 0;
    std::vector<glm::vec3> pixels;
    //Texture() : width(0), height(0) {}
    //Texture(std::vector<glm::vec3> data, int w, int h)
  //  : width(w), height(h), pixels(std::move(data)) {}
    Texture() = default;  // default constructor
    // Texture(int w, int h, std::vector<glm::vec3>& p)
    //     : width(w), height(h), pixels(std::move(p)) {
    // }
    Texture(int w, int h, std::vector<glm::vec3> p)
        : width(w), height(h), pixels(p) {
    }
    glm::vec3 getColor(glm::vec2 uv) const {
        uv = glm::fract(uv);
        uv = glm::clamp(uv, 0.0f, 0.9999f);
        int x = static_cast<int>(uv.x * (width - 1));
        int y = static_cast<int>(uv.y * (height - 1));

        return pixels[y * width + x];
    }
};


extern Texture texture;

std::vector<glm::vec3> createImage(int width, int height, const glm::vec3& color);
void writeBMP(const std::vector<glm::vec3>& pixels, int width, int height, const char* filename);
std::vector<glm::vec3> loadImage(const char* filename, int& width, int& height);
void drawLine(Image2D& img, const  Vertex& V1, const  Vertex& V2, glm::vec3 color);
void setPixel(int x, int y, glm::vec3 color);


void fillTriangle(Image2D& img, const Vertex& V1, const Vertex& V2, const Vertex& V3);

void fillRandomBrick(Image2D& image, int x, int y, int blockW, int blockH,
    const std::vector<glm::vec3>& brick1, int w1, int h1,
    const std::vector<glm::vec3>& brick2, int w2, int h2,
    const std::vector<glm::vec3>& brick3, int w3, int h3);


