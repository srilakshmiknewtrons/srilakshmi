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
std::vector<glm::vec3> createImage(int width, int height, const glm::vec3& color) {
    std::vector<glm::vec3> pixels(width * height, color);
    return pixels;
}
void writeBMP(const std::vector<glm::vec3>& pixels, int width, int height, const char* filename) {
    std::vector<unsigned char> data(width * height * 3);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const glm::vec3& pixel = pixels[y * width + x];
            int index = ((height - 1 - y) * width + x) * 3; // Flip vertically for BMP
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
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3); // force RGB
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
    int width=0;
    int height=0;
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
    const glm::vec2 uv;
    Vertex(glm::vec2 pos, glm::vec3 col, glm::vec2 t) :position(pos), color(col), uv(t) {}

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
        uv = glm::clamp(uv, 0.0f, 0.9999f);
        int x = static_cast<int>(uv.x * (width - 1));
        int y = static_cast<int>(uv.y * (height - 1));
        return pixels[y * width + x];
    }
};
Texture texture;
void fillTriangle(Image2D& img, const Vertex& V1, const Vertex& V2, const Vertex& V3)
{

    int minX = std::min({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x, });
    int maxX = std::max({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x, });
    int minY = std::min({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y, });
    int maxY = std::max({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y, });

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
                glm::vec2 uv = a1 * V1.uv + a2 * V2.uv + a3 * V3.uv;
                glm::vec3 finalcolor = texture.getColor(uv);
                img.setPixel(x, y, finalcolor);
            }
        }
    }

}
//passing the parameters for blockW, blockH( size of the block)
//and w and h are the width and height of the each bricks




/*void fillRandomBrick(Image2D& image, int x, int y, int blockW, int blockH,
    const std::vector<glm::vec3>& brick1, int w1, int h1,
    const std::vector<glm::vec3>& brick2, int w2, int h2,
    const std::vector<glm::vec3>& brick3, int w3, int h3)
{
    // Create a vector of textures for storing multuiple blocks
    std::vector<Texture> textures = {
         Texture(w1, h1, brick1),
         Texture(w2, h2, brick2),
         Texture(w3, h3, brick3)
    };
    // 0 - brick1, 1 - brick2 , 2 - brick3
     // Pick a random texture
     // choice is the variable 
     //produves an int from 0 to n-1 (n = textures_size)
     //here the rand() gives different number
     // if we call the rand() without initializing it will give same sequence so for that 
     // we have to initiliaze the seed se to time(0) - current time
    int choice = std::rand() % textures.size();
    texture = textures[choice];

    // Define the four corners of the block
    Vertex A({ x, y }, glm::vec3(1, 1, 1), glm::vec2(0, 0));
    Vertex B({ x + blockW, y }, glm::vec3(1, 1, 1), glm::vec2(1, 0));
    Vertex C({ x, y + blockH }, glm::vec3(1, 1, 1), glm::vec2(0, 1));
    Vertex D({ x + blockW, y + blockH }, glm::vec3(1, 1, 1), glm::vec2(1, 1));

    // Fill the block using two triangles
    fillTriangle(image, A, B, C);
    fillTriangle(image, B, D, C);
}
*/
int main()
{
    const int height = 512;
    const int width = 512;
    std::srand(static_cast<unsigned int>(time(0)));
    int w1, h1, w2, h2, w3, h3; // declaring the width and height of the bricks 
    std::vector<glm::vec3> brick1 = loadImage("brick1.bmp", w1, h1);
    std::vector<glm::vec3> brick2 = loadImage("brick2.bmp", w2, h2);
    std::vector<glm::vec3> brick3 = loadImage("brick3.bmp", w3, h3);
    Image2D image(width, height, glm::vec3(1.0, 1.0, 1.0));
   

    const std::vector<Texture> textures = {
        Texture(w1, h1, brick1),
        Texture(w2, h2, brick2),
        Texture(w3, h3, brick3)
    };
    const int choice = std::rand() % textures.size();
    texture = textures[choice];
  //  const Vertex A({ 50,  50 }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }); // red, bottom-left
   // const Vertex B({ 450,  50 }, { 0.0f, 1.0f, 0.0f }, { 3.0f, 0.0f }); // green, bottom-right
  //  const Vertex C({ 50, 450 }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 3.0f }); // blue, top-left
  //  const Vertex D({ 450, 450 }, { 1.0f, 1.0f, 0.0f }, { 3.0f, 3.0f });


    const Vertex A(glm::vec2(50, 50), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)); // red, bottom-left
    const Vertex B(glm::vec2(450, 50), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(3.0f, 0.0f)); // green, bottom-right
    const Vertex C(glm::vec2(50, 450), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 3.0f)); // blue, top-left
    const Vertex D(glm::vec2(450, 450), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(3.0f, 3.0f)); // yellow, top-right


    fillTriangle(image, A, B, C);
    fillTriangle(image, B, D, C);

    writeBMP(image.pixels, width, height, "test.bmp");
    return EXIT_SUCCESS;
}