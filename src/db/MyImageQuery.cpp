#include "MyImageQuery.h"

//public:

template<typename T>
MyImageQuery<T>::MyImageQuery() {}

template<typename T>
MyImageQuery<T>::~MyImageQuery() {

} 

template<typename T>
vector<int> MyImageQuery<T>::queryImage(CImg<T> image_tpl, int number, float param1, float param2)
{
  return findBestMatchesForImage(image_tpl,number);
}

template<typename T>
void MyImageQuery<T>::whoami() {
  printf("whoami: MyImageQuery\n");
}

template<class T>
void MyImageQuery<T>::indexImageList(MyImageList<T> imagelist)
{
  for (int i=0; i < imagelist.size(); i++) {
    MyImageInfo<T> imageinfo = imagelist.at(i);
    CImg<T> image = imageinfo.image;
    image.resize(-100,-100,1,3);
    haarDB.indexImage(i,image);
    printf("\rIndexing images .................... [ %4d ]",i);
  }
  printf("\n");
}

template<class T>
void MyImageQuery<T>::clusterImagesAcordingToRepresentatives(MyImageList<T> *imglist, vector< CImg<T> > represen)
{
  vector < vector< std::pair<double,int> > > clusters;

  int number_of_cluster=0;

  for (int i=0; i < represen.size(); i++)
  {
    multimap<double, int> ordermap = haarDB.bestMatchesForImageWithScores(represen[i], imglist->size(), ScannedSketch);

    vector< std::pair<double,int> > ordervec( ordermap.begin(), ordermap.end() );
    clusters.push_back(ordervec);
    number_of_cluster++;
    printf("\rClustering images into group ....... [ %4d ]",i+1);
  }
  //int img_match = 0;
  //vector<int> pos(number_of_cluster,0);
  for (int yy=0; yy<imglist->size(); yy++)
    for (int xx=0; xx<number_of_cluster; xx++)
      if (imglist->score[ clusters[xx][yy].second ] > clusters[xx][yy].first) {
        imglist->score[ clusters[xx][yy].second ] = clusters[xx][yy].first;
        imglist->group[ clusters[xx][yy].second ] = xx+1;
      }
  imglist->number_of_groups = number_of_cluster;
  printf("\n");
}

template<class T>
vector<int> MyImageQuery<T>::findBestMatchesForImage(CImg<T> image, int number)
{
  this->setNumberOfQueryImages(number);
  return haarDB.bestMatchesForImage(image, number, ScannedSketch);
}

//Class instantiation
template class MyImageQuery<unsigned char>;


