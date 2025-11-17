#include "utility.h"
#include <cstdlib>
#include <ctime>


utility::Texture texture;
utility::Texture tex1, tex2, tex3;



// Fill triangle function
 void fillTriangle(utility::Image2D& img, const utility::Vertex& V1, const utility::Vertex& V2, const utility::Vertex& V3)
{
    int minX = std::min({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x });
    int maxX = std::max({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x });
    int minY = std::min({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y });
    int maxY = std::max({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y });

    float area = ((V2.position.y - V3.position.y) * (V1.position.x - V3.position.x) +
        (V3.position.x - V2.position.x) * (V1.position.y - V3.position.y));

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            glm::vec2 P(x, y);
            float a1 = ((V2.position.y - V3.position.y) * (P.x - V3.position.x) +
                (V3.position.x - V2.position.x) * (P.y - V3.position.y)) / area;
            float a2 = ((V3.position.y - V1.position.y) * (P.x - V3.position.x) +
                (V1.position.x - V3.position.x) * (P.y - V3.position.y)) / area;
            float a3 = 1.0f - a1 - a2;
            if (a1 >= 0 && a2 >= 0 && a3 >= 0 && a1 <= 1 && a2 <= 1 && a3 <= 1) {
                glm::vec2 uv = a1 * V1.textcods + a2 * V2.textcods + a3 * V3.textcods;
                
                int id = utility::randomIndex(uv);

                // Choose texture based on id
                glm::vec3 finalColor;
                switch (id) {
                case 0: finalColor = tex1.getColor(uv); break;
                case 1: finalColor = tex2.getColor(uv); break;
                case 2: finalColor = tex3.getColor(uv); break;
                }

                // Set pixel
                img.setPixel(x, y, finalColor);
            }
        }
    }
}
 utility::Vertex vertexBuffer[4];
 
 

 void drawTriangle(utility::Image2D& img, int index1, int index2, int index3)
 {
      utility::Vertex& v1 = vertexBuffer[index1];
      utility::Vertex& v2 = vertexBuffer[index2];
      utility::Vertex& v3 = vertexBuffer[index3];

     fillTriangle(img, v1, v2, v3);
 }


int main()
{
    const int width = 512;
    const int height = 512;
    int w1, h1, w2, h2, w3, h3;

    std::srand(static_cast<unsigned int>(time(0)));
    utility::Image2D image(width, height, glm::vec3(1.0f));

    std::vector<glm::vec3> brick1 = utility::loadImage("brick1.bmp", w1, h1);
    if (brick1.empty())
    {
        fmt::print(" The brick1.bmp file is missing \n");
        return EXIT_FAILURE;
    }

    std::vector<glm::vec3> brick2 = utility::loadImage("brick2.bmp", w2, h2);

    if (brick2.empty())
    {
        fmt::print(" The brick2.bmp file is missing \n");
        return EXIT_FAILURE;
    }
    std::vector<glm::vec3> brick3 = utility::loadImage("brick3.bmp", w3, h3);

    if (brick3.empty())
    {
        fmt::print(" The brick3.bmp file is missing \n");
        return EXIT_FAILURE;
    }
    tex1 = utility::Texture(w1, h1, brick1);
    tex2 = utility::Texture(w2, h2, brick2);
    tex3 = utility::Texture(w3, h3, brick3);


    vertexBuffer[0] = utility::Vertex({ 0,0 }, { 1, 0, 0 }, { 0, 0 });
    vertexBuffer[1] = utility::Vertex({ 511,0 }, { 0, 1, 0 }, { 4, 0 });
    vertexBuffer[2] = utility::Vertex({ 0,511 }, { 0, 0, 1 }, { 0, 9 });
    vertexBuffer[3] = utility::Vertex({ 511,511 }, { 1, 1, 0 }, { 4, 9 });

    // Draw your quad
    drawTriangle(image, 0, 1, 2);
    drawTriangle(image, 1, 3, 2);



    utility::writeBMP(image.pixels, width, height, "test.bmp");
    return EXIT_SUCCESS;
}




