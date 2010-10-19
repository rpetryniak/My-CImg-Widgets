#ifndef MYIMAGELIST_H
#define MYIMAGELIST_H

#include <vector>

#include "MyImageInfo.h"
#include "MyImageAlgorithm.h"
#include "util.h"

template<typename T>
class MyImageList : public vector< MyImageInfo<T> >
{
public:
  bool draw_global_mask;
  bool draw_local_mask;
  CImg<unsigned char> global_mask;
  vector<int>         orderList;

  vector<double>      score;
  vector<int>         group;
  vector<rgb>         groupColor;
  int                 number_of_groups;

  MyProcessingAlgorithm<T>  *processingAlgorithm;
  MyQueryAlgorithm<T>       *queryAlgorithm;
  
  MyImageList() : draw_global_mask(false), draw_local_mask(true), number_of_groups(0) {
    processingAlgorithm = new DoNothingAlgorithm<T>();
    queryAlgorithm      = new QueryNothingAlgorithm<T>();
  }

  void switchGlobalMaskVisibility();
  void switchLocalMaskVisibility();
  void resetOrderList();
  void showInGroups();
  void loadImagesFromListFile(const char *file_l);
  void loadImagesFromDatabase(const char *db_name);
  void saveImagesToDatabase(const char *db_name);
  void processAllVisibleImages(float param1);

  void setProcessingAlgorithm(MyProcessingAlgorithm<T> *alg) { processingAlgorithm = alg; }
  void setQueryAlgorithm(MyQueryAlgorithm<T> *query) { queryAlgorithm = query; }
};

#endif
