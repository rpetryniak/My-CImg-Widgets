#ifndef MYIMAGELISTWINDOW_H
#define MYIMAGELISTWINDOW_H

#include "MyImageListCanvas.h"
#include "MyImageQuery.h"
#include "MyImageAlgorithm.h"
#include "util.h"

template<typename T>
class MyImageListWindow : public CImgDisplay {
public:
  MyImageListWindow(MyImageListCanvas<T> *_listCanvas, const char *title="CImg Image Viewer [type F1 for help]") : CImgDisplay(*_listCanvas, title) {
    this->listCanvas = _listCanvas;

    scroll_step = 15;
    size_step   = 30;
    y_pos       = 0;

    show();
  }
  
  void show();

private:
  MyImageListCanvas<T> *listCanvas;

  int scroll_step; //Scroll step
  int size_step;   //Size step
  int y_pos;
  int fillColor;
};

#endif
