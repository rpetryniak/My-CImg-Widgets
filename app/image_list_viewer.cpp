#include "MyImageList.h"
#include "MyImageListCanvas.h"
#include "MyImageListWindow.h"

int main(int argc,char **argv)
{
  const char
    *file_l  = cimg_option("-list",(char*)0, "Image list");

  if (file_l ==(char*)0) throw CImgException("Please specify file with image paths (option -list)");

  MyImageList<unsigned char> imageList;

  //Load images from filesystem:
  imageList.loadImagesFromListFile(file_l);

  MyImageListCanvas<unsigned char> imageListCanvas(800,600,1,3,200);
  imageListCanvas.setImageList(&imageList);
  imageListCanvas.prepare_image_panel();
  imageListCanvas.redraw();
  
  //Set algorithm
  ThresholdAlg<unsigned char> thresholdAlg;
  thresholdAlg.setParam1(128);
  imageList.setProcessingAlgorithm(&thresholdAlg);

  MyImageListWindow<unsigned char> imageListWindow(&imageListCanvas);

  return 0;
}

/* Please read first README.txt file.

CMake project building:
  cd $PROJECT_BUILD/MyCImgWidgets && make

Go to building directory:
  cd $PROJECT_BUILD/MyCImgWidgets
  cd $PROJECT_BUILD/MyCImgWidgets-qtcreator

Example usage:
* Display all images listed in specified input file. Press F1 to see additional help.
  $PROJECT_BUILD/MyCImgWidgets/image_list_viewer \
    -list $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected.txt

Command to build single app from console (for testing purpose):
  g++ -o $PROJECT_BUILD/MyCImgWidgets/image_list_viewer \
    $PROJECT_SRC/MyCImgWidgets/app/image_list_viewer.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageWindow.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageListCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageListWindow.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageList.cpp \
    -I$PROJECT_SRC/MyCImgWidgets/include \
    -lX11 -lpthread -lm -lsqlite3 -O3
*/
