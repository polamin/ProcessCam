#include "morphological.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace  cv;
morphological::morphological()
{
}

void morphological::set_dilation_size(int size)
{
    dilation_size = size;
}

void morphological::set_erosion_size(int size)
{
    erosion_size = size;
}


//All operation use MORPH_ELLIPSE erosion model
void morphological::Dilation(Mat src,Mat *dilation_dst)
{
      Mat element = getStructuringElement( MORPH_ELLIPSE,
                                           Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                           Point( dilation_size, dilation_size ) );
      /// Apply the dilation operation
      dilate(src,*dilation_dst, element );
}


void morphological::Erosion(Mat src,Mat *erosion_dst)
{
  Mat element = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  /// Apply the erosion operation
  erode( src, *erosion_dst, element );
}
