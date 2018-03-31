#include <opencv-3.3.1/opencv2/opencv.hpp>
#include <iostream>
#include <string>  
  
using namespace cv;  
  
int main()  
{  
    Mat img;  
    std::vector<Rect> people;
    VideoCapture cam(0);
    if (!cam.isOpened())
    {
        std::cout<<"error!"<<std::endl;
        exit(0);
    }
    

    for(;waitKey(1)!=27;cam>>img)
  {
    //定义HOG对象，采用默认参数，或者按照下面的格式自己设置  
    HOGDescriptor defaultHog;  
        //(cv::Size(64, 128), cv::Size(16, 16), cv::Size(8, 8),   
                                //cv::Size(8, 8),9, 1, -1,   
                                //cv::HOGDescriptor::L2Hys, 0.2, true,   
                                //cv::HOGDescriptor::DEFAULT_NLEVELS);  
  
    //设置SVM分类器，用默认分类器  
    defaultHog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());  
  
    //对图像进行多尺度行人检测，返回结果为矩形框  
    defaultHog.detectMultiScale(img, people,0,Size(8,8),Size(0,0),1.03,2);  
  
    //画长方形，框出行人  
    for (int i = 0; i < people.size(); i++)  
    {  
        Rect r = people[i];  
        rectangle(img, r.tl(), r.br(), Scalar(0, 0, 255), 3);  
    }  
    imshow("detect", img);  
  }
    return 0;  
}  