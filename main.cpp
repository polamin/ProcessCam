//============================================================================
// Name        : Process.cpp
// Author      : prabhu
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <thread>         // std::thread
#include <mutex>
#include <vector>
#include <math.h>

#include <curl/curl.h>
#include "opencv2/opencv.hpp"
//#include "opencv2/gpu/gpu.hpp"
#include "Camera.h"
#include "Calibration.h"
#include "mainwindow.h"
#include <ctime>
#include <QApplication>
#include <QDebug>
#include "morphological.h"
#include "Add_Class/time_catch.h"


using namespace std;
using namespace cv;


std::mutex m1;
static cv::Mat processedImg,cpImg;
cv::Mat pImg;
cv::Mat stImg;
bool waitflg = false;


const int xLen = 19;
const int yLen = 19;
const int nstate = xLen * yLen;
const int ncams = 3;
//const int nped = 1;

bool START = false;

struct Target
{
    int x;
    int y;
    int dir;
};

Calibration calib;
vector<Camera> cameras;
vector<Target> targets;

mutex m;


//Add from bg subtraction
Ptr<BackgroundSubtractor> pMOG1; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG2; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG3; //MOG Background subtractor
Ptr<BackgroundSubtractor> pMOG4; //MOG Background subtractor
Mat static_fgMaskMOG;
Mat ptz1_fgMaskMOG;
Mat ptz2_fgMaskMOG;
Mat ptz3_fgMaskMOG;


int ix = 0; int iy = 0; double wX = -1; double wY = -1;

int getState(int x, int y, int xlen)
{
    return (x + y*xlen);
}

void correctState(double wx, double wy, int &sx, int &sy)
{
    sx = round(wx); sy = round(wy);

    if(sx > xLen)
    {
        sx = xLen;
    }
    else if(sx < 0)
    {
        sx = 0;
    }
    if(sy > yLen-1)
    {
        sy = yLen-1;
    }
    else if(sy < 0)
    {
        sy = 0;
    }
    //	cout<<"X: "<<sx<<", Y: "<<sy<<"\n";
}

vector<int> computeActions1(int sx, int sy, vector<Camera> camera, vector<Target> target)
{
    vector<int> optAct; optAct.clear();
    int opt1=-1, opt2=-1, opt3=-1;

    for(int a1=0; a1<camera[0].naction; a1++)
    {
        double summ=0;
        int state = getState(target[0].x,target[0].y,xLen);
        if(camera[0].FOV[a1][state])
        {
            opt1 = a1;
        }
    }
    for(int a2=0; a2<camera[1].naction; a2++)
    {
        double summ=0;
        int state = getState(target[0].x,target[0].y,xLen);
        if(camera[1].FOV[a2][state])
        {
            opt2 = a2;
        }
    }
    for(int a3=0; a3<camera[2].naction; a3++)
    {
        double summ=0;
        int state = getState(target[0].x,target[0].y,xLen);
        if(camera[2].FOV[a3][state])
        {
            opt3 = a3;
        }
    }
    optAct.push_back(opt1); optAct.push_back(opt2); optAct.push_back(opt3);

    return optAct;
}


vector<int> computeActions(int sx, int sy, vector<Camera> camera, vector<Target> target)
{
    vector<int> optAct; optAct.clear();
    int opt1=-1, opt2=-1, opt3=-1;

    double maxsum=0;
    for(int a1=0; a1<camera[0].naction; a1++)
        for(int a2=0; a2<camera[1].naction; a2++)
            for(int a3=0; a3<camera[2].naction; a3++)
            {
                double summ=0;
                for(int t=0; t<target.size(); t++)
                {

                    int state = getState(target[t].x,target[t].y,xLen);
                    if(camera[0].FOV[a1][state] || camera[1].FOV[a2][state] || camera[2].FOV[a3][state])
                    {
                        summ++;
                    }
                    if(summ > maxsum)
                    {
                        maxsum = summ;
                        opt1 = a1; opt2 = a2; opt3 = a3;
                    }
                }
            }

    optAct.push_back(opt1); optAct.push_back(opt2); optAct.push_back(opt3);

    return optAct;
}

int processing()
{
    //this_thread::sleep_for (std::chrono::seconds(3));
    cv::VideoCapture vscap;

    const std::string videoStreamAddress = "http://root:SCdevel2013@172.25.8.71/axis-cgi/mjpg/video.cgi?.mjpg"; //closed save

    waitflg=true;

    cv::VideoCapture vcap1, vcap2, vcap3, vcap4;
    cv::Mat image1,image2, image3, image4;
    cv::Mat show_n_save1,show_n_save2, show_n_save3, show_n_save4;
    const std::string videoStreamAddress1 = "http://root:SCdevel2013@172.25.8.76/axis-cgi/mjpg/video.cgi?.mjpg";
    const std::string videoStreamAddress2 = "http://root:SCdevel2013@172.25.8.77/axis-cgi/mjpg/video.cgi?.mjpg";
    const std::string videoStreamAddress3 = "http://root:SCdevel2013@172.25.8.78/axis-cgi/mjpg/video.cgi?.mjpg";
    const std::string videoStreamAddress4 = "http://root:SCdevel2013@172.25.8.71/axis-cgi/mjpg/video.cgi?.mjpg";

    if(!vscap.open(videoStreamAddress))
    {
        std::cout << "\nError opening static camera stream" << std::endl;
        return -1;
    }

    //open the video stream and make sure it's opened

    if(!vcap1.open(videoStreamAddress1)||!vcap2.open(videoStreamAddress2)||!vcap3.open(videoStreamAddress3)||!vcap4.open(videoStreamAddress4))
    {
        std::cout << "\n###Error opening one of the video stream" << std::endl;
        return -1;
    }

    //start bgsubtractor object
    pMOG1= new BackgroundSubtractorMOG();
    pMOG2= new BackgroundSubtractorMOG();
    pMOG3= new BackgroundSubtractorMOG();
    pMOG4= new BackgroundSubtractorMOG();
    //Start time_catch
    time_catch timer;

    while(true)
    {
        cout<<"\n Processing \n"; //you grab frames from all the cameras here
        //this_thread::sleep_for (std::chrono::seconds(1));
        if(!vscap.read(stImg))
        {
            std::cout << "No frame from static camera" << std::endl;
            cv::waitKey();
        }

        if(!vcap1.read(image1)||!vcap2.read(image2)||!vcap3.read(image3)||!vcap4.read(image4))
        {
            std::cout << "\n###No frame in PTZ camera 1 or 2 or 3 or 4" << std::endl;
            cv::waitKey();
            return -1;
        }

        //do processing here
        processedImg = stImg;
        resize(stImg,processedImg,Size(640,480),1,1,CV_INTER_LINEAR);

        //cv::blur(stImg, processedImg, cv::Size(30,30));
        //cv::rectangle(processedImg,cv::Point(10+i,10+i),cv::Point(20+i,20+i),cv::Scalar(255,255,255),2,0,0);

        //compute the optimal actions of the PTZ cameras

        START = true;

        pMOG1->operator()(processedImg, static_fgMaskMOG);
        pMOG2->operator()(image1, ptz1_fgMaskMOG);
        pMOG3->operator()(image2, ptz2_fgMaskMOG);
        pMOG4->operator()(image3, ptz3_fgMaskMOG);

//        cv::Mat static_mask = static_fgMaskMOG>0;
//        cv::Mat static_removed_background;
//        cv::Mat ptz1_mask = ptz1_fgMaskMOG>0;
//        cv::Mat ptz1_removed_background;
//        cv::Mat ptz2_mask = ptz2_fgMaskMOG>0;
//        cv::Mat ptz2_removed_background;
//        cv::Mat ptz3_mask = ptz3_fgMaskMOG>0;
//        cv::Mat ptz3_removed_background;

//        processedImg.copyTo(static_removed_background,static_mask);
//        image1.copyTo(ptz1_removed_background,ptz1_mask);
//        image2.copyTo(ptz2_removed_background,ptz2_mask);
//        image3.copyTo(ptz3_removed_background,ptz3_mask);


        morphological morp;
        morp.set_dilation_size(4);
        morp.set_erosion_size(2);
        cv::Mat static_morp,ptz1_morp,ptz2_morp,ptz3_morp;

        morp.Erosion(static_fgMaskMOG,&static_morp);
        morp.Erosion(ptz1_fgMaskMOG,&ptz1_morp);
        morp.Erosion(ptz2_fgMaskMOG,&ptz2_morp);
        morp.Erosion(ptz3_fgMaskMOG,&ptz3_morp);
        morp.Dilation(static_morp,&static_morp);
        morp.Dilation(ptz1_morp,&ptz1_morp);
        morp.Dilation(ptz2_morp,&ptz2_morp);
        morp.Dilation(ptz3_morp,&ptz3_morp);
        morp.Erosion(static_morp,&static_morp);
        morp.Erosion(ptz1_morp,&ptz1_morp);
        morp.Erosion(ptz2_morp,&ptz2_morp);
        morp.Erosion(ptz3_morp,&ptz3_morp);


        show_n_save1 = static_morp;
        show_n_save2 = ptz1_morp;
        show_n_save3 = ptz2_morp;
        show_n_save4 = ptz3_morp;


        cv::namedWindow("Static camera",CV_WINDOW_AUTOSIZE);
        cv::moveWindow("Static camera",300,350);
        cv::imshow("Static camera", show_n_save1);
        if(cv::waitKey(5) >= 0)
            break;

        cv::namedWindow("PTZ 1",CV_WINDOW_OPENGL);
        cv::moveWindow("PTZ 1",0,0);
        cv::imshow("PTZ 1", show_n_save2);
        if(cv::waitKey(1) >= 0)
            break;
        cv::namedWindow("PTZ 2",CV_WINDOW_OPENGL);
        cv::moveWindow("PTZ 2",465,0);
        cv::imshow("PTZ 2", show_n_save3);
        if(cv::waitKey(1) >= 0)
            break;
        cv::namedWindow("PTZ 3",CV_WINDOW_OPENGL);
        cv::moveWindow("PTZ 3",465+400,0);
        cv::imshow("PTZ 3", show_n_save4);
        if(cv::waitKey(1) >= 0)
            break;

        if (timer.diff_time(5))
        {
            time_t rawtime;
            struct tm * timeinfo;
            char buffer[80];

            time (&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer,80,"%d-%m-%Y %I:%M:%S",timeinfo);
            std::string str(buffer);

            string path        =   "/media/polamin/ad73bd2e-e1ca-4fd1-a5b3-08b598c6e14b/Prabhu/Save/";

            string cam_static_save     =   path + "static/" + buffer + ".jpeg";
            string cam_ptz1_save       =   path + "ptz1/" + buffer + ".jpeg";
            string cam_ptz2_save       =   path + "ptz2/" + buffer + ".jpeg";
            string cam_ptz3_save       =   path + "ptz3/" + buffer + ".jpeg";

//            imwrite(cam_static_save,show_n_save1);
            imwrite(cam_ptz1_save,show_n_save2);
            imwrite(cam_ptz2_save,show_n_save3);
            imwrite(cam_ptz3_save,show_n_save4);

        }


    }

    cout<<"\nProcessing Thread!!!\n";

    return -1;
}


int main (int argc, char* argv[])
{

    //       QApplication a(argc, argv);
    //        MainWindow w;
    //       w.show();

    //Load calibration parameters
    //calib.load_calib_data("static_calib_cp.dat");



    //PTZ Camera initialization
    cameras.clear();
    for(int i=0;i<3;i++)
    {
        Camera camera(i, xLen, yLen, 3, "p1", "p2", "p3");
        cameras.push_back(camera);
    }
    cout<<"\n### PTZ Cameras are initialized...\n";
    cout<<"\n### Loading cameras views...\n";


    //std::thread th1(displaythread);
    std::thread th2(processing);
    //th1.join();
    th2.join();
    std::cout << "Main function ended \n";
    return 0;
    //          return a.exec();
}


