#ifndef MYIMAGEWINDOW_H
#define MYIMAGEWINDOW_H

#include "MyApplicationParameters.h"
#include "MyImageCanvas.h"
#include "util.h"

template<typename T>
class MyImageWindow : public CImgDisplay {
private:
  MyApplicationParameters* parent;
  MyImageCanvas<T> *canvas;
  double zoomFactor;
  int orginalSizeX;
  int orginalSizeY;
  int circleSize;
  bool wait_for_event;
  bool show_menu_in_window;

protected:
  //void run();
  static void* run(void* p);

  void onLeftButtonRelease() { parent->setActiveWindowId(this->window_id); };
  int window_id;
  
public:
  
  MyImageWindow(MyApplicationParameters *ptr, MyImageCanvas<T> *canvas, const char *title=0, int w_id=0) : CImgDisplay(*canvas, title), canvas(canvas), window_id(w_id), parent(ptr) {
    initialize();
  }

MyImageWindow(MyImageCanvas<T> *canvas, const char *title=0, int w_id=0) : CImgDisplay(*canvas, title), canvas(canvas), window_id(w_id) {
    parent = new MyApplicationParameters();
    initialize();
  }
  
  void initialize() {
    this->canvas        = canvas;
    zoomFactor          = 1.0;
    orginalSizeX        = this->window_width();
    orginalSizeY        = this->window_height();
    circleSize          = 10;
    wait_for_event      = true;
    show_menu_in_window = true;
  }
  ~MyImageWindow() {
    //CImgDisplay::~CImgDisplay();
  }
  void show() { run(this); }
  
  void refresh();
  void setImageCanvas(MyImageCanvas<T> *canvas);
  MyImageCanvas<T> * getImageCanvas();
  void zoomIn();
  void zoomOut();
  void zoom100();

  void set_wait_for_event     (bool opt) { wait_for_event = opt; }
  void set_show_menu_in_window(bool opt) { show_menu_in_window = opt; }
};

#endif
