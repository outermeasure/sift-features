#include <fstream>
#include <string>
#include <stdio.h>

#undef FLANN_DEPRECATED

#include "opencv/cv.h"
#include "opencv/highgui.h"

#include "DifferenceOfGaussians.h"
#include "MaximaAndMinima.h"
#include "Orientations.h"
#include "ScaleSpace.h"
#include "Timer.h"
#include "Utils.h"

using namespace std;

int main( int argc, char* argv[]){
	Timer timer;

	IplImage* sourceImage;

	string imageSourcePath;
	string outputPath;

	if( argc >= 2 ){
		imageSourcePath = argv[1];
	} else {
		imageSourcePath = "images/melon_cat.png";
	}

	if( argc == 3 ){
		outputPath = string(argv[2]) + "/";
	} else {
		outputPath = "temp/";
	}

	sourceImage = cvLoadImage( imageSourcePath.c_str() );

	LogMessage( string("Building SIFT for ") + imageSourcePath + "." );

	// Part1: Construct a Scale Space
	ScaleSpace<4, 5> scaleSpace(sourceImage);
	scaleSpace.Build();
	scaleSpace.DumpImages( outputPath + "SS_image_%d_%d.png" );
	scaleSpace.DumpSigmas( outputPath + "sigmas.txt" );

	// Part2: Difference of Gaussians
	DifferenceOfGaussians<decltype(scaleSpace)> dog(scaleSpace);
	dog.Build();
	dog.DumpImages( outputPath + "DOG_image_%d_%d.png" );

	// Part3: Locate Maxima and Minima
	MaximaAndMinima<decltype(dog)> mam(dog);
	mam.Build();
	mam.DumpImages( outputPath + "MaM_image_%d_%d.png" );

	// Part4: Assign Orientations
	Orientations<decltype(scaleSpace), decltype(mam)> orientations(scaleSpace, mam);
	orientations.Build();

	// Part5: Extract Keypoint Descriptors

	int milis = timer.Count( Timer::MILISECONDS );
	LogTimedTask( "SIFT done.", milis );

	cvReleaseImage( &sourceImage );
	return 0;
}