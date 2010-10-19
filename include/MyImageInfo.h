#ifndef MYIMAGEINFO_H
#define MYIMAGEINFO_H

#include "util.h"

template <class T>
class MyImageInfo
{
public:
  CImg<T>               image;
  CImg<unsigned char>   mask;
  bool                  maskChanged;
  string                fileName;
  int                   db_id;
  int                   stat;

  MyImageInfo()                                               : maskChanged(false) {}
  MyImageInfo(CImg<T> img)                                    : maskChanged(false), image(img), mask(img.width(),img.height(),1,3,0) {}
  MyImageInfo(CImg<T> img, string fName)                      : maskChanged(false), image(img), mask(img.width(),img.height(),1,3,0), fileName(fName) {}
  MyImageInfo(CImg<T> img, CImg<T> msk, string fName)         : maskChanged(false), image(img), mask(msk), fileName(fName) {}
  MyImageInfo(CImg<T> img, CImg<T> msk, string fName, int id) : maskChanged(false), image(img), mask(msk), fileName(fName), db_id(id) {}
};
 
#endif
