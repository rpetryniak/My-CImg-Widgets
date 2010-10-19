#include "MyImageListWindow.h"
#include "MyImageCanvas.h"
#include "MyImageWindow.h"

template<class T>
void MyImageListWindow<T>::show()
{
  printf("Preparing image viewer ............. \n");

  char title[100];
  int  clicked_x=-1,clicked_y=-1, selection=-1;
  bool redraw_canvas=false, redraw_panel=false, numberOfImagesChanged=false;
  bool algorithmParamChanged=false, numberOfQueryImagesChanged=false;

  CImg<T> selimg, selmsk;

  int size   = listCanvas->getThumbnailSize();
  
  MyImageList<T>* imageList = listCanvas->getImageList();
  float algorithmParam    = imageList->processingAlgorithm->getParam1();
  int numberOfQueryImages = imageList->queryAlgorithm->getNumberOfQueryImages();

  while (!this->is_closed() && this->key()!=cimg::keyESC && this->key()!=cimg::keyQ)
  {
    this->wait();

    if (this->key()) {
      switch (this->key()) {
        case cimg::keyF1:         listCanvas->showHelp(); redraw_canvas=true; break;
        case cimg::keyARROWDOWN:  y_pos-=scroll_step;     redraw_canvas=true; break; //scrool down
        case cimg::keyARROWUP:    y_pos+=scroll_step;     redraw_canvas=true; break; //scrool up
        case cimg::keyARROWLEFT:  size-=size_step;        listCanvas->setThumbnailSize(size); redraw_panel =true;  break; //decrease thumbnail size
        case cimg::keyARROWRIGHT: size+=size_step;        listCanvas->setThumbnailSize(size); redraw_panel =true;  break; //increase thumbnail size

        case cimg::keyM:          imageList->switchLocalMaskVisibility();             redraw_panel =true;  break;

        case cimg::keyR:          imageList->resetOrderList();                        redraw_panel = true;  break;  //reset search result and marker
        case cimg::keyG:          imageList->showInGroups();                          redraw_panel = true;  break;
        case cimg::keyA:          imageList->processAllVisibleImages(algorithmParam); redraw_panel = true;  break;
      }
    }

    //Mouse wheel changed
    if (this->wheel()) {
      if (this->is_keySHIFTLEFT()) {
        algorithmParam+=(this->wheel());
        algorithmParam = algorithmParam<=0?1:algorithmParam;
        algorithmParamChanged=true;
        imageList->processingAlgorithm->setParam1(algorithmParam);
        redraw_panel=true;
        sprintf(title,"Algorithm param: %f",algorithmParam);
        this->set_title(title);
      } else
      if (this->is_keyCTRLLEFT()) {  //change number of query images
        numberOfQueryImages+=(this->wheel());
        numberOfQueryImages = numberOfQueryImages<=0?1:numberOfQueryImages;
        numberOfQueryImagesChanged=true;
        redraw_panel=true;
        sprintf(title,"Number of query images: %d",numberOfQueryImages);
        this->set_title(title);
      } else
    if (this->is_keyCTRLRIGHT()) {   //change thumbnail size
        size+=(this->wheel()*15);  redraw_panel=true;
        listCanvas->setThumbnailSize(size);
      } else {                      //scrool thumbnails panel
        y_pos+=(this->wheel()*15); redraw_canvas=true;
      }
    }

    //Mouse button pressed
    if (this->button()) {
      clicked_x = this->mouse_x();
      clicked_y = this->mouse_y();
      selection = listCanvas->select_image(clicked_x,clicked_y-y_pos);
      redraw_panel=true;
    }

    //Window size changed
    if (this->is_resized()) {
      listCanvas->resize(this->window_width(),this->window_height()); //.fill(fillColor);
      this->resize(false);
      listCanvas->setWidth( this->window_width() );
      redraw_panel=true;
    }
    /*
    if (this->mouse_x>=0 && this->mouse_y>=0) { // Mouse pointer is over the image
      const int x = this->mouse_x, y = this->mouse_y;
    }
    */

    //Right-mouse button pressed
    if (this->button()&2) {
      if (selection>=0) {
        selimg = imageList->at(selection).image;
        selmsk = imageList->at(selection).mask;

        MyImageCanvas<T> maincanvas(selimg);
        maincanvas.setMask(selmsk, red);
        maincanvas.setProcessingAlgorithm(imageList->processingAlgorithm, algorithmParam);
        maincanvas.redraw();
        MyImageWindow<T> mainwindow(&maincanvas,"[Type F1 for help]");
        mainwindow.show();
        if (maincanvas.isMaskChanged()) {
          imageList->at(selection).mask = maincanvas.getMask();
          imageList->at(selection).maskChanged = true;
          if (algorithmParam != maincanvas.getAlgorithmParam()) {
            algorithmParam = maincanvas.getAlgorithmParam();
            algorithmParamChanged = true;
          }
          redraw_panel = true;
        }
      }
    }

    //Middle-mouse button pressed, or number of results changed
    if (this->button()&4 || numberOfQueryImagesChanged || algorithmParamChanged) {
      if (selection>=0) {
        selimg = imageList->at(selection).image;
        if (this->is_keyCTRLLEFT() || numberOfQueryImagesChanged) {
          imageList->orderList = imageList->queryAlgorithm->queryImage(selimg, numberOfQueryImages);
          y_pos=0;
          numberOfQueryImagesChanged=false;
        }
        else {
          imageList->at(selection).mask = imageList->processingAlgorithm->processImage(selimg,algorithmParam );
          imageList->at(selection).maskChanged = true;
        }
        algorithmParamChanged=false;
      }
    }

    //Redraw thumbnail panel and canvas
    if (redraw_panel)
      listCanvas->prepare_image_panel(selection);
    if (redraw_panel || redraw_canvas) {
      listCanvas->redraw(0,y_pos);
      listCanvas->display(*this);
    }

    //Clear temporary settings
    if (redraw_panel || redraw_canvas) {
      redraw_panel  = false;
      redraw_canvas = false;
      this->set_key(0);
      this->set_wheel();
      this->set_button();
    }
  }
}

//Class instantiation
template class MyImageListWindow<unsigned char>;
