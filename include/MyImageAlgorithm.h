#ifndef MYIMAGEALGORITHM_H
#define MYIMAGEALGORITHM_H

#include <vector>

#include "util.h"

template<typename T>
class MyImageAlgorithm
{
  public:
    virtual CImg<T>     processImage(CImg<T> image_in,  float param1=0, float param2=0, float param3=0) = 0;
    virtual vector<int> queryImage  (CImg<T> image_tpl, int count=20, float param1=0, float param2=0) = 0;
    
    virtual void whoami() = 0;
};
template<typename T>

class MyProcessingAlgorithm : public MyImageAlgorithm<T>
{
  public:
    enum ResultImageType {
      ResultImageBinary,
      ResultImageGrayscale
    };
  
  public:
    MyProcessingAlgorithm() : algParam1(128), resultImageType(ResultImageGrayscale) {}
    virtual CImg<T> processImage(CImg<T> image_in,  float param1=0, float param2=0, float param3=0) = 0;
    virtual vector<int> queryImage  (CImg<T> image_tpl, int count=20, float param1=0, float param2=0) {
      throw CImgException("MyProcessingAlgorithm: use 'processImage' function.\n");
      vector<int> emptyVec;
      return emptyVec;
    }
    virtual void setResultImageType (ResultImageType resultType) { resultImageType = resultType; }
    virtual ResultImageType getResultImageType() { return resultImageType; }
    virtual void setParam1(float p1) { algParam1=p1; }
    virtual float getParam1() { return algParam1; }
    virtual void whoami() = 0;
    
  private:
    float algParam1;
    ResultImageType resultImageType;
  
};
template<typename T>

class MyQueryAlgorithm : public MyImageAlgorithm<T>
{
  public:
    MyQueryAlgorithm() : numberOfQueryImages(20) {}
    virtual CImg<T> processImage(CImg<T> image_in,  float param1=0, float param2=0, float param3=0) {
      throw CImgException("MyQueryAlgorithm: use 'queryImage' function.\n");
      return CImg<T>(image_in.width(),image_in.height(),1,3,0);
    }
    virtual vector<int> queryImage  (CImg<T> image_tpl, int number=20, float param1=0, float param2=0) = 0;
    virtual void setNumberOfQueryImages(int n) { numberOfQueryImages=n; }
    virtual int getNumberOfQueryImages() { return numberOfQueryImages; }
    virtual void whoami() = 0;
  private:
    int numberOfQueryImages;
};


template<typename T>
class DoNothingAlgorithm : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2, float param3) {
      printf("MyProcessingAlgorithm: processImage");
      return CImg<T>(image_in.width(),image_in.height(),1,3,0);
    }
    void whoami() {
      printf("whoami: MyProcessingAlgorithm");
    }
};

template<typename T>
class QueryNothingAlgorithm : public MyQueryAlgorithm<T>
{
  public:
    vector<int> queryImage(CImg<T> image_tpl, int number, float param1, float param2) {
      printf("QueryNothingAlgorithm: queryImage\n");
      vector<int> emptyVec;
      return emptyVec;
    }
    void whoami() {
      printf("whoami: QueryNothingAlgorithm");
    }
};


template<typename T>
class ThresholdAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageBinary );
      return image_in.get_threshold((int)param1);
    }
    void whoami() {
      printf("whoami: ThresholdAlg\n");
    }
};

template<typename T>
class ErodeAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      return image_in.get_erode((int)param1);
    }
    void whoami() {
      printf("whoami: ErodeAlg\n");
    }
};

template<typename T>
class DilateAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      return image_in.get_dilate((int)param1);
    }
    void whoami() {
      printf("whoami: DilateAlg\n");
    }
};

template<typename T>
class DericheAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      return image_in.get_deriche((int)param1);
    }
    void whoami() {
      printf("whoami: DericheAlg\n");
    }
};

template<typename T>
class GradientAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      CImgList<> grad               = image_in.get_gradient("xy",(int)param1);
      CImg<unsigned char> grad_out  = (grad[0].pow(2) + grad[1].pow(2)).sqrt().normalize(0,255);
      return grad_out;
    }
    void whoami() {
      printf("whoami: GradientAlg\n");
    }
};

template<typename T>
class SharpenAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      return image_in.get_sharpen((int)param1);
    }
    void whoami() {
      printf("whoami: SharpenAlg\n");
    }
};

template<typename T>
class BlurMedianAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      return image_in.get_blur_median(param1);
    }
    void whoami() {
      printf("whoami: BlurMedianAlg\n");
    }
};

template<typename T>
class BlurAlg : public MyProcessingAlgorithm<T>
{
  public:
    CImg<T> processImage(CImg<T> image_in,  float param1, float param2=0, float param3=0) {
      this->setParam1(param1);
      this->setResultImageType( MyProcessingAlgorithm<T>::ResultImageGrayscale );
      return image_in.get_blur(param1);
    }
    void whoami() {
      printf("whoami: BlurAlg\n");
    }
};



#endif
