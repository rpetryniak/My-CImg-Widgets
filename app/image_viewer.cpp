#include "MyImageCanvas.h"
#include "MyImageWindow.h"

int main(int argc, char** argv )
{
  const char *file_i  = cimg_option("-image", (char*)0, "Input image");
  const char *file_m  = cimg_option("-mask",  (char*)0, "Mask image");
  const char *file_o  = cimg_option("-out",   (char*)0, "Output mask image");

  if (file_i ==(char*)0) throw CImgException("Please specify input image path");

  //Open input image
  CImg<unsigned char> image(file_i);

  //Open input mask or if not specified create an empty
  CImg<unsigned char> m_image;
  if (file_m!=(char*)0) {
    m_image.assign(file_m);
  } else {
    m_image.assign(image.width(), image.height(), 1,3,255);
  }

  //Create canvas and set background image
  MyImageCanvas<unsigned char> maincanvas(image);
  maincanvas.setImage(image);
  maincanvas.setMask(m_image, blue);
  maincanvas.redraw();

  //Set algorithm
  ThresholdAlg<unsigned char> thresholdAlg;
  thresholdAlg.setParam1(128);
  maincanvas.setProcessingAlgorithm(&thresholdAlg,128);
  
  //Create application window
  MyImageWindow<unsigned char> mainwindow(&maincanvas,"Image viewer (type F1 for help)");
  mainwindow.show();

  if ( file_o != (char*)0 ) {
    CImg<unsigned char> maskresult = maincanvas.getMask();
    maskresult.normalize(0,255).save(file_o);
  }

return 0;
}
/* Please read first README.txt file.

CMake project building:
  cd $PROJECT_BUILD/MyCImgWidgets && make

Go to building directory:
  cd $PROJECT_BUILD/MyCImgWidgets
  cd $PROJECT_BUILD/MyCImgWidgets-qtcreator

Example usage:
* Display the image and after editing the mask save it to a specified file ('out' options). Press F1 to see additional help.
  $PROJECT_BUILD/MyCImgWidgets/image_viewer \
    -image $PROJECT_DATA/test_data/lenna_gray.bmp \
    -out $PROJECT_DATA/test_data/lenna_gray-hat.bmp

* Display the image and selected mask image.
  $PROJECT_BUILD/MyCImgWidgets/image_viewer \
    -image $PROJECT_DATA/test_data/lenna_gray.bmp \
    -mask $PROJECT_DATA/test_data/lenna_gray-hat.bmp

Command to build single app from console (for testing purpose):
  g++ -o $PROJECT_BUILD/MyCImgWidgets/image_viewer \
    $PROJECT_SRC/MyCImgWidgets/app/image_viewer.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageWindow.cpp \
    -I$PROJECT_SRC/MyCImgWidgets/include \
    -lX11 -lpthread -lm -O3
*/
