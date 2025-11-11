#include <vector>
#include <fstream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION // for compilation
#include <stb_image.h> // library : used to load the image 
#include <fmt/core.h>
#include <glm/glm.hpp>
std::vector<glm::vec3> createImage(int width, int height, const glm::vec3& color)
{
    std::vector<glm::vec3> pixels(width * height, color);
    return pixels;
}
//in order ot hold the image data  need arry: <glm::vec3> pixels
//filename , width, height, as parameters
//Each pixel has 3 bytes which stores R,G,B 3 channels
//the colors are stored in bmp is bytes not floats 
//unsigned give postive the range 0 - 255 so the the images stored in 8bit foramte
// one byte per channel
// data[0] = r , data[1] = g , data[2] = b = > 3  channel (3 bytes)= one pixel
//clamp () to ensure within range(value,min ,max)
void writeBMP(const std::vector<glm::vec3>& pixels, int width, int height, const char* filename)
{
    std::vector<unsigned char> data(width * height * 3);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const glm::vec3& pixel = pixels[y * width + x];
            int index = ((height - 1 - y) * width + x) * 3; // image will flip
            data[index] = (unsigned char)(glm::clamp(pixel.r, 0.0f, 1.0f) * 255.0f);
            data[index + 1] = (unsigned char)(glm::clamp(pixel.g, 0.0f, 1.0f) * 255.0f);
            data[index + 2] = (unsigned char)(glm::clamp(pixel.b, 0.0f, 1.0f) * 255.0f);
        }
    }
    stbi_write_bmp(filename, width, height, 3, data.data());
}

// we have to load the image using stb-image 
// Loadimage  stack grows as the filname and width and height
// channels here RGB used to store in the pixels here RBB are 3 standard channels
//create a vector for pixels
//stack shrinks when it retruns
std::vector<glm::vec3> loadImage(const char* filename, int& width, int& height) {
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3);
    std::vector<glm::vec3> pixels(width * height);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = (y * width + x) * 3; // Calculate the index in the byte array =>as the img stores bytes need to convert into float
            // 3 channels = 3 bytes
            //as we are getting the image in pixels we need to convert into floats as we need to apply text cords
            pixels[y * width + x] = glm::vec3(
                data[i + 0] / 255.0f,
                data[i + 1] / 255.0f,
                data[i + 2] / 255.0f
            );
        }
    }

    stbi_image_free(data);// to free up the data
    return pixels;
}
struct Image2D {
    std::vector<glm::vec3> pixels;
    const int width;
    const int height;
    Image2D(int w, int h, glm::vec3 color)
        : width(w), height(h), pixels(w* h, color)
    {
    }
    void setPixel(int x, int y, glm::vec3 color)
    {
        if (x >= 0 && x < width && y >= 0 && y < height)
            pixels[y * width + x] = color;
    }
};
//stack grows as the positin , color, textcoords
// drawline() grows as the img,vertex V1,V2, and color as parameters 
// perform the operations
//setpixel() grows with x,y,color
// drawline and setpixel shrinks when it returns
struct Vertex
{
    const glm::vec2 position;
    const glm::vec3 color;
    const glm::vec2 textcoords;
    Vertex(glm::vec2 pos, glm::vec3 col, glm::vec2 text) :position(pos), color(col), textcoords(text) {}
};
// width and height are the members if the struct 
//creating empty constuctor to avcoid  unnecessary initiliasation
//as the new image requires weight , height, pixels 
//get color mainly used to convert from (x,y) to (x,yz)
//clamp() => for min val gives 0, for max val gives 1 (within the range )
struct Texture {
    int width, height;
    std::vector<glm::vec3> pixels;
    Texture() = default;  // default constructor
    Texture(int w, int h, std::vector<glm::vec3> p)
        : width(w), height(h), pixels(p) {
    }
    glm::vec3 getColor(glm::vec2 uv) const
    {
        uv = glm::fract(uv);
        uv = glm::clamp(uv, 0.0f, 1.0f);
        //pixel at the (x,y)
        int x = static_cast<int>(uv.x * (width - 1));
        int y = static_cast<int>(uv.y * (height - 1));
        return pixels[y * width + x];
    }
};
Texture texture;//gloabl can be used any where

//ENters : stack grows with the 3 vertices locals min and max of x,y and area , small areas 
//using barycentric make it fill up the imge pixels
//as to form the quad need ot draw 2 triangles 
//as uv as 2 coords need to convert 3 co ords getcolor() will move to bloack and execute
//final color will pass to the setpixel() with x,y to apply the color
void fillTriangle(Image2D& img, const Vertex& V1, const Vertex& V2, const Vertex& V3)
{

    int minX = std::min({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x, });
    int maxX = std::max({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x, });
    int minY = std::min({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y, });
    int maxY = std::max({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y, });
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float area = ((V2.position.y - V3.position.y) * (V1.position.x - V3.position.x) + (V3.position.x - V2.position.x) * (V1.position.y - V3.position.y));
            glm::vec2 P(x, y);
            float a1 = ((V2.position.y - V3.position.y) * (P.x - V3.position.x) +
                (V3.position.x - V2.position.x) * (P.y - V3.position.y)) / area;
            float a2 = ((V3.position.y - V1.position.y) * (P.x - V3.position.x) +
                (V1.position.x - V3.position.x) * (P.y - V3.position.y)) / area;
            float a3 = 1.0f - a1 - a2;
            //for the texture coors the range is alwaus from 0.0 to 1.0
            if (a1 >= 0.0f && a2 >= 0.0f && a3 >= 0.0f &&
                a1 <= 1.0f && a2 <= 1.0f && a3 <= 1.0f) {
                glm::vec2 uv = a1 * V1.textcoords + a2 * V2.textcoords + a3 * V3.textcoords;//formula
                glm::vec3 finalcolor = texture.getColor(uv); // as color is in (x,y,z) but uv (x,y) need to convert
                img.setPixel(x, y, finalcolor);
            }
        }
    }

}
// main() stack grows as the need to load the brick.img and texture filename , width and height 
//vertex need pos,colr, textcoords
//brick.img is loaded form stb-image and its height and width are stored as original in the texture_width,_height
//texture global initialised here
//here 4 vertex as passed as needto form a quadrant and drawline() called 4 times to fill draw 4 lines 
//fill trianagle () should he called 2 times to fill up the image pixels
//writeBMP() will print the image which now we refecting the brick image 
//exit : stack shrinks when all the locals and fucntions are destroyed it will returns
int main()
{
    int height = 512;
    int width = 512;
    const char* texture_filename = "brick.bmp";
    int texture_width, texture_height;
    std::vector<glm::vec3> text_pixels = loadImage(texture_filename, texture_width, texture_height);
    texture = Texture(texture_width, texture_height, text_pixels);
    Image2D image(width, height, glm::vec3(1.0, 1.0, 1.0));
    const Vertex A(glm::vec2(0, 0), glm::vec3(1.0, 0.0, 0.0), glm::vec2(0, 0));
    const Vertex B(glm::vec2(width, 0), glm::vec3(0.0, 1.0, 0.0), glm::vec2(width / 50, 0));
    const Vertex C(glm::vec2(0, height), glm::vec3(0.0, 0.0, 1.0), glm::vec2(0, height / 50));
    const Vertex D(glm::vec2(width, height), glm::vec3(0.0, 0.0, 0.0), glm::vec2(width / 50, height / 50));
    fillTriangle(image, A, B, C);
    fillTriangle(image, B, D, C);
    writeBMP(image.pixels, width, height, "test.bmp");
    return EXIT_SUCCESS;
}