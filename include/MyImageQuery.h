#ifndef MYIMAGEQUERY_H
#define MYIMAGEQUERY_H

#include "queryhaar.h"

#include "MyImageAlgorithm.h"
#include "MyImageList.h"

template<typename T>
class MyImageQuery : public MyQueryAlgorithm<T>
{
public:
  MyImageQuery();
  ~MyImageQuery();

  void indexImageList(MyImageList<T> imagelist);
  void clusterImagesAcordingToRepresentatives(MyImageList<T> *imagelist, vector< CImg<T> > represen);
  vector<int> findBestMatchesForImage(CImg<T> image, int number);

  //Function required by parent class
  vector<int> queryImage  (CImg<T> image_tpl, int number, float param1, float param2);
  void whoami();
  
private:
  HaarIface haarDB;
};

#endif
