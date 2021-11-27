/*
This file is used for color recognition
*/
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <string>
#include <unistd.h>
#include "serial/include/serial/serial.h"
using namespace cv;
using namespace std;
// HSV: Hue, Saturation (Xianyan), LIghtness Value
Mat src,hsv, AfterHistProcessed, ResizedMask; 
Mat beforeclose, afterclose,kernel;
//#define EMITTED_SIGNAL

enum ColorType {Red,White,Black,Green};

struct col{
    Scalar low, high; // low and high threshold for the color
    Mat mean, sd;
    Mat mask;
} lst[4];

Mat img;
vector<Mat> hsvSplit;

void handle_output(int b, int r, int w);

int main(int argc, char* argv[]){
#ifdef EMITTED_SIGNAL
        string port("/dev/ttyUSB0");
        serial::Serial my_serial(port);

        if(!my_serial.isOpen()){
            cout << "Not opened" << endl;
            return 0;
        }
#endif

    //src=imread("/home/johnny/Green.png"); // source file of the image
    VideoCapture capture(0);   //2; laptop
    while(true)
    { 
        
        capture.read(src);
        if (src.empty())
        {
            printf("could not show the image");
            return -1;
        }

        // imshow("camera",src);
        // Rect r1(50,50,375,375);         // set the size of the mask
        // ResizedMask=Mat::zeros(src.size(),CV_8UC1) ;
        // ResizedMask(r1).setTo(255);
        // img = src(r1);
        
        //GaussianBlur(img, img, Size(5, 5), 0);
        //imshow("afterG",img);
        
        GaussianBlur(src, img, Size(5, 5), 0);

        //img = src;
        imshow("After Blur", img);
        
        //bgr
        lst[Black].low = Scalar(0,0,0);        
        lst[Black].high = Scalar(120,120,120);       
        lst[Red].low = Scalar(0, 0, 150);
        lst[Red].high = Scalar(130, 130, 255);
        lst[White].low = Scalar(135,137,132);
        lst[White].high = Scalar(165,158,163);
        lst[Green].low = Scalar(0,130,0);
        lst[Green].high = Scalar(140, 255, 160);


        inRange(img,lst[Black].low, lst[Black].high, lst[Black].mask);
        imshow("Black mask",lst[Black].mask);

        inRange(img, lst[Red].low, lst[Red].high, lst[Red].mask);
        imshow("Red mask",lst[Red].mask);

        inRange(img, lst[White].low, lst[White].high, lst[White].mask);
        imshow("white mask",lst[White].mask);

        inRange(img, lst[Green].low, lst[Green].high, lst[Green].mask);
        imshow("Green mask",lst[Green].mask);

        
        meanStdDev(lst[Black].mask,lst[Black].mean,lst[Black].sd);
        cout<<"Black:\t"<<lst[Black].mean<<" "<< lst[Black].mean.at<double>(0,0) <<endl;
        meanStdDev(lst[Red].mask,lst[Red].mean,lst[Red].sd);
        cout<<"Red:\t"<<lst[Red].mean<<" "<< lst[Red].mean.at<double>(0,0) <<endl;
        meanStdDev(lst[White].mask,lst[White].mean,lst[White].sd);
        cout<<"White:\t"<<lst[White].mean<<" "<< lst[White].mean.at<double>(0,0) <<endl;
        meanStdDev(lst[Green].mask,lst[Green].mean,lst[Green].sd);
        cout<<"Green:\t"<<lst[Green].mean<<" "<< lst[Green].mean.at<double>(0,0) <<endl;        

        auto b = lst[Black].mean.at<double>(0,0);
        auto r = lst[Red].mean.at<double>(0,0);
        auto w = lst[White].mean.at<double>(0,0);
        auto ble = lst[Green].mean.at<double>(0,0);

        #ifdef EMITTED_SIGNAL
            while(my_serial.read()!=""){
                if(b > r && b > w){
                    my_serial.write("1");
                } else if(w > r && w > b){
                    my_serial.write("2");
                } else{
                    my_serial.write("3");
                }     
            } 
        #else
            if(b > r && b > w && b > ble){
                cout << "Black" <<endl;
            } else if(w > r && w > b && w > ble){
                cout << "White" << endl;
            } else if(r > w && r > b && r > ble){
                cout << "Red" << endl;
            } else{
                cout << "No object detected" << endl;
            }
        /*
            if(b > half){
                cout << "Black" <<endl;
                cout << b / (b + w + r) << endl;
            } else if(w > half){
                cout << "White" << endl;
                cout << w / (b + w + r) << endl;
            } else  if(r>half){
                cout << "Red" << endl;
                cout << r / (b + w + r) << endl;
            } else{
                cout<<"No object detected." << endl;
            }
        */ 
        #endif

        char c = waitKey(100);
        if (c==27){         // end the program by pressing "ESC"
            break;
        }
    } 
    return 0;
}