#ifndef UTIL_H
#define UTIL_H

#include <string>
#include "CImg.h"

using namespace std;
using namespace cimg_library;

// The lines below are necessary when using a non-standard compiler as visualcpp6.
#ifdef cimg_use_visualcpp6
#define std
#endif
#ifdef min
#undef min
#undef max
#endif

static unsigned char
  white[3]  = {255,255,255},
  black[3]  = {0,0,0},
  red[3]    = {255,0,0},
  green[3]  = {0,255,0},
  blue[3]   = {0,0,255},
  yellow[3] = {255,255,0},
  magenta[3]= {255,0,255},
  orange[3] = {255,90,0},
  middle[3] = {127,127,127}
;

typedef struct { unsigned char r, g, b; } rgb;
static rgb random_rgb() {
  rgb c;
  double r;

  c.r = (unsigned char)(rand()%255);
  c.g = (unsigned char)(rand()%255);
  c.b = (unsigned char)(rand()%255);

  return c;
}

static string extractDirectory( const string& path )  {
  return path.substr( 0, path.find_last_of( '/' ) +1 );
}
static string extractFilename( const string& path ) {
  return path.substr( path.find_last_of( '/' ) +1 );
}
/* C-array version
//Extract file name from file path
int  last_slash = 0;
for (last_slash=sizeof(filename_buffor)-1; last_slash>=0; last_slash--)
if (filename_buffor[last_slash] == '/' || filename_buffor[last_slash] == '\\') break;
*/

static string changeExtension( const std::string& path, const std::string& ext )  {
  string filename = extractFilename( path );
  return extractDirectory( path ) +filename.substr( 0, filename.find_last_of( '.' ) ) +ext;
}

//Based on CImg examples: mcf_levelsets.cpp
template<typename T>
static CImg<T> create_closed_curve(CImg<T> ims, CImgDisplay *disp=NULL)
{
  unsigned char col1[2]={0,255}, col2[2]={200,255}, col3[2]={255,255};
  // Create a user-defined closed curve
  CImg<unsigned char> curve(ims.width(),ims.height(),1,2,0);
  ims.draw_grid(20,20,0,0,false,false,col1,0.4f,0xCCCCCCCC,0xCCCCCCCC).
    draw_text(5,5,"Please draw your curve\nin this window\n(Use your mouse)",col1);

  if(disp==NULL) disp->assign(ims,"Draw Curve",0);
  else ims.display(*disp);

  //disp->assign(curve,"Estimated Motion",0);
  //disp = new CImgDisplay(curve,"Estimated Motion",0);
  int xo=-1,yo=-1,x0=-1,y0=-1,x1=-1,y1=-1;
  while (!disp->is_closed() && (x0<0 || disp->button())) {
    if (disp->button() && disp->mouse_x()>=0 && disp->mouse_y()>=0) {
      if (x0<0) { xo = x0 = disp->mouse_x(); yo = y0 = disp->mouse_y(); }
      else {
        x1 = disp->mouse_x(); y1 = disp->mouse_y();
        curve.draw_line(x0,y0,x1,y1,col2);
        ims.draw_line(x0,y0,x1,y1,col2).display(*disp);
        x0 = x1; y0 = y1;
      }
    }
    disp->wait();
    if (disp->is_resized()) disp->resize(disp);
  }
  curve.draw_line(x1,y1,xo,yo,col2).channel(0).draw_fill(0,0,col3);
  CImg<unsigned char> tmp = curve.get_shared_channel(0);
  
  cimg_forXY(tmp,x,y) tmp(x,y) = 255 - tmp(x,y);
  CImg<T> img = CImg<T>(tmp,false);
  return img;
}

template<typename T>
static CImg<T> create_closed_curve(int width, int height, CImgDisplay *disp=NULL)
{
  return create_closed_curve(CImg<T>(width,height,1,3,0), disp);
}

//template<typename T>
//CImg<T> get_vertical_line(CImg<T> img, int x)
static CImg<unsigned char> get_vertical_line(CImg<unsigned char> img, int x)
{
  CImg<unsigned char> res(img.height(),1,1,3,0);

  cimg_forY(img,y) {
    cimg_forC(img,v) {
      res(y,0,v) = img(x,y,v);
    };
  }

  return res;
}

static CImg<unsigned char> get_selected_line(CImg<unsigned char> img, CImg<int> select)
{
  int x1 = select(0,0), y1 = select(0,1), x2 = select(0,3), y2 = select(0,4);
  int dx = std::abs(x2-x1), dy = std::abs(y2-y1);
  int norm = 1+(int)std::sqrt((double)(dx*dx+dy*dy));

  CImg<unsigned char> res(norm,1,1,3,0);

  CImg<unsigned char> mask(img.width(),img.height(),1,3,0);
  mask.draw_line(x1,y1,x2,y2, white);

  int m = x1<x2?1:-1;
  int n = y1<y2?1:-1;

  int p = 0;
  for (int i=x1; m*i<=m*x2; i=1*m+i)
    for (int j=y1; n*j<=n*y2; j=1*n+j)
      if (mask(i,j,0)==255) {
        for (int k=0; k<3; k++)
          res(p,0,k) = img(i,j,k);
        p++;
      }

  res.crop(0,0,p,0).resize(norm,1);

  return res;
}

#endif
