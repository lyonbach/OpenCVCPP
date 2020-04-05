#include <iostream>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

using std::cout;

const char* image1 = "/home/lyonbach/Repositories/lb_opencv/OpenCVBuiltinGUITrackbar/samples/windowsLogo.png";
const char* image2 = "/home/lyonbach/Repositories/lb_opencv/OpenCVBuiltinGUITrackbar/samples/ubuntuLogo.png";
const int alpha_slider_max = 100;
int alpha_slider;
double alpha;
double beta;

Mat src1;
Mat src2;
Mat dst;

static void onTrackBarChangedValue( int, void* )
{
   alpha = (double) alpha_slider / alpha_slider_max ;
   beta = ( 1.0 - alpha );
   addWeighted( src1, alpha, src2, beta, 0.0, dst);
   imshow( "Linear Blend", dst );
}

int main( void )
{
   src1 = imread( image1 );
   src2 = imread( image2 );

   if( src1.empty() ) { cout << "Error loading src1 \n"; return -1; }
   if( src2.empty() ) { cout << "Error loading src2 \n"; return -1; }
   
   // Create window.
//    namedWindow("Linear Blend", WINDOW_AUTOSIZE); // Create Window
   namedWindow("Linear Blend", WINDOW_GUI_EXPANDED); // Create Window
   resizeWindow("Linear Blend", 400, 400);

   
   char TrackbarName[50];
   alpha_slider = 0;
   sprintf( TrackbarName, "Alpha x %d", alpha_slider_max );

   createTrackbar( TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, onTrackBarChangedValue );
   onTrackBarChangedValue( alpha_slider, 0 );
   waitKey(0);
   return 0;
}