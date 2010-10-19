#include "MyImageList.h"
#include "MyImageListCanvas.h"
#include "MyImageListWindow.h"

#include "MyImageDB.h"

int main(int argc,char **argv)
{
  //Input-output options:
  const char
    *file_l     = cimg_option("-list",     (char*)0, "Image list"),
    *db_name    = cimg_option("-db_name",  (char*)0, "Database file");

  const int
    source_type = cimg_option("-source", 1, "Image source type (1-List file, 2-Database)");

  const int
    db_opt      = cimg_option("-db_opt", 0, "Database operation (0-Update changed, 1-Add images, 2-Delete all)");

  if (db_name==(char*)0                     ) throw CImgException("Please specify database file (option -db_name)");
  if (file_l ==(char*)0 && source_type == 1 ) throw CImgException("Please specify file with image paths (option -list)");

  MyImageDB imageDB;
  if (db_name!=(char*)0) imageDB.open(db_name);
  if (db_opt == 2)
    imageDB.deleteAll();

  MyImageList<unsigned char> imageList;
  
  //Load images from list or database
  if (source_type == 1) 
    imageList.loadImagesFromListFile(file_l);
  else if (source_type == 2) {
    imageList = imageDB.getImages();
  }
  else
    throw CImgException("Bad '-source' option (choose 1 or 2)");
  
  MyImageListCanvas<unsigned char> imageListCanvas(800,600,1,3,200);
  imageListCanvas.setImageList(&imageList);
  imageListCanvas.prepare_image_panel();
  imageListCanvas.redraw();

  //Set algorithm
  ThresholdAlg<unsigned char> thresholdAlg;
  thresholdAlg.setParam1(128);
  imageList.setProcessingAlgorithm(&thresholdAlg);

  MyImageListWindow<unsigned char> imageListWindow(&imageListCanvas);

  //Database operation before closing application
  if (db_opt == 1)
    imageDB.addImages(imageList); //add loaded images to database
  else if (db_opt == 0)
    imageDB.update(imageList);    //save only changed images to database

  imageDB.close();

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
  $PROJECT_BUILD/MyCImgWidgets/image_db_viewer -source 1 -db_opt 1 \
    -list $PROJECT_SRC/MyCImgWidgets/test_data/casimage-selected.txt \
    -db_name $PROJECT_TMP/MyCImgWidgets/image_db_casimage_selected.sqlite3

* Working on selected image database. After finishing write only changed mask images.
  $PROJECT_BUILD/MyCImgWidgets/image_db_viewer -source 2 \
    -db_name $PROJECT_TMP/MyCImgWidgets/image_db_casimage_selected.sqlite3

Command to build single app from console (for testing purpose):
  g++ -o $PROJECT_BUILD/MyCImgWidgets/image_db_viewer \
    $PROJECT_SRC/MyCImgWidgets/app/image_db_viewer.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageWindow.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageListCanvas.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageListWindow.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/gui/MyImageList.cpp \
    $PROJECT_SRC/MyCImgWidgets/src/db/MyImageDB.cpp \
    -I$PROJECT_SRC/MyCImgWidgets/include \
    -lX11 -lpthread -lm -lsqlite3 -O3
*/
