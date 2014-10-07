/*
 * Camera.h
 *
 *  Created on: Jul 14, 2014
 *      Author: prabhu
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>
#include <map>
#include <curl/curl.h>

using namespace std;

typedef map <int, double> sparse_vector;

class Camera {

public:
    //Members
    int id=-1;
    string state[3];
    int  naction;
    vector<sparse_vector> FOV;
    int xLen, yLen;

    //Constructors
    Camera();
    Camera(int id, int xLen, int yLen, int  naction, string state1, string state2, string state3);

    //Destructors
    virtual ~Camera();

    //Methods
    bool ptzCommand(int i);
    int getState(int x, int y, int xlen);
};

#endif /* CAMERA_H_ */
