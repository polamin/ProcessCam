#ifndef MORPHOLOGICAL_H
#define MORPHOLOGICAL_H
#include "opencv2/opencv.hpp"
class morphological
{
public:
    morphological();
    void Dilation(cv::Mat,cv::Mat*);
    void Erosion(cv::Mat,cv::Mat*);
    void set_dilation_size(int);
    void set_erosion_size(int);

private:
    int erosion_size = 0;
    int dilation_size = 0;
    int const max_elem = 2;
    int const max_kernel_size = 21;

};

#endif // MORPHOLOGICAL_H
