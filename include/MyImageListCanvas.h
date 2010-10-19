#ifndef MYIMAGELISTCANVAS_H
#define MYIMAGELISTCANVAS_H

#include "MyImageList.h"
#include "util.h"

static const char* imageListViewerHelpAndInfo =
  "---------------------------------------------\n"
  "             IMAGE LIST VIEWER               \n"
  "---------------------------------------------\n"
  "Key 'F1'    : Show/hide this help            \n"
  "Key 'M'     : Show/hide masks                \n"
  "Key 'Up'    : Scrool panel up                \n"
  "Key 'Down'  : Scrool panel down              \n"
  "Key 'Right' : Zoom In  thumbnails            \n"
  "Key 'Left'  : Zoom Out thumbnails            \n"
  "left  mouse click: select image              \n"
  "right mouse click: display selected image    \n"
  "ctrl right + left mouse scrool : Zoom In/Out \n"
  "                                             \n"
  "---------- Processing options ---------------\n"
  "middle mouse click:                          \n"
  "   process selected image                    \n"
  "shift + middle mouse scrool:                 \n"
  "   change 1st algorithm parameter            \n"
  "Key 'A' : Process all images                 \n"
  "                                             \n"
#ifdef USE_QUERY_FUNCTIONALITY
  "------------ Query options ------------------\n"
  "ctrl + middle mouse click:                   \n"
  "   select and search similar images          \n"
  "ctrl + middle mouse scrool:                  \n"
  "   change number of images to schow          \n"
  "Key 'R' : Show all images and reset order    \n"
  "Key 'G' : Show in groups                     \n"
#endif
;

template<typename T>
class MyImageListCanvas : public CImg<T> {
public:
  MyImageListCanvas(int W, int H, int Z, int V, int fill) :
    CImg<T>(W, H, Z, V, fill)
  {
    fillColor   = fill;
    size        = 100;
    border      = 7;
    width       = W;
    helpVisible = false;
    setHelpText( imageListViewerHelpAndInfo );
  }
  void setImageList(MyImageList<T>* _imageList);
  MyImageList<T>* getImageList();
  void redraw(int x=0, int y=0);
  int select_image(int selx=-1, int sely=-1);
  void prepare_image_panel(int sel=-1);

  void setHelpText(const char* help);
  void showHelp();

  void setParameters(int size, int border)  {this->size = size; this->border = border; }
  void setThumbnailSize(int s)  {size = s;}
  int  getThumbnailSize()       {return size;}
  void setBorderValue(int b)    {border = b;}
  int  getBorderValue()         {return border;}
  void setWidth(int w)          {width = w;}
  int  getWidthValue()          {return width;}

private:
  MyImageList<T> *imageList;
  CImg<T> panelCanvas;
  CImg<T> helpImage;

  int fillColor;
  int size;
  int border;
  int width;

  bool helpVisible;
};

#endif
