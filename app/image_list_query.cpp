#define USE_QUERY_FUNCTIONALITY

#include <iostream>
#include <fstream>

#include "MyImageList.h"
#include "MyImageListCanvas.h"
#include "MyImageListWindow.h"
#include "MyImageQuery.h"
#include "MyImageAlgorithm.h"

int main(int argc,char **argv)
{
  //Input-output options:
  const char
    *file_t  = cimg_option("-template", (char*)0, "Template image"),
    *file_l  = cimg_option("-list",     (char*)0, "Image list"),
    *file_c  = cimg_option("-cluster",  (char*)0, "Image list with cluster representation");

  if ( (file_l==(char*)0) ) {
    printf("Please specify image list file\n");
    exit(0);
  }

  //Haar query oprions:
  int
    numberOfResults = cimg_option("-n", 20,"Number of results");

  //Interface options:
  int
    size      = cimg_option("-s",300,"Thumbnail size"),
    width     = cimg_option("-w",800,"Window width"),
    heihgt    = cimg_option("-h",600,"Window heihgt"),
    border    = cimg_option("-b",  7,"Border size");


  MyImageList<unsigned char> imageList;
  imageList.loadImagesFromListFile(file_l);

  //Index all loaded images
  MyImageQuery<unsigned char> queryHarr;
  queryHarr.indexImageList(imageList);

  //Cluster images acording to templates
  if ( file_c != (char*)0 ) {
    ifstream tplsfile(file_c);
    string fline;
    vector< CImg<unsigned char> > represent;
    if (tplsfile.is_open())
    {
      while (!tplsfile.eof())
      {
        getline (tplsfile,fline);
        if (!fline.empty()) {
          CImg<unsigned char> loadimg(fline.c_str());
          loadimg.resize(-100,-100,1,3);
          represent.push_back(loadimg);
        }
      }
      queryHarr.clusterImagesAcordingToRepresentatives(&imageList, represent);
      imageList.showInGroups();
      tplsfile.close();
    }
    else {
      printf("Unable to open cluster representative file\n");
      return -1;
    }
  }

  //Search for images matching a input pattern
  if ( file_t != (char*)0 ) {
    CImg<unsigned char> input_tpl(file_t);
    input_tpl.resize(-100,-100,1,3);

    vector<int> order = queryHarr.findBestMatchesForImage(input_tpl, numberOfResults);

    if (file_c == (char*)0 ) {
      imageList.orderList = order;
      printf("Sort images acording to input pattern\n");
    }
  }

  ThresholdAlg<unsigned char> thresholdAlg;
  thresholdAlg.setParam1(128);
  imageList.setProcessingAlgorithm(&thresholdAlg);
  imageList.setQueryAlgorithm(&queryHarr);

  MyImageListCanvas<unsigned char> imageListCanvas(width,heihgt,1,3,200);
  imageListCanvas.setImageList(&imageList);
  imageListCanvas.setParameters(size, border);
  imageListCanvas.prepare_image_panel();
  imageListCanvas.redraw();
  
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
* Initial start-up, load the image into the database. Press F1 to see additional help.
  $PROJECT_BUILD/MyCImgWidgets/image_list_query -s 100 -n 20 \
    -list $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected.txt

* Load images and find 20 matching the given template.
  $PROJECT_BUILD/MyCImgWidgets/image_list_query -s 100 -n 20 \
    -list $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected.txt \
    -template $PROJECT_DATA/test_data/casimage_selected/357.jpg

* Cluster images according to provided representative images ('cluster' option).
  $PROJECT_BUILD/MyCImgWidgets/image_list_query -s 100 -n 20 \
    -list $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected.txt \
    -cluster $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected-rep.txt

* Cluster images and select the best group for the given image (need more testing).
  $PROJECT_BUILD/MyCImgWidgets/image_list_query -s 100 -n 20 \
    -list $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected.txt \
    -cluster $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected-rep.txt \
    -template $PROJECT_DATA/test_data/casimage_selected/357.jpg

Command to build single app from console (for testing purpose):
  g++ -o $PROJECT_BUILD/MyCImgWidgets/image_list_query \
    $PROJECT_SRC/MyCImgWidgets/app/image_list_query.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageWindow.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageListCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageListWindow.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageList.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/db/MyImageQuery.cpp \
    -I$PROJECT_SRC/MyCImgWidgets/include \
    -lX11 -lpthread -lm -O3
*/
