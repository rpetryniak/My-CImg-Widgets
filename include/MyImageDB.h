#ifndef MYIMAGEDB_H
#define MYIMAGEDB_H

#include <sqlite3.h>

#include "MyImageInfo.h"
#include "MyImageList.h"

#include "util.h"

class MyImageDB {
public:
  MyImageDB();
  MyImageDB(const char* db_name);
  ~MyImageDB();

  void open(const char* db_name);
  void close();

  void addImage (MyImageInfo<unsigned char> imageinfo);
  void addImages(MyImageList<unsigned char> imagelist);
  MyImageList<unsigned char> getImages();

  void update(MyImageList<unsigned char> imagelist);
  int update(MyImageInfo<unsigned char> imageinfo);
  void deleteAll();

private:  
  void startTransaction();
  void endTransaction();

  sqlite3 *db;  // Database pointer
};

#endif
