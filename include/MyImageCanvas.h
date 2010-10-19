#ifndef MYIMAGECANVAS_H
#define MYIMAGECANVAS_H

#include "util.h"

#include "MyImageAlgorithm.h"

static const char* imageViewerHelpAndInfo =
  "------------------------------------------\n"
  "Image Viewer\n"
  "------------------------------------------\n"
  "Key 'F1': Show/hide this help\n"
  "Key 'F2': Show/hide image info and stat\n"
  "Key 'I' : Show/hide background image\n"
  "Key 'M' : Show/hide mask\n"
  "Key 'G' : Show/hide grid\n"
  "Key 'C' : Show/hide coordinates\n"
  "Key 'S' : Show/hide selection tool\n"
  "Key 'E' : Clean mask image\n"
  "Key 'T' : Show/hide circle tool\n\t\t[left mouse - draw,left mouse - erase]\n"
  "Key 'K' : Show/hide closed curve tool\n\t\t[left mouse - draw, left mouse - erase,\n\t\t\tmiddle mouse - close curve]\n"
  "Key 'P' : Show/hide paint\n"
  "Key 'H' : Show/hide histogram\n"
  "Key 'R' : Show/hide profile line\n"
  "Key 'X' : Show/hide X profile\n"
  "Key 'Y' : Show/hide Y profile\n";


template<typename T>
class MyImageCanvas : public CImg<T> {
public:
  MyImageCanvas(CImg<T> img) :
    CImg<T>(img),
    background(img),
    foreground(img.width(), img.height(), img.depth(), img.spectrum(), 255),
    curve(img.width(), img.height(), 1, 2, 0),
    fillColor(200)
  {
    initialize();
    setImage(img);
  }

  MyImageCanvas(int W, int H, int Z, int V, int fill=200) :
    CImg<T>(W, H, Z, V, fill),
    image(W, H, Z, V, fill),
    background(W, H, Z, V, fill),
    foreground(W, H, Z, V, 255),
    curve(W, H, 1, 2, 0),
    fillColor(fill)
  {
    initialize();
  }

  void initialize() {
    imageVisible        = false;
    maskVisible         = false;
    maskChanged         = false;
    helpVisible         = false;
    infoVisible         = false;
    gridVisible         = false;
    coordinatesVisible  = false;
    circleToolVisible   = false;
    closedCurveToolVisible = false;
    finishCurveChoosen  = false;
    paintVisible        = false;
    histogramVisible    = false;
    profileVisible      = false;
    profileXVisible     = false;
    profileYVisible     = false;
    disp_profil_exist   = false;
    gridColor           = yellow;
    hatch               = 0xF0F0F0F0;
    x0=-1,y0=-1;
    setHelpText( imageViewerHelpAndInfo );
    processingAlgorithm = new DoNothingAlgorithm<T>();
    algorithmParam  = 128;

    disp_histogram = new CImgDisplay();
    disp_profil    = new CImgDisplay();
    disp_profil_X  = new CImgDisplay();
    disp_profil_Y  = new CImgDisplay();
  }

  ~MyImageCanvas() {
    //CImg<T>::~CImg();
  }

  void redraw();
  void setImage(CImg<T> image);
  CImg<T> getImage();
  void showImage();
  void setMask(CImg<T> msk, unsigned char* color);
  void setMask(CImg<T> msk);
  CImg<T> getMask();
  void showMask();
  void cleanMask();
  bool isMaskChanged();
  void setHelpText(const char* help);
  void showHelp();
  void showInfo();
  void showGrid();
  void setGridColor(unsigned char* color);
  void showCoordinates();
  void hideCoordinates();
  bool isCoordinatesVisible();
  void drawCoordinates(int x, int y);
  void showHistogram();
  void hideHistogram();
  bool isHistogramVisible();
  bool isHistogramDisplayClosed();
  void drawHistogram(CImg<int> select);
  void drawHistogram(CImg<T> img);
  void showProfile();
  void hideProfile();
  bool isProfileVisible();
  bool isProfileDisplayClosed();
  void drawProfile(CImg<int> select);

  void showProfileX();
  void showProfileY();
  void hideProfileX();
  void hideProfileY();
  bool isProfileXVisible();
  bool isProfileYVisible();
  bool isProfileXDisplayClosed();
  bool isProfileYDisplayClosed();
  void drawProfileX(int mouse_x, int mouse_y);
  void drawProfileY(int mouse_x, int mouse_y);

  void showPaint();
  bool isPaintVisible();
  void showCircleTool();
  bool isCircleToolVisible();
  void showClosedCurveTool();
  bool isClosedCurveToolVisible();
  void paint(int x, int y);
  void paintCircle(int x, int y, int r, bool del=false);
  void createClosedCurve(int x, int y, bool del=false);
  void finishCurve();

  void setProcessingAlgorithm(MyProcessingAlgorithm<T> *alg, float param1) { processingAlgorithm = alg; algorithmParam=param1;}
  void setAlgorithmParam(float param1) { algorithmParam=param1; }
  float getAlgorithmParam() { return algorithmParam; }

public:
  MyProcessingAlgorithm<T> *processingAlgorithm;

private:
  void draw();
  //void draw2();
  void drawBackground();

  CImgDisplay *disp_histogram;
  CImgDisplay *disp_profil_X;
  CImgDisplay *disp_profil_Y;

  CImgDisplay *disp_profil;

  CImg<T> background;
  CImg<T> foreground;

  CImg<T> image;
  CImg<T> mask;
  CImg<T> mask_c;
  CImg<unsigned char> curve;

  unsigned char* maskColor;
  unsigned char* gridColor;

  int fillColor;
  unsigned long hatch;

  int x0,y0, xstart, ystart;

  bool imageVisible;
  bool maskVisible;
  bool maskChanged;
  bool helpVisible;
  bool infoVisible;
  bool gridVisible;
  bool coordinatesVisible;
  bool paintVisible;
  bool circleToolVisible;
  bool closedCurveToolVisible;
  bool histogramVisible;
  bool profileVisible;
  bool profileXVisible;
  bool profileYVisible;
  bool finishCurveChoosen;

  bool disp_profil_exist;

  float algorithmParam;

  CImg<T> helpImage;
  CImg<T> infoImage;
};

#endif
