
#include <vector>
#include <fstream>
#define STBI_NO_FAILURE_STRINGS
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cstdlib>
#include <ctime> 
#include <fmt/core.h>
#include <glm/glm.hpp>
#include <random>
std::vector<glm::vec3> createImage(int width, int height, const glm::vec3 & color) {
    std::vector<glm::vec3> pixels(width * height, color);
    return pixels;
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

    // Process all pixels in one loop
    for (int i = 0, j = 0; i < width * height; ++i, j += 3) {
        pixels[i] = glm::vec3(
            data[j] * inv255,
            data[j + 1] * inv255,
            data[j + 2] * inv255
        );
    }
    stbi_image_free(data);
    return pixels;
}
struct Image2D {
    std::vector<glm::vec3> pixels;
    int width = 0;
    int height = 0;
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
    const glm::vec2 textcods;
    Vertex(glm::vec2 pos, glm::vec3 col, glm::vec2 text) :position(pos), color(col), textcods(text) {}

};
struct Texture {
    int width = 0;
    int height = 0;
    std::vector<glm::vec3> pixels;
    Texture() = default;
    Texture(int w, int h, std::vector<glm::vec3> p)
        : width(w), height(h), pixels(std::move(p)) {
    }
    glm::vec3 getColor(glm::vec2 uv) const
    {
        uv = glm::fract(uv);
        // uv = glm::clamp(uv, 10.0f, 20.0f);
        uv = glm::clamp(uv, 0.0f, 1.0f);
        int x = static_cast<int>(uv.x * (width - 1));
        int y = static_cast<int>(uv.y * (height - 1));
        return pixels[y * width + x];
    }
};
Texture texture;
Texture tex1, tex2, tex3;



glm::vec3 randomColor(glm::vec2 uv)
{
    int tileX = (int)uv.x;
    int tileY = (int)uv.y;

   unsigned int seed = tileX * 12582917u ^ tileY * 40503u;
  
  
 //   unsigned int seed = tileX * 73 ^ tileY * 91;
  //  unsigned int seed = tileX * 374761393u ^ tileY * 668265263u;
//   unsigned int seed = tileX * 73856093u ^ tileY * 19349663u;
    std::srand(seed);

    int id = std::rand() % 3;

    switch (id) 
    {
     case 0: return tex1.getColor(uv);
     case 1: return tex2.getColor(uv);
     case 2: return tex3.getColor(uv);
 
    }

}








void fillTriangle(Image2D& img, const Vertex& V1, const Vertex& V2, const Vertex& V3)
{

    int minX = std::min({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x });
    int maxX = std::max({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x });
    int minY = std::min({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y });
    int maxY = std::max({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y });

    float area = ((V2.position.y - V3.position.y) * (V1.position.x - V3.position.x) + (V3.position.x - V2.position.x) * (V1.position.y - V3.position.y));
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            glm::vec2 P(x, y);
            float a1 = ((V2.position.y - V3.position.y) * (P.x - V3.position.x) +
                (V3.position.x - V2.position.x) * (P.y - V3.position.y)) / area;
            float a2 = ((V3.position.y - V1.position.y) * (P.x - V3.position.x) +
                (V1.position.x - V3.position.x) * (P.y - V3.position.y)) / area;
            float a3 = 1.0f - a1 - a2;
            if (a1 >= 0.0f && a2 >= 0.0f && a3 >= 0.0f && a1 <= 1.0f && a2 <= 1.0f && a3 <= 1.0f) {
                glm::vec2 uv = a1 * V1.textcods + a2 * V2.textcods + a3 * V3.textcods;
                //   glm::vec3 finalcolor = texture.getColor(uv);
          
           
                glm::vec3 finalColor = randomColor(uv);
                img.setPixel(x, y, finalColor);

               
              

            }
        }
    }

}






int main()
{
    const int height = 512;
    const int width = 512;
    std::srand(static_cast<unsigned int>(time(0)));
    int w1, h1, w2, h2, w3, h3; // declaring the width and height of the bricks 
   
    Image2D image(width, height, glm::vec3(1.0, 1.0, 1.0));
    std::vector<glm::vec3> brick1 = loadImage("brick1.bmp", w1, h1);
    //when the if block checks if the file is empty then the
    if (brick1.empty())
    {
        fmt::print(" The brick1.bmp file is missing \n");
        return EXIT_FAILURE;
    }

    std::vector<glm::vec3> brick2 = loadImage("brick2.bmp", w2, h2);

    if (brick2.empty())
    {
        fmt::print(" The brick2.bmp file is missing \n");
        return EXIT_FAILURE;
    }
    std::vector<glm::vec3> brick3 = loadImage("brick3.bmp", w3, h3);

    if (brick3.empty())
    {
        fmt::print(" The brick3.bmp file is missing \n");
        return EXIT_FAILURE;
    }




    tex1 = Texture(w1, h1, brick1);
    tex2 = Texture(w2, h2, brick2);
    tex3 = Texture(w3, h3, brick3);

    const Vertex A(glm::vec2(0, 0), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0, 0));
    const Vertex B(glm::vec2(511, 0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(4, 0));
    const Vertex C(glm::vec2(0, 511), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0, 9));
    const Vertex D(glm::vec2(511, 511), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(4, 9));


    fillTriangle(image, A, B, C);
    fillTriangle(image, B, D, C);

    writeBMP(image.pixels, width, height, "test.bmp");
    return EXIT_SUCCESS;
}


/*
// Round UV to the nearest integer tile index
int tileX = int(uv.x);   // UV already scaled by your vertex attribute
int tileY = int(uv.y);

// Create deterministic random seed per tile
unsigned int seed = tileY * 73856093u ^ tileX * 19349663u;

std::srand(seed);
int id = std::rand() % 3;

switch (id) {
case 0: finalColor = tex1.getColor(uv); break;
case 1: finalColor = tex2.getColor(uv); break;
case 2: finalColor = tex3.getColor(uv); break;
}
*/





/*int getTileID(glm::vec2 uv)
{
    int tileX = (int)uv.x;   // 0..2 (for 3 tiles)
    int tileY = (int)uv.y;   // 0..7 (for 8 tiles)


 //unsigned int h = tileX * 1367130551u ^ tileY * 12820163u;

    // make a local RNG seeded by the hash
   // std::mt19937 rng(h);

//    return rng() % 3;



 unsigned int seed = tileX * 12582917u ^ tileY * 40503u;
  std::srand(seed);

    return std::rand() % 3;   // choose tex1 / tex2 / tex3
}*/