#include "MyImageListCanvas.h"

template<class T>
void MyImageListCanvas<T>::setImageList(MyImageList<T>* _imageList)
{
  imageList = _imageList;
}

template<class T>
MyImageList<T>* MyImageListCanvas<T>::getImageList() {
  return imageList;
}

template<class T>
void MyImageListCanvas<T>::redraw(int x, int y) {
  this->fill(fillColor).draw_image(x,y,panelCanvas);
  if (helpVisible) this->draw_image(helpImage,0.9f);
}

template<class T>
void MyImageListCanvas<T>::setHelpText(const char* help)
{
  helpImage.assign(230,400,1,3,100).draw_text(5, 5, help, green);
}

template<class T>
void MyImageListCanvas<T>::showHelp()
{
  helpVisible = !helpVisible;
}

template<class T>
int MyImageListCanvas<T>::select_image(int selx, int sely)
{
  int col = (float)selx / (float)(size+border);
  int row = (float)sely / (float)(size+border);
  int max_col = (int)width/size;

  int selection = row * max_col + col;

  return imageList->orderList[selection];
}

template<class T>
void MyImageListCanvas<T>::prepare_image_panel(int sel)
{
  int row=0, col=0;
  int xpos=border, ypos=border;

  int max_col = (int)(width/size);
  int max_row = imageList->size()/max_col + 1;
  int height  = max_row * (size + border);

  CImg<unsigned char> image_panel(width,height,1,3,fillColor);

  int index=0, i=0;

  for (vector<int>::iterator it = imageList->orderList.begin(); it != imageList->orderList.end(); it++) {
    index = *it;
    col  = i%max_col;
    if (col==0 && i!=0) row++;
    xpos = border + col * (border+size);
    ypos = border + row * (border+size);

    MyImageInfo<unsigned char> imageInfo = imageList->at(index);

    CImg<unsigned char> tmp  = imageInfo.image;
    CImg<unsigned char> msk  = imageInfo.mask;
    string name              = imageInfo.fileName;

    float scale = -100 * (float)size/(float)max(tmp.width(),tmp.height());
    if (imageList->draw_local_mask)  cimg_forXY(                  msk,x,y)  if(                   msk(x,y)>=1   && tmp.containsXYZC(x,y)) { tmp.draw_point(x,y,red,0.4f); }
    if (imageList->draw_global_mask) cimg_forXY(imageList->global_mask,x,y) if(imageList->global_mask(x,y)>=1   && tmp.containsXYZC(x,y)) { tmp.draw_point(x,y,blue,0.4f); }

    int group     = imageList->group.at(index);
    if (group != 0) {
      unsigned char gcolor[3] = {imageList->groupColor[group].r, imageList->groupColor[group].g, imageList->groupColor[group].b};
      image_panel.draw_rectangle(xpos-(border/2),ypos-(border/2),xpos+size-(border/2),ypos+size-(border/2),gcolor,0.8f);
    }

    image_panel.draw_image(xpos,ypos,tmp.get_resize(scale,scale)).draw_text(xpos+5,ypos+5,name.c_str(),red,yellow,0.9f);

    if (sel==index) image_panel.draw_rectangle(xpos-(border/2),ypos-(border/2),xpos+size-(border/2),ypos+size-(border/2),red,0.9f,-1);
    i++;
  }

  panelCanvas = image_panel;
}

//Class instantiation
template class MyImageListCanvas<unsigned char>;
