#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

#include "MyImageList.h"
#include "MyImageDB.h"

template<class T>
void MyImageList<T>::switchGlobalMaskVisibility() {
  draw_global_mask = !draw_global_mask;
}

template<class T>
void MyImageList<T>::switchLocalMaskVisibility() {
  draw_local_mask  = !draw_local_mask;
}

template<class T>
void MyImageList<T>::resetOrderList() {
  //draw_global_mask = false;
  //draw_local_mask  = false;
  orderList = vector<int>(this->size());
  for (int i = 0; i < orderList.size(); ++i)
    orderList[i] = i;
}

template<class T>
void MyImageList<T>::showInGroups() {
  int groupMax = -1;
  groupColor.clear();
  for (int i = 0; i < group.size(); ++i)
    if (group[i] > groupMax) groupMax = group[i];
    for (int i = 0; i <= groupMax; i++)
      groupColor.push_back(random_rgb());
    //return groupMax;
  
  //Group sorting:
  multimap<double,int> orderInGroups;
  for (vector<int>::iterator it = orderList.begin(); it != orderList.end(); it++) {
    int index = *it;
    
    int    group  = this->group.at(index);
    double score  = this->score.at(index);
    
    orderInGroups.insert( pair<double,int>(1000*group+score,index) );
  }
  vector<int> orderInGroupsTmp(orderInGroups.size(),0); int i_tmp=0;
  for (multimap<double,int>::iterator it = orderInGroups.begin(); it != orderInGroups.end(); ++it)
    orderInGroupsTmp[i_tmp++] = it->second;
  
  orderList = orderInGroupsTmp;
}

template<class T>
void MyImageList<T>::loadImagesFromListFile(const char *file_l) {
  string fline;
  ifstream imgsfile(file_l);

  if (imgsfile.is_open())
  {
    int number_of_images=0;
    while (!imgsfile.eof())
    {
      getline (imgsfile,fline);
      if (!fline.empty()) {
        printf("\rLoading images ..................... [ %4d ]",number_of_images);
        CImg<unsigned char> loadimg(fline.c_str());
        this->push_back( MyImageInfo<unsigned char>(loadimg.get_resize(-100,-100,1,3),extractFilename(fline)) );
        this->orderList.push_back(number_of_images);
        this->group.push_back(0);
        this->score.push_back(0);
        number_of_images++;
      }
    }
    printf("\n");
    imgsfile.close();
  }
  else {
    printf("Unable to open file\n");
    exit(-1);
  }
}

template<class T>
void MyImageList<T>::processAllVisibleImages(float param1)
{
  int index=0, i=1;
  for (vector<int>::iterator it = this->orderList.begin(); it != this->orderList.end(); it++) {
    index = *it;
    printf("\rProcessing images .................. [ %4d ]",i++);
    this->at(index).mask = processingAlgorithm->processImage(this->at(index).image, param1);
    this->at(index).maskChanged = true;
  }
  printf("\n");
}

//Class instantiation
template class MyImageList<unsigned char>;
