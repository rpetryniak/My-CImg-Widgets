#include "MyImageWindow.h"

template<class T>
void MyImageWindow<T>::refresh()
{
  canvas->display(*this);
}

template<class T>
void MyImageWindow<T>::setImageCanvas(MyImageCanvas<T> *canvas)
{
  this->canvas = canvas;
}

template<class T>
MyImageCanvas<T> * MyImageWindow<T>::getImageCanvas()
{
  return canvas;
}

template<class T>
void MyImageWindow<T>::zoomIn()
{
  zoomFactor *= 1.5;
  this->resize((int)(orginalSizeX*zoomFactor),(int)(orginalSizeY*zoomFactor));
}

template<class T>
void MyImageWindow<T>::zoomOut()
{
  zoomFactor /= 1.5;
  this->resize((int)(orginalSizeX*zoomFactor),(int)(orginalSizeY*zoomFactor));
}

template<class T>
void MyImageWindow<T>::zoom100()
{
  zoomFactor = 1.0;
  this->resize(orginalSizeX,orginalSizeY);
}

template<class T>
//void MyImageWindow<T>::run()
void* MyImageWindow<T>::run(void* p)
{
  MyImageWindow<unsigned char>* window = (MyImageWindow<unsigned char>*) p;
  MyImageCanvas<unsigned char>* canvas = window->getImageCanvas();
  
  char title[100];
  float algorithmParam = canvas->getAlgorithmParam();

  bool left_button_pressed = false;
  bool force_refresh = false;
  bool algorithmParamChanged = false;
  
  while (!window->is_closed() && window->key()!=cimg::keyESC && window->key()!=cimg::keyQ)
  {
    if (window->wait_for_event) window->wait();
    else window->wait(30);

    //Check state of external window and auto refresh main window (work only if 'wait_for_event' = false)
    if (canvas->isHistogramVisible() && canvas->isHistogramDisplayClosed()) { canvas->hideHistogram();  force_refresh = true; }
    if (canvas->isProfileVisible()   && canvas->isProfileDisplayClosed())   { canvas->hideProfile();    force_refresh = true; }
    if (canvas->isProfileXVisible()  && canvas->isProfileXDisplayClosed())  { canvas->hideProfileX();   }
    if (canvas->isProfileYVisible()  && canvas->isProfileYDisplayClosed())  { canvas->hideProfileY();   }

    if (window->show_menu_in_window && window->key()) {
      switch (window->key()) {
        case cimg::keyF1:   canvas->showHelp();             break;
        case cimg::keyF2:   canvas->showInfo();             break;
        case cimg::keyM:    canvas->showMask();             break;
        case cimg::keyE:    canvas->cleanMask();            break;
        case cimg::keyI:    canvas->showImage();            break;
        case cimg::keyG:    canvas->showGrid();             break;
        case cimg::keyC:    canvas->showCoordinates();      break;
        case cimg::keyT:    canvas->showCircleTool();       break;
        case cimg::keyK:    canvas->showClosedCurveTool();  break;
        case cimg::keyP:    canvas->showPaint();            break;
        case cimg::keyH:    canvas->showHistogram();        break;
        case cimg::keyR:    canvas->showProfile();          break;
        case cimg::keyX:    canvas->showProfileX();         break;
        case cimg::keyY:    canvas->showProfileY();         break;
        case cimg::keyESC:  return (void*)0;                break;
        case cimg::keyQ:    return (void*)0;                break;
        
        case cimg::keyS: {
          canvas->hideCoordinates();
          const CImg<int> selection = canvas->get_select(*window,2,0,red);
        }
        break;
      }
      window->set_key(0);
      force_refresh = true; //canvas->display(*window);
    }
    
    if (window->mouse_x()>=0 && window->mouse_y()>=0) { // Mouse pointer is over the image
      const int mouse_x = window->mouse_x() / window->zoomFactor, mouse_y = window->mouse_y() / window->zoomFactor;
      
      if (window->button()) {
        if ( canvas->isPaintVisible() ) {
          canvas->paint(mouse_x,mouse_y);
        }
        if (canvas->isHistogramVisible()) {
          CImg<int> selection = canvas->get_select(*window,2,0,red);
          canvas->drawHistogram(selection);
        }
        if (canvas->isProfileVisible() ) {
          CImg<int> selection = canvas->get_select(*window,1,0,red);
          canvas->drawProfile(selection);
        }
      }
      
      //Left button
      if (window->button()&1) {
        left_button_pressed = true;
        if ( canvas->isCircleToolVisible() ) {
          canvas->paintCircle(mouse_x,mouse_y,window->circleSize);
        }
        if ( canvas->isClosedCurveToolVisible() ) {          
          canvas->createClosedCurve(mouse_x,mouse_y,false);
        }
      }
      else {
        if (left_button_pressed) {
          left_button_pressed = false;
          window->onLeftButtonRelease();
        }
      }
      
      //Right button
      if (window->button()&2) {
        if ( canvas->isCircleToolVisible() ) {
          canvas->paintCircle(mouse_x,mouse_y,window->circleSize,true);
        }
        if ( canvas->isClosedCurveToolVisible() ) {
          canvas->createClosedCurve(mouse_x,mouse_y,true);
        }
      }
      //Middle button
      if (window->button()&4 || algorithmParamChanged) {
        if ( canvas->isClosedCurveToolVisible() ) {
          canvas->finishCurve();
        } else {
          canvas->setMask( canvas->processingAlgorithm->processImage(canvas->getImage(),algorithmParam ) );
          canvas->redraw();
          force_refresh = true;
        }
        algorithmParamChanged=false;
      }
      //Mouse wheel changed
      if (window->wheel()) {
        if (window->is_keySHIFTLEFT()) {
          algorithmParam+=(window->wheel());
          algorithmParam = algorithmParam<=0?1:algorithmParam;
          algorithmParamChanged=true;
          canvas->processingAlgorithm->setParam1(algorithmParam);
          canvas->setAlgorithmParam(algorithmParam);
          sprintf(title,"Param: %f",algorithmParam);
          window->set_title(title);
        } else
          if (window->is_keyCTRLLEFT()) {
            window->circleSize+=(window->wheel()*1);
            if (window->circleSize<2) window->circleSize = 2;
            sprintf(title,"Circle: %d",window->circleSize);
            window->set_title(title);
          }
          window->set_wheel();
      }
      
      if ( canvas->isCoordinatesVisible() ) {
        canvas->drawCoordinates(mouse_x,mouse_y);
      }
      if ( canvas->isProfileXVisible() ) {
        canvas->drawProfileX(mouse_x,mouse_y);
      }
      if ( canvas->isProfileYVisible() ) {
        canvas->drawProfileY(mouse_x,mouse_y);
      }
      if (canvas->isCoordinatesVisible() || canvas->isPaintVisible() || canvas->isCircleToolVisible() || canvas->isClosedCurveToolVisible() ) {
        force_refresh = true; //canvas->display(*window);
      }
    }
    if (force_refresh) {
      canvas->display(*window);
      force_refresh = false;
    }
  }
}

//Class instantiation
template class MyImageWindow<unsigned char>;
