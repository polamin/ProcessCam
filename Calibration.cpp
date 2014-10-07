/*
 * Calibration.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: prabhu
 */

#include "Calibration.h"
using namespace std;

Calibration::Calibration() {
    // TODO Auto-generated constructor stub

}

Calibration::~Calibration() {
    // TODO Auto-generated destructor stub
}

void Calibration::load_calib_data(string filenmae)
{
    ifstream ifile(filenmae);
    if(ifile.is_open())
    {
        ifile>>cp.Ncx;
        ifile>>cp.Nfx;
        ifile>>cp.dx;
        ifile>>cp.dy;
        ifile>>cp.dpx;
        ifile>>cp.dpy;
        ifile>>cp.Cx;
        ifile>>cp.Cy;
        ifile>>cp.sx;

        ifile>>cc.f;
        ifile>>cc.kappa1;
        ifile>>cc.Tx;
        ifile>>cc.Ty;
        ifile>>cc.Tz;
        ifile>>cc.Rx;
        ifile>>cc.Ry;
        ifile>>cc.Rz;

        sa = sin(cc.Rx); ca = cos(cc.Rx);
        sb = sin(cc.Ry); cb = cos(cc.Ry);
        sg = sin(cc.Rz); cg = cos(cc.Rz);

        cc.r1 = cb * cg;
        cc.r2 = cg * sa * sb - ca * sg;
        cc.r3 = sa * sg + ca * cg * sb;
        cc.r4 = cb * sg;
        cc.r5 = sa * sb * sg + ca * cg;
        cc.r6 = ca * sb * sg - cg * sa;
        cc.r7 = -sb;
        cc.r8 = cb * sa;
        cc.r9 = ca * cb;

        ifile>>cc.p1;
        ifile>>cc.p2;

        cout<<"\n### Calibration file is loaded successfully...\n\n";
    }
    else
    {
        cout<<"\n### Error loading calibration parameter\n";
    }
}

void Calibration::image_coord_to_world_coord(double Xfd, double Yfd, double zw, double &xw, double &yw)
{
    double Xd, Yd, Xu, Yu, common_denominator;

    /* convert from image to distorted sensor coordinates */
    Xd = cp.dpx * (Xfd - cp.Cx) / cp.sx;
    Yd = cp.dpy * (Yfd - cp.Cy);

    /* convert from distorted sensor to undistorted sensor plane coordinates */
    Xu = 0; Yu = 0;
    distorted_to_undistorted_sensor_coord(Xd, Yd, Xu, Yu);

    /* calculate the corresponding xw and yw world coordinates	 */
    /* (these equations were derived by simply inverting	 */
    /* the perspective projection equations using Macsyma)	 */
    common_denominator = ((cc.r1 * cc.r8 - cc.r2 * cc.r7) * Yu +
              (cc.r5 * cc.r7 - cc.r4 * cc.r8) * Xu -
              cc.f * cc.r1 * cc.r5 + cc.f * cc.r2 * cc.r4);

    xw = (((cc.r2 * cc.r9 - cc.r3 * cc.r8) * Yu +
        (cc.r6 * cc.r8 - cc.r5 * cc.r9) * Xu -
        cc.f * cc.r2 * cc.r6 + cc.f * cc.r3 * cc.r5) * zw +
       (cc.r2 * cc.Tz - cc.r8 * cc.Tx) * Yu +
       (cc.r8 * cc.Ty - cc.r5 * cc.Tz) * Xu -
       cc.f * cc.r2 * cc.Ty + cc.f * cc.r5 * cc.Tx) / common_denominator;

    yw = -(((cc.r1 * cc.r9 - cc.r3 * cc.r7) * Yu +
         (cc.r6 * cc.r7 - cc.r4 * cc.r9) * Xu -
         cc.f * cc.r1 * cc.r6 + cc.f * cc.r3 * cc.r4) * zw +
        (cc.r1 * cc.Tz - cc.r7 * cc.Tx) * Yu +
        (cc.r7 * cc.Ty - cc.r4 * cc.Tz) * Xu -
        cc.f * cc.r1 * cc.Ty + cc.f * cc.r4 * cc.Tx) / common_denominator;

}

void Calibration::distorted_to_undistorted_image_coord(double Xfd, double Yfd, double &Xfu, double &Yfu)
{
    double Xd, Yd, Xu, Yu;

    /* convert from image to sensor coordinates */
    Xd = cp.dpx * (Xfd - cp.Cx) / cp.sx;
    Yd = cp.dpy * (Yfd - cp.Cy);

    /* convert from distorted sensor to undistorted sensor plane coordinates */
    Xu = 0; Yu = 0;
    distorted_to_undistorted_sensor_coord(Xd, Yd, Xu, Yu);

    /* convert from sensor to image coordinates */
    Xfu = Xu * cp.sx / cp.dpx + cp.Cx;
    Yfu = Yu / cp.dpy + cp.Cy;
}

void Calibration::distorted_to_undistorted_sensor_coord(double Xd, double Yd, double &Xu, double &Yu)
{
    double distortion_factor;
    /* convert from distorted to undistorted sensor plane coordinates */
    distortion_factor = 1 + cc.kappa1 * ((Xd * Xd) + (Yd * Yd));
    Xu = Xd * distortion_factor;
    Yu = Yd * distortion_factor;
}
