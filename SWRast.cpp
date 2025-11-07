// SWRast.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <vector>
#include <fstream>
#define STBI_NO_FAILURE_STRINGS

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fmt/core.h>
#include <glm/glm.hpp>

//#include <algorithm> // for min/max
//glm stands for OpenGL Mathematics —it’s a math library used for graphics programming in C++.
//It gives you ready-made types for vectors, matrices, and math operations (like rotations, scaling, directions, etc).

//So instead of writing your own math for 2D/3D coordinates, you just use glm::vec2, glm::vec3, or glm::mat4.




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


std::vector<glm::vec3> loadImage(const char* filename, int& width, int& height) {
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 3); // force RGB
    if (!data) {
        throw std::runtime_error("Failed to load image");
    }

    std::vector<glm::vec3> pixels(width * height);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = (y * width + x) * 3;
            pixels[y * width + x] = glm::vec3(
                data[i + 0] / 255.0f,
                data[i + 1] / 255.0f,
                data[i + 2] / 255.0f
            );
        }
    }

    stbi_image_free(data);
    return pixels;
}



//“Start at some point, move in a direction, color each pixel we pass through.”
struct Image2D {
    std::vector<glm::vec3> pixels;
    const int width;
    const int height;
   // Image2D(int w, int h, glm::vec3 color): width(w), height(h), pixels(w* h, color) {  } // Parameterized constructor
    Image2D(int w, int h, glm::vec3 color)
        : width(w), height(h), pixels(w* h, color)
    {
    }
        //pixels is a single long vector, not a 2D array.
        void setPixel( int x, int y, glm::vec3 color)
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
    Vertex(glm::vec2 pos, glm::vec3 col,glm::vec2 text):position(pos),color(col),textcoords(text){}
   
 };


struct Texture {
    int width = 0;
    int height = 0;
    std::vector<glm::vec3> pixels;
    //Texture() : width(0), height(0) {}
    //Texture(std::vector<glm::vec3> data, int w, int h)
  //  : width(w), height(h), pixels(std::move(data)) {}
    Texture() = default;  // default constructor
    Texture(int w, int h, std::vector<glm::vec3> p)
        : width(w), height(h), pixels(std::move(p)) {
    }

    glm::vec3 getColor(glm::vec2 uv) const {
        uv = glm::clamp(uv, 0.0f, 1.0f);
        int x = static_cast<int>(uv.x * (width - 1));
       int y = static_cast<int>(uv.y * (height - 1));

        return pixels[y * width + x];
    }
};

Texture texture;

void drawLine(Image2D& img, const  Vertex& V1,const  Vertex& V2, glm::vec3 color)
{

    glm::vec2 dir = V2.position - V1.position;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    glm::vec2 unitDir = dir / dist;
    for (float t = 0; t <= dist; t += 1.0f)
    {
        glm::vec2 pos = V1.position + unitDir * t;
        // img.setPixel(static_cast<int>(pos.x+0.5), static_cast<int>(pos.y+0.5), color);
  //      glm::vec3 lineColor = glm::mix(V1.color, V2.color, t / dist);

        img.setPixel(
            static_cast<int>(std::floor(pos.x)),
            static_cast<int>(std::floor(pos.y)),
            color);
    }
}


//bool isPointInside(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 P, float& a1, float& a2, float& a3)
/*
static bool isPointInside(const Vertex &A, const Vertex &B,const  Vertex &C, glm::vec2 P)
{
    // Compute the area of the main triangle using cross product
 // float area = ((B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y));
    float area = ((B.position.y - C.position.y) * (A.position.x - C.position.x) +
        (C.position.x - B.position.x) * (A.position.y - C.position.y));
    // Barycentric coordinates
    float a1 = ((B.position.y - C.position.y) * (P.x - C.position.x) +
        (C.position.x - B.position.x) * (P.y - C.position.y)) / area;
    float a2 = ((C.position.y - A.position.y) * (P.x - C.position.x) +
        (A.position.x - C.position.x) * (P.y - C.position.y)) / area;
    float a3 = 1.0f - a1 - a2;

    // Return true if inside

    return a1 >= 0.0f && a2 >= 0.0f && a3 >= 0.0f &&
        a1 <= 1.0f && a2 <= 1.0f && a3 <= 1.0f;


}*/


void fillTriangle(Image2D& img, const Vertex& V1, const Vertex& V2, const Vertex& V3)
{
    
    int minX = std::min({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x,});
    int maxX = std::max({ (int)V1.position.x, (int)V2.position.x, (int)V3.position.x, });
    int minY = std::min({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y, });
    int maxY = std::max({ (int)V1.position.y, (int)V2.position.y, (int)V3.position.y, });


 
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
           
           
            float area = ((V2.position.y - V3.position.y) * (V1.position.x - V3.position.x) +(V3.position.x - V2.position.x) * (V1.position.y - V3.position.y));

            glm::vec2 P(x, y);

           float a1 = ((V2.position.y - V3.position.y) * (P.x - V3.position.x) +
               (V3.position.x - V2.position.x) * (P.y - V3.position.y)) / area;
           float a2 = ((V3.position.y - V1.position.y) * (P.x - V3.position.x) +
                (V1.position.x - V3.position.x) * (P.y - V3.position.y)) / area;
           float a3 = 1.0f - a1 - a2;
       
           // if (isPointInside(A, B, C, P,a1,a2,a3)) {
           // if (isPointInside(V1, V2, V3, P)){
              if(a1 >= 0.0f && a2 >= 0.0f && a3 >= 0.0f &&
                  a1 <= 1.0f && a2 <= 1.0f && a3 <= 1.0f){
                glm::vec2 uv = a1 * V1.textcoords + a2 * V2.textcoords + a3 * V3.textcoords;
                glm::vec3 finalcolor = texture.getColor(uv);
                img.setPixel(x, y, finalcolor);
            }
        }
    }
    
}


// When 'image' is declared here, main() allocates space for it inside its own stack frame.
// Then, the Image2D constructor is called to initialize that object.
// During this call, a temporary stack frame is created specifically for the constructor.
// This frame holds the constructor’s parameters (w, h, color) and local variables used during initialization.
// Once the constructor finishes, its frame is popped (removed) from the stack.
// However, the 'image' object itself remains part of main()’s stack frame and continues to exist
// until main() finishes and its entire stack frame is destroyed.

int main()
{
     int height = 512 ;
     int width  = 512;
   // std::vector<glm::vec3> img = createImage(512, 512, glm::vec3(1.0, 1.0, 1.0));
   // writeBMP(img, 512, 512, "test.bmp");


//     std::vector<glm::vec3> pixels = loadImage("brick.bmp", width, height);
// 
 //const char* texture_filename = "brick.bmp";
 int texture_width, texture_height;
 std::vector<glm::vec3> text_pixels= loadImage("brick.bmp", texture_width, texture_height);
// Texture texture{ texture_width, texture_height, text_pixels };

 texture = Texture(texture_width, texture_height, text_pixels );



// Texture wall_texture(std::move(texture_pixels), texture_width, texture_height);
    //Image2D image(img, width, height); 
    //Image2D image(width, height, glm::vec3(1.0, 1.0, 1.0));
    //creating the image instance of image2d creating memory to the inside values
    //(512, 512, white )

   // glm::vec2 origin(200, 200);
   // glm::vec2 direction(2, 1);
    //float distance = 250;

   // moveAndDraw(image, origin, direction, distance, glm::vec3(1, 0, 0));
    // everything a function needs(pixels, width, height) comes from one argument : image.
    //image (the drawing surface) to the moveAndDraw function.
    //writeBMP(image.pixels, width, height, "test.bmp");
    //image.pixels gives you the vector of all pixel colors.
    
    //image is the “paper” you draw on.
     //setPixel() is your “pen.”
     // //moveAndDraw() controls where the pen moves.
   // writeBMP(img, width, height, "test.bmp");
    /* 
      Image2D                        	A drawing sheet
      pixels	         The small color dots (pixels) on the sheet
     setPixel()	             A marker that colors a single pixel
    moveAndDraw()	       The hand moving the marker along a line
       writeBMP()	   A camera that takes a photo of your sheet and saves it as an image file
    */
    Image2D image(width, height, glm::vec3(1.0, 1.0, 1.0));
 // Vertex A = { glm::vec2(100, 100), glm::vec3(1.0f, 0.0f, 0.0f) }; // Red
 // Vertex B = { glm::vec2(400, 100), glm::vec3(0.0f, 1.0f, 0.0f) }; // Green
 // Vertex C = { glm::vec2(250, 400), glm::vec3(0.0f, 0.0f, 1.0f) };
    


   const  Vertex A(glm::vec2(0,0), glm::vec3(1.0, 0.0, 0.0),glm::vec2(0,0)); 
    const Vertex B(glm::vec2(25,0), glm::vec3(0.0, 1.0, 0.0),glm::vec2(1,0));
    const Vertex C(glm::vec2(0,25), glm::vec3(0.0, 0.0, 1.0),glm::vec2(0,1));
    const Vertex D(glm::vec2(25,25), glm::vec3(0.0, 0.0, 0.0),glm::vec2(1,1));
 //   drawLine(image, A, B, glm::vec3(0.0, 1.0, 0.0));
  //  drawLine(image, B, D, glm::vec3(0.0, 1.0, 0.0));
  //  drawLine(image, D, C, glm::vec3(0.0, 1.0, 0.0));
  //  drawLine(image, C, A, glm::vec3(0.0, 1.0, 0.0));
   
 int brickWidth = B.position.x - A.position.x;
    int brickHeight = C.position.y - A.position.y;

    for (int y = 0; y < height; y+=brickWidth ) {
        for (int x = 0; x < width; x += brickHeight) {
            Vertex A1({ A.position.x + x, A.position.y + y }, A.color, A.textcoords);
            Vertex B1({ B.position.x + x, B.position.y + y }, B.color, B.textcoords);
            Vertex C1({ C.position.x + x, C.position.y + y }, C.color, C.textcoords);
            Vertex D1({ D.position.x + x, D.position.y + y }, D.color, D.textcoords);

            fillTriangle(image, A1, B1, C1);
            fillTriangle(image, B1, D1, C1);
        }
    }

   

   // fillTriangle(image, A, B, C, glm::vec3(0.0f,0.0f,0.0f));
 
   // fillTriangle(image, A, B, C);
   // fillTriangle(image, B, D,C);
    //fillTriangle(image, A, B, C, colorA, colorB, colorC);
  //  wroteBMP(pixels,height,width,"brick.bmp");
    writeBMP(image.pixels, width, height, "test.bmp");
   // writeBMP(image.pixels, width, height, "brick.bmp");
    fmt::print("Siri");
    
    return EXIT_SUCCESS;
}





//int y = 200;   //  (row)
    //int x = 100;   // (column)

    //for (int i = 0; i < 10; ++i) {  
      //  img[y * width + (x + i)] = glm::vec3(0.0, 1.0, 0.0); 
    //}

   //img[100,200]= glm::vec3(0.0, 1.0, 0.0);



// for draw inner rectangle
   //int innerRows = height /3;  
    //int innerCols = width /3;  
    //int startRow = (height - innerRows) / 2;
    //int startCol = (width - innerCols) / 2;
    //for (int y = startRow; y < startRow + innerRows; ++y) {
       // for (int x = startCol; x < startCol + innerCols; ++x) {
         //  img[y * width + x] = glm::vec3(0.0, 1.0, 0.0);

        //}
    //}




/* for squares 
struct Image2D
{
  std::vector<glm::vec3>& pixels;
  int width;
  int height;
};
void Square( Image2D& image, int startRow, int startCol, glm::vec3 color)
{
    int innerRows =image. height / 4; //both gives 1/4 of the img
    int innerCols = image. width / 4;
    for (int y = startRow; y < startRow + innerRows; ++y) {
        for (int x = startCol; x < startCol + innerCols; ++x) {
            image.pixels[y * image. width + x] = color;
        }
    }
}*/


/*void lineDraw(std::vector<glm::vec3>& pixels, int corw, int cocm)
{}*/

/* Image2D image{ img,width, height };
    Square(image, 100, 100, glm::vec3(1.0, 0.0, 0.0));
    Square(image, 100, width / 2, glm::vec3(0.0, 1.0, 0.0));
    Square(image, height / 2, 100, glm::vec3(0.0, 0.0, 1.0));
    Square(image, height / 2, width / 2, glm::vec3(1.0, 1.0, 1.0)); */






/* #include <vector>
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
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

struct Image2D {
    std::vector<glm::vec3> pixels;
    int width, height;
// Parameterized constructor
    Image2D(int w, int h, glm::vec3 color)
    {
        width = w;
        height = h;
 // create a pixel array (vector) of size w*h and fill it with the given color
        pixels = std::vector<glm::vec3>(w * h, color);
 
    }
};

void setPixel(Image2D& img, int x, int y, glm::vec3 color) {
    if (x >= 0 && x < img.width && y >= 0 && y < img.height)
        img.pixels[y * img.width + x] = color;
}
// for p1 to p2 it moves (100,100)  (101,100)  (102, 100 )........(400,100)
//for p2 to p3 - as slope(y/x) ( 300/-150 ) = -2 => it moves 2 pixels up for every 1 left.
//(400,100), (399,101), (398,102), (397,103), (397,104) ... (250,400)
// for p3 to p2  slope(y/x) (-300/-150  ) = 2 = (250, 400)(249, 399) (249, 398) (248, 397) ....(100,100)

void moveAndDraw(Image2D& img, glm::vec2 origin, glm::vec2 direction, float distance, glm::vec3 color)
                           
{
    float len = std::sqrt(direction.x * direction.x + direction.y * direction.y); 
    glm::vec2 unitDir = direction / len;             
      for (float t = 0; t <= distance; t += 1.0f)
      {
    glm::vec2 pos = origin + unitDir * t;

    setPixel(img, static_cast<int>(pos.x), static_cast<int>(pos.y), color);
 }  
  }


void drawTriangle(Image2D& img, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, glm::vec3 color) {
    // p1 to p2
    glm::vec2 dir1 = p2 - p1;
    float dist1 = std::sqrt(dir1.x * dir1.x + dir1.y * dir1.y);
    moveAndDraw(img, p1, dir1, dist1, color);

    // p2 to p3
    glm::vec2 dir2 = p3 - p2;
    float dist2 = std::sqrt(dir2.x * dir2.x + dir2.y * dir2.y);
    moveAndDraw(img, p2, dir2, dist2, color);

    //  p3 to p1
    glm::vec2 dir3 = p1 - p3;
    float dist3 = std::sqrt(dir3.x * dir3.x + dir3.y * dir3.y);
    moveAndDraw(img, p3, dir3, dist3, color);
}

int main()
{
    int height = 512 ;
    int width  = 512;
    Image2D image(width, height, glm::vec3(1.0, 1.0, 1.0));
    glm::vec2 A(100, 100);
    glm::vec2 B(400, 100);
    glm::vec2 C(250, 400);
    drawTriangle(image, A, B, C, glm::vec3(1.0, 0.0, 0.0));
    writeBMP(image.pixels, width, height, "test.bmp");
    fmt::print("Siri");
    
    return EXIT_SUCCESS;
}
*/