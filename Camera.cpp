/*
 * Camera.cpp
 *
 *  Created on: Jul 14, 2014
 *      Author: prabhu
 */

#include "Camera.h"

Camera::Camera() {

}

Camera::~Camera() {

}

Camera::Camera(int id, int xLen, int yLen, int nactions, string state1, string state2, string state3)
{
    this->id = id; this->xLen = xLen; this->yLen = yLen;
    this->naction= nactions;

    this->state[0] = state1; this->state[1] = state1; this->state[2] = state2;

    //Load FOV parameters
    cout<<"CAMERA_"<<id<<": \n";
    FOV.clear();
    char filename[100]; sprintf(filename,"cam_%d.txt",id);
    ifstream ifile(filename);
    if(ifile.is_open())
    {
        for(int a=0; a<nactions;a++)
        {
          sparse_vector eachFov; eachFov.clear();
          for(int i=0; i<xLen*yLen; i++)
              eachFov.insert(make_pair(i,0));
          int x,y,total;
          ifile>>total;
          cout<<total<<": ";
          for(int i=0;i<total;i++)
          {
              ifile>>x; ifile>>y;
              int state = getState(x,y,xLen);
              cout<<x<<","<<y<<"("<<state<<") ";
              eachFov[state]=1;
          }
          cout<<"\n";
          FOV.push_back(eachFov);
        }
        for(int i=0;i<3;i++)
        {
            ifile>>state[i];
            cout<<i<<": "<<state[i]<<"\n";
        }
    }
}

int Camera::getState(int x, int y, int xlen)
{
    return (x + y*xlen);
}

bool Camera::ptzCommand(int i)
{
    if(i>=0)
    {
        char arg[1000];
        strcpy(arg,state[i].c_str());
        CURL *curl; CURLcode res;
        curl = curl_easy_init();
        if(curl)
        {
          /* First set the URL that is about to receive our POST. This URL can just as well be a https:// URL if that is what should receive the
             data. */
          curl_easy_setopt(curl, CURLOPT_URL, arg);
          /* Perform the request, res will get the return code */
          res = curl_easy_perform(curl);
          /* always cleanup */
          curl_easy_cleanup(curl);
          //std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return true;
    }
    else
        return false;
}
