#include <fstream>
#include <string>
#include <stdio.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "DifferenceOfGaussians.h"
#include "ScaleSpace.h"
#include "Utils.h"

using namespace std;

int main(){
	IplImage* src = cvLoadImage( "images/melon_cat.png" );

	// Part1: Construct a Scale Space
	ScaleSpace<4, 5> scaleSpace(src);
	scaleSpace.Build();
	scaleSpace.DumpImages( "temp/SS_image_%d_%d.png" );
	scaleSpace.DumpSigmas( "temp/sigmas.txt" );

	// Part2: Difference of Gaussians
	DifferenceOfGaussians<decltype(scaleSpace)> dog(scaleSpace);
	dog.Build();
	dog.DumpImages( "temp/DOG_image_%d_%d.png" );

	cvReleaseImage( &src );
	return 0;
}