#include "utility.h"
#include <cstdlib>
#include <ctime>
utility::Texture texture;
utility::Texture tex;
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
             //   glm::vec3 finalcolor = a1 * V1.color + a2 * V2.color + a3 * V3.color;
                glm::vec3 finalcolor = tex.getColor(uv);
                img.setPixel(x, y,finalcolor);
            }
        }
    }
}
utility::Vertex vertexBuffer[13];
int indexBuffer[36];
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
    int w, h;
    utility::Image2D image(width, height, glm::vec3(1.0f));
    std::vector<glm::vec3> circle = utility::loadImage("smiley.bmp", w, h);
    if (circle.empty())
    {
        fmt::print(" The smiley.bmp file is missing \n");
        return EXIT_FAILURE;
    }
    tex = utility::Texture(w, h, circle);
    glm::vec2 center(width / 2.0f, height / 2.0f);
    float radius = 200.0f;
    int segments = 12;
  // center vertex at index 0
    vertexBuffer[0] = utility::Vertex(center, {1,1,1}, {0.5f,0.5f});

    // creates the outer vertices of the circle.
    for (int i = 0; i < segments; i++)
    {
        float degree = (360.0f / segments) * i;
        float a = glm::radians(degree);
        // direction on the circle   //x = cos(a) , y = sin(a)
        glm::vec2 dir = glm::vec2(cos(a), sin(a));
       // position on the circle
        glm::vec2 pos = center + radius * dir;
        // color based on direction
        glm::vec3 col = { fabs(dir.x), fabs(dir.y), 1.0f };
        glm::vec2 uv = { 0.5f + 0.5f * dir.x,0.5f - 0.5f * dir.y};//as the image uv space is from the top to bottom 
        //normalised uv space bottom to top

        // store vertex's attributes  pos,col,uv
        vertexBuffer[i + 1] = utility::Vertex(pos,col,uv);
    }

// creates triangle indices that connect each outer vertex to the center
    for (int i = 1; i <= segments; i++)
    {
        int next = (i == segments) ? 1 : (i + 1);

        // Each triangle uses 3 indices
        int base = (i - 1) * 3;

        indexBuffer[base + 0] = 0;     // center
        indexBuffer[base + 1] = i;     // current
        indexBuffer[base + 2] = next;  // next
    }

    //Loop 3 draws each triangle using those indices.
    int totalIndices = segments * 3;
    for (int i = 0; i < totalIndices; i += 3)
    {
        drawTriangle(image,indexBuffer[i],indexBuffer[i + 1],indexBuffer[i + 2]);
    }
    utility::writeBMP(image.pixels, width, height, "test.bmp");
    return EXIT_SUCCESS;
}




