
#include <ImfRgbaFile.h>
#include <ImfIO.h>
// #include "Iex.h"
#include <drawImage.h>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace Imf;
using namespace Imath;

void
readRgba1 (const char fileName[],
    Array2D<Rgba> &pixels,
    int &width,
    int &height)
// from openexr-1.7.0/doc/ReadingAndWritingImageFiles.pdf page 6
{
  RgbaInputFile file (fileName);
  Box2i dw = file.dataWindow();
  width = dw.max.x - dw.min.x + 1;
  height = dw.max.y - dw.min.y + 1;
  pixels.resizeErase (height, width);
  file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
  file.readPixels (dw.min.y, dw.max.y);
}

int main()
{
  // string fileName = "../textures/earth.jpg";
  // string fileName = "../textures/tux.png";
  string fileName = "textures/tiled.exr";
  Array2D<Rgba> pixels;
  int width, height;
  readRgba1(fileName.c_str(), pixels, width, height);
}
