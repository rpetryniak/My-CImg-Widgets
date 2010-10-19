#include "MyImageCanvas.h"

//public:
template<class T>
void MyImageCanvas<T>::redraw()
{
  draw();
}

template<class T>
void MyImageCanvas<T>::setImage(CImg<T> image)
{
  this->assign(image.width(),image.height(),1,3,fillColor);
  background.assign(image.width(),image.height(),1,3,fillColor);
  foreground.assign(image.width(),image.height(),1,3,255);

  this->image  = image;
  imageVisible= true;
}

template<class T>
CImg<T> MyImageCanvas<T>::getImage()
{
  return image;
}

template<class T>
void MyImageCanvas<T>::showImage()
{
  imageVisible = !imageVisible;
  draw();
}

template<class T>
void MyImageCanvas<T>::setMask(CImg<T> msk)
{
  setMask(msk, maskColor);
}

template<class T>
void MyImageCanvas<T>::setMask(CImg<T> msk, unsigned char* color)
{
  mask        = msk.get_normalize(0,1);
  maskVisible = true;
  maskColor   = color;

  mask_c = msk;
  mask_c.draw_rectangle(0, 0, msk.width(),msk.height(), maskColor);
  maskChanged = true;
}

template<class T>
CImg<T> MyImageCanvas<T>::getMask()
{
  return mask;
}

template<class T>
void MyImageCanvas<T>::showMask()
{
  maskVisible = !maskVisible;
  draw();
}

template<class T>
void MyImageCanvas<T>::cleanMask()
{
  mask.fill(0);
  maskChanged = true;
  draw();
}

template<class T>
bool MyImageCanvas<T>::isMaskChanged()
{
  return maskChanged;
}

template<class T>
void MyImageCanvas<T>::setHelpText(const char* help)
{
  helpImage.assign(270,290,1,3,100).draw_text(5, 5, help, green);
}

template<class T>
void MyImageCanvas<T>::showHelp()
{
  helpVisible = !helpVisible;
  if (helpVisible) infoVisible = false;
  draw();
}

template<class T>
void MyImageCanvas<T>::showInfo()
{
  infoVisible = !infoVisible;

  if (infoVisible) {
    helpVisible = false;
    char info[512] = { 0 };
    const unsigned int siz = image.size(), msiz = siz*sizeof(T);
    const unsigned int mdisp = msiz<8*1024?0:(msiz<8*1024*1024?1:2);
    CImg<double> st = image.get_stats();
    int xm = 0, ym = 0, zm = 0, vm = 0, xM = 0, yM = 0, zM = 0, vM = 0;
    xm = (int)st[4]; ym = (int)st[5], zm = (int)st[6], vm = (int)st[7];
    xM = (int)st[8]; yM = (int)st[9], zM = (int)st[10], vM = (int)st[11];
    
    std::sprintf(info,"size = (%u,%u,%u,%u) [%u %s] \n"
                      "min = %g, max = %g \n"
                      "mean = %g, std = %g \n"
                      "coords(min) = (%u,%u,%u,%u) \n"
                      "coords(max) = (%u,%u,%u,%u)",
                  image.width(), image.height(), image.depth(), image.spectrum(),
                  mdisp==0?msiz:(mdisp==1?(msiz>>10):(msiz>>20)),
                  mdisp==0?"b":(mdisp==1?"Kb":"Mb"),
                  st[0],st[1],st[2],std::sqrt(st[3]),xm,ym,zm,vm,xM,yM,zM,vM
                );
    
    infoImage.assign(180,77,1,3,100).draw_text(5, 5, info, yellow);
  }
  draw();
}

template<class T>
void MyImageCanvas<T>::showGrid()
{
  gridVisible = !gridVisible;
  draw();
}

template<class T>
void MyImageCanvas<T>::setGridColor(unsigned char* color)
{
  gridColor = color;
}

template<class T>
void MyImageCanvas<T>::showCoordinates()
{
  coordinatesVisible = !coordinatesVisible;
  if (!coordinatesVisible) draw();
}

template<class T>
void MyImageCanvas<T>::hideCoordinates()
{
  coordinatesVisible = false;
  draw();
}

template<class T>
bool MyImageCanvas<T>::isCoordinatesVisible()
{
  return coordinatesVisible;
}

template<class T>
void MyImageCanvas<T>::drawCoordinates(int x, int y)
{
  unsigned int val_red=0, val_green=0, val_blue=0;
  if (imageVisible) {
    val_red   = image(x,y,0),
    val_green = image(x,y,1),
    val_blue  = image(x,y,2);
  }
  else {
    val_red   = background(x,y,0),
    val_green = background(x,y,1),
    val_blue  = background(x,y,2);
  }

  this->draw_image(background).
    draw_line(x,0,x,this->height()-1,orange,0.5f,hatch=cimg::rol(hatch)).
    draw_line(0,y,this->width()-1,y,orange,0.5f,hatch=cimg::rol(hatch)).
    //draw_rectangle(0, this->height()-20, 200, this->height()-1, green, 0.3f).
    draw_text(5,this->height()-16,"Pixel(%d,%d)={%d %d %d}",orange,0,0.9f,13,   x,y,val_red,val_green,val_blue);
}

template<class T>
void MyImageCanvas<T>::showHistogram()
{
  histogramVisible = !histogramVisible;
  if (histogramVisible) {
    disp_histogram->assign(255,400 ,"Image histogram",0);
    drawHistogram(image);
  }
  else
    disp_histogram->close();
}

template<class T>
void MyImageCanvas<T>::hideHistogram()
{
  histogramVisible = false;
  draw();
}

template<class T>
bool MyImageCanvas<T>::isHistogramVisible()
{
  return histogramVisible;
}

template<class T>
bool MyImageCanvas<T>::isHistogramDisplayClosed()
{
  if (disp_histogram->is_empty())
    return false;
  else 
    return disp_histogram->is_closed();
}

template<class T>
void MyImageCanvas<T>::drawHistogram(CImg<int> select)
{
  drawHistogram( image.get_crop( select(0,0),select(0,1),select(0,3),select(0,4) ) );
}

template<class T>
void MyImageCanvas<T>::drawHistogram(CImg<T> img)
{
  CImg<> hist = img.get_histogram(256,0,255);
  float min, max = hist.max_min(min);

  CImg<float> axis_255 (11,1,1,1,0); cimg_foroff(axis_255, x) axis_255[x]  = x * 25.5;
  CImg<float> axis_hist(11,1,1,1,0); cimg_foroff(axis_hist,x) axis_hist[x] = max - (x * max/10);
  
  CImg<unsigned char>(disp_histogram->width(),disp_histogram->height(),1,3,255).
    draw_grid(-50*100.0f/hist.width(),-50*100.0f/256,0,0,false,true,black,0.2f,0xCCCCCCCC,0xCCCCCCCC).
    draw_axis(axis_255,  12, black, 0.6f).
    draw_axis(7,  axis_hist, black, 0.6f).
    //(OLD VERSION:) draw_axis(0,255,max,  0.0f,black).
    draw_graph(hist,red).
    display(*disp_histogram);
}

template<class T>
void MyImageCanvas<T>::showProfile()
{
  profileVisible = !profileVisible;

  if (profileVisible) {
    disp_profil->assign();
    disp_profil->set_title("Color profile of the selected line");
  }
  else {
    //disp_profil_x = disp_profil->window_posx();
    //disp_profil_y = disp_profil->window_posy();
    disp_profil->close();
  }
}

template<class T>
void MyImageCanvas<T>::hideProfile()
{
  profileVisible = false;
  draw();
}

template<class T>
bool MyImageCanvas<T>::isProfileVisible()
{
  return profileVisible;
}

template<class T>
bool MyImageCanvas<T>::isProfileDisplayClosed()
{
  if (disp_profil->is_empty())
    return false;
  else
    return disp_profil->is_closed();
}

template<class T>
void MyImageCanvas<T>::drawProfile(CImg<int> select)
{
  CImg<unsigned char> selected_line = get_selected_line(image,select);

  disp_profil->resize(selected_line.width(),400);
  selected_line.display(*disp_profil);

  int ax_p = selected_line.width() < 180 ? 5 : 10;
  
  CImg<float> axis_255   (11,  1,1,1,0); cimg_foroff(axis_255,   x) axis_255[x]    = 255 - (x * 25.5);
  CImg<float> axis_profil(ax_p,1,1,1,0); cimg_foroff(axis_profil,x) axis_profil[x] = x * (selected_line.width()-1.0f)/ax_p;
  
  CImg<unsigned char>(disp_profil->width(),disp_profil->height(),1,3,255).
    draw_grid(-50*100.0f/background.width(),-50*100.0f/256,0,0,false,true,black,0.2f,0xCCCCCCCC,0xCCCCCCCC).
    draw_axis(axis_profil, disp_profil->height()-10, black, 0.6f).
    draw_axis(7, axis_255, black, 0.6f).
    //(OLD VERSION:) draw_axis(0,selected_line.width()-1.0f,255.0f,0.0f,black).
    draw_graph(selected_line.get_shared_channel(0),red,1,1,0,255,0).
    draw_graph(selected_line.get_shared_channel(1),green,1,1,0,255,0).
    draw_graph(selected_line.get_shared_channel(2),blue,1,1,0,255,0).
    draw_text(30,5,"Vect (%d,%d)-(%d,%d)",black,0,1,11, select[0],select[1],select[3],select[4]).
    display(*disp_profil);
}

template<class T>
void MyImageCanvas<T>::showProfileX()
{
  profileXVisible = !profileXVisible;
  if (profileXVisible)
    disp_profil_X->assign( this->width() ,400,"Color profile of the X-axis",0);
  else
    disp_profil_X->close();
}

template<class T>
void MyImageCanvas<T>::showProfileY()
{
  profileYVisible = !profileYVisible;
  if (profileYVisible)
    disp_profil_Y->assign(this->height() ,400 ,"Color profile of the Y-axis",0);
  else
    disp_profil_Y->close();
}

template<class T>
void MyImageCanvas<T>::hideProfileX()
{
  profileXVisible = false;
}

template<class T>
void MyImageCanvas<T>::hideProfileY()
{
  profileYVisible = false;
}

template<class T>
bool MyImageCanvas<T>::isProfileXVisible()
{
  return profileXVisible;
}

template<class T>
bool MyImageCanvas<T>::isProfileYVisible()
{
  return profileYVisible;
}

template<class T>
bool MyImageCanvas<T>::isProfileXDisplayClosed()
{
  if (disp_profil_X->is_empty())
    return false;
  else
    return disp_profil_X->is_closed();
}

template<class T>
bool MyImageCanvas<T>::isProfileYDisplayClosed()
{
  if (disp_profil_Y->is_empty())
    return false;
  else
    return disp_profil_Y->is_closed();
}


template<class T>
void MyImageCanvas<T>::drawProfileX(int mouse_x, int mouse_y)
{
  const int
    xm = mouse_x, // X-coordinate of the mouse pointer over the image
    ym = mouse_y, // Y-coordinate of the mouse pointer over the image
    xl = xm,      // Corresponding X-coordinate of the hatched line
    yl = ym,      // Corresponding Y-coordinate of the hatched line
    x  = xm,      // Corresponding X-coordinate of the pointed pixel in the image
    y  = ym;      // Corresponding Y-coordinate of the pointex pixel in the image

  // Retrieve color component values at pixel (x,y)
  const unsigned int
    val_red   = image(x,y,0),
    val_green = image(x,y,1),
    val_blue  = image(x,y,2);

  CImg<float> axis_255 (11,1,1,1,0); cimg_foroff(axis_255, x) axis_255[x] = 255 - (x * 25.5);
  CImg<float> axis_x   (11,1,1,1,0); cimg_foroff(axis_x,   x) axis_x[x]   = x * (image.width()-1.0f)/10;
  
  // Create and display the image of the intensity profile
  CImg<unsigned char>(disp_profil_X->width(),disp_profil_X->height(),1,3,255).
    draw_grid(-50*100.0f/image.width(),-50*100.0f/256,0,0,false,true,black,0.2f,0xCCCCCCCC,0xCCCCCCCC).
    draw_axis(axis_x, disp_profil_X->height()-10, black, 0.6f).
    draw_axis(7, axis_255, black, 0.6f).
    //(OLD VERSION:) draw_axis(0,image.width()-1.0f,255.0f,0.0f,black).
    draw_graph(image.get_shared_line(y,0,0),red,1,1,0,255,0).
    draw_graph(image.get_shared_line(y,0,1),green,1,1,0,255,0).
    draw_graph(image.get_shared_line(y,0,2),blue,1,1,0,255,0).
    draw_text(30,5,"Pixel (%d,%d)={%d %d %d}",black,0,1,11,mouse_x,mouse_y,val_red,val_green,val_blue).
    draw_line(xl,0,xl,disp_profil_X->height()-1,black,0.5f,hatch=cimg::rol(hatch)).
    display(*disp_profil_X);
}

template<class T>
void MyImageCanvas<T>::drawProfileY(int mouse_x, int mouse_y)
{
  const int
    xm = mouse_x, // X-coordinate of the mouse pointer over the image
    ym = mouse_y, // Y-coordinate of the mouse pointer over the image
    xl = xm,      // Corresponding X-coordinate of the hatched line
    yl = ym,      // Corresponding Y-coordinate of the hatched line
    x  = xm,      // Corresponding X-coordinate of the pointed pixel in the image
    y  = ym;      // Corresponding Y-coordinate of the pointex pixel in the image

  // Retrieve color component values at pixel (x,y)
  const unsigned int
    val_red   = image(x,y,0),
    val_green = image(x,y,1),
    val_blue  = image(x,y,2);

  CImg<unsigned char> vartical_line = get_vertical_line(image,x);

  CImg<float> axis_255 (11,1,1,1,0); cimg_foroff(axis_255, x) axis_255[x] = 255 - (x * 25.5);
  CImg<float> axis_y   (11,1,1,1,0); cimg_foroff(axis_y,   x) axis_y[x]   = x * (image.height()-1.0f)/10;
  
  // Create and display the image of the intensity profile
  CImg<unsigned char>(disp_profil_Y->width(),disp_profil_Y->height(),1,3,255).
    draw_grid(-50*100.0f/image.width(),-50*100.0f/256,0,0,false,true,black,0.2f,0xCCCCCCCC,0xCCCCCCCC).
    draw_axis(axis_y, disp_profil_Y->height()-10, black, 0.6f).
    draw_axis(7, axis_255, black, 0.6f).
    //(OLD VERSION:) draw_axis(0,image.height()-1.0f,255.0f,0.0f,black).
    draw_graph(vartical_line.get_shared_channel(0),red,1,1,0,255,0).
    draw_graph(vartical_line.get_shared_channel(1),green,1,1,0,255,0).
    draw_graph(vartical_line.get_shared_channel(2),blue,1,1,0,255,0).
    draw_text(30,5,"Pixel (%d,%d)={%d %d %d}",black,0,1,11,mouse_x,mouse_y,val_red,val_green,val_blue).
    draw_line(yl,0,yl,disp_profil_Y->height()-1,black,0.5f,hatch=cimg::rol(hatch)).
    display(*disp_profil_Y);
}

template<class T>
void MyImageCanvas<T>::showPaint()
{
  paintVisible = !paintVisible;
  if (!paintVisible) {
    draw();
    x0 = -1;
    y0 = -1;
  }
}

template<class T>
bool MyImageCanvas<T>::isPaintVisible()
{
  return paintVisible;
}

template<class T>
void MyImageCanvas<T>::showCircleTool()
{
  circleToolVisible = !circleToolVisible;
  if (circleToolVisible) closedCurveToolVisible = false;
  if (!circleToolVisible) draw();
}

template<class T>
bool MyImageCanvas<T>::isCircleToolVisible()
{
  return circleToolVisible;
}

template<class T>
void MyImageCanvas<T>::showClosedCurveTool()
{
  closedCurveToolVisible = !closedCurveToolVisible;
  if (closedCurveToolVisible) circleToolVisible = false;
  if (!closedCurveToolVisible) draw();
}

template<class T>
bool MyImageCanvas<T>::isClosedCurveToolVisible()
{
  return closedCurveToolVisible;
}

template<class T>
void MyImageCanvas<T>::paint(int x, int y)
{
  if (x0<0) {
    x0 = x; y0 = y;
  }
  else {
    //foreground.draw_line(x0,y0,x,y,yellow);
    background.draw_line(x0,y0,x,y,yellow);
    this->draw_image(background);
    x0 = x; y0 = y;
  }
  //draw2();
}

template<class T>
void MyImageCanvas<T>::paintCircle(int x, int y, int r, bool del)
{
  unsigned char white_[3]  = {1,1,1};

  if (del)
    mask.draw_circle(x,y,r,black);
  else
    mask.draw_circle(x,y,r,white_);

  maskChanged = true;

  draw();
}

template<class T>
void MyImageCanvas<T>::createClosedCurve(int x, int y, bool del)
{
  unsigned char curve_col2[2]={200,255};

  if (x0<0) {
    x0 = x; y0 = y;
    xstart = x; ystart = y;
  }
  else {
    curve.draw_line(x0,y0,x,y,curve_col2);
    background.draw_line(x0,y0,x,y,blue);
    this->draw_image(background);
    x0 = x; y0 = y;
  }
  
  finishCurveChoosen = del;

  //draw();
}

template<class T>
void MyImageCanvas<T>::finishCurve()
{
  unsigned char curve_col2[2]={200,255}, curve_col3[2]={255,255};

  curve.draw_line(x0,y0,xstart,ystart,curve_col2);

  curve.channel(0).draw_fill(0,0,curve_col3);
  CImg<unsigned char> tmp = curve.get_shared_channel(0).get_normalize(0,1);
  CImg<unsigned char> mask_tmp(mask);

  if (!finishCurveChoosen) {
    cimg_forXYC(mask_tmp,x,y,v) mask_tmp(x,y,v) = 1 - tmp(x,y);
    mask |= mask_tmp;
  } else {
    cimg_forXYC(mask_tmp,x,y,v) mask_tmp(x,y,v) = tmp(x,y);
    mask &= mask_tmp;
  }

  maskChanged = true;
  draw();
  curve.fill(0);
  x0=-1; y0=-1;
}

//private:
template<class T>
void MyImageCanvas<T>::draw()
{
  drawBackground();
  this->draw_image(background);
  //this->draw_image(foreground);
}
/*
template<class T>
void MyImageCanvas<T>::draw2()
{
  this->draw_image(background);

  CImg<> foreground_f1 = foreground;
  cimg_forXY(foreground_f1,x,y) foreground_f1(x,y)  = (foreground_f1(x,y,0)==255 && foreground_f1(x,y,1)==255 && foreground_f1(x,y,2)==255)?0.0f:1.0f;
  CImg<> foreground_f2=foreground_f1.get_shared_channel(0);
  this->draw_image(foreground,foreground_f2);
}
*/
template<class T>
void MyImageCanvas<T>::drawBackground()
{
  background.fill(fillColor);
  if (imageVisible) background.draw_image(image);
  if (maskVisible)  background.draw_image(mask_c,mask,0.3f);
  if (helpVisible)  background.draw_image(helpImage,0.8f);
  if (infoVisible)  background.draw_image(infoImage,0.8f);
  if (gridVisible)  background.draw_grid(32,32,0,0,false,false,yellow,0.4f,0xCCCCCCCC,0xCCCCCCCC);
}

//Class instantiation
template class MyImageCanvas<unsigned char>;

