#include "opencv/cv.h"
#include "opencv/highgui.h"

int main(){

	IplImage* img = cvLoadImage( "IndianHeadTestPattern16x9.png" );
	cvNamedWindow( "Display" );
	cvShowImage( "Display", img );
	cvWaitKey(0);
	cvReleaseImage( &img );

	return 0;
}