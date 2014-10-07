/*
 * Calibration.h
 *
 *  Created on: Jul 21, 2014
 *      Author: prabhu
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;

class Calibration {
public:
    Calibration();
    virtual ~Calibration();

    struct camera_parameters
    {
        double Ncx;		/* [sel]     Number of sensor elements in camera's x direction */
        double Nfx;		/* [pix]     Number of pixels in frame grabber's x direction   */
        double dx;		/* [mm/sel]  X dimension of camera's sensor element (in mm)    */
        double dy;		/* [mm/sel]  Y dimension of camera's sensor element (in mm)    */
        double dpx;		/* [mm/pix]  Effective X dimension of pixel in frame grabber   */
        double dpy;		/* [mm/pix]  Effective Y dimension of pixel in frame grabber   */
        double Cx;		/* [pix]     Z axis intercept of camera coordinate system      */
        double Cy;		/* [pix]     Z axis intercept of camera coordinate system      */
        double sx;		/* []        Scale factor to compensate for any error in dpx   */
    };
    struct calibration_constants
    {
        double f;		/* [mm]          */
        double kappa1;		/* [1/mm^2]      */
        double p1;		/* [1/mm]        */
        double p2;		/* [1/mm]        */
        double Tx;		/* [mm]          */
        double Ty;		/* [mm]          */
        double Tz;		/* [mm]          */
        double Rx;		/* [rad]	 */
        double Ry;		/* [rad]	 */
        double Rz;		/* [rad]	 */
        double r1;		/* []            */
        double r2;		/* []            */
        double r3;		/* []            */
        double r4;		/* []            */
        double r5;		/* []            */
        double r6;		/* []            */
        double r7;		/* []            */
        double r8;		/* []            */
        double r9;		/* []            */
    };
    double sa, ca, sb, cb, sg, cg;
    camera_parameters cp;
    calibration_constants cc;

    //Methods
    void load_calib_data(string filenmae);
    void image_coord_to_world_coord(double Xfd, double Yfd, double zw, double &xw, double &yw);
    void distorted_to_undistorted_image_coord(double Xfd, double Yfd, double &Xfu, double &Yfu);
    void distorted_to_undistorted_sensor_coord(double Xd, double Yd, double &Xu, double &Yu);




};

#endif /* CALIBRATION_H_ */
