#pragma once

#include <stdio.h>

#include "Timer.h"
#include "Utils.h"

template< int O, int S >
class ScaleSpace{
public:

	static enum{
		octaves = O,
		scales = S
	};

	ScaleSpace( IplImage* source ){
		this->source = source;
	}

	~ScaleSpace(){
	}

	int GetOctaveCount(){ return O; }
	int GetScaleCount(){ return S; }

	void Build( double sigmaConstant = 1.41421356237 ){
		timer.Reset();
		LogMessage( "Building Scale Space" );
		IplImage* imgGray = ConstructGrayscaleCopy( source );
		ConstructOctaves( imgGray );
		ConstructScales( sigmaConstant );

		LogTimedTask( "Done Building", timer.Count() );

		cvReleaseImage( &imgGray );
	}

	IplImage* ConstructGrayscaleCopy( IplImage* source ){
		IplImage* imgGray = cvCreateImage( cvGetSize(source), IPL_DEPTH_32F, 1 );
		IplImage* imgTemp = cvCreateImage( cvGetSize(source), 8, 1 );

		// Convert the source to grayscale
		if( source->nChannels == 3 ){
			cvCvtColor( source, imgTemp, CV_BGR2GRAY );
		} else {
			cvCopy( source, imgTemp );
		}

		// convert the grayscale to float
		for( int x = 0; x < source->width; ++x ){
			for( int y = 0; y < source->height; ++y ){
				cvSetReal2D( imgGray, y, x, cvGetReal2D(imgTemp, y, x) / 255.0 );
			}
		}

		cvReleaseImage( &imgTemp );
		return imgGray;
	}

	void ConstructOctaves( const IplImage* source ) 
	{
		// scale the first octave UP
		images[0][0] = cvCreateImage( cvSize( source->width * 2, source->height * 2 ), IPL_DEPTH_32F, 1 );
		cvPyrUp( source, images[0][0] );

		// scale the rest of the octaves DOWN
		float imageScale = 1.0f;
		for( int octave = 1; octave < octaves; ++octave ){
			images[octave][0] = cvCreateImage( cvSize( (int)(source->width * imageScale), (int)(source->height * imageScale) ), IPL_DEPTH_32F, 1 );
			cvPyrDown( images[octave-1][0], images[octave][0] );
			imageScale /= 2.0;
		}
	}

	void ConstructScales( double sigmaConstant ){
		for( int octave = 0; octave < octaves; ++octave ){
			double sigma = 1.0f; //pow(2.0, (double)octave);
			for( int scale = 1; scale < scales; ++scale ){
				sigmaValues[octave][scale] = sigma;
				images[octave][scale] = cvCreateImage( cvGetSize(images[octave][0]), IPL_DEPTH_32F, 1 );
				cvSmooth(images[octave][scale-1], images[octave][scale], CV_GAUSSIAN, 0, 0, sigma);
				sigma *= sigmaConstant;
			}
		}
	}

	void DumpImages( const std::string& filenameTemplate ){
		timer.Reset();
		SaveImageBatch( filenameTemplate.c_str(), octaves, scales, images );
		LogTimedTask( "Dumped images", timer.Count() );
	}

	void DumpSigmas( const std::string& filename ){
		ofstream fout(filename);
		for( int octave = 0; octave < O; ++octave ){
			for( int scale = 1; scale < S; ++scale ){
				fout << setw(12) << sigmaValues[octave][scale];
			}
			fout << endl;
		}
	}

	const IplImage* GetImage( int octave, int scale ) const{
		if( octave < O && scale < S ){
			return images[octave][scale];
		}
		return nullptr;
	}

	double GetSigma( int octave, int scale ) const {
		return sigmaValues[octave][scale];
	}

private:
	Timer timer;
	IplImage* images[octaves][scales];
	IplImage* source;
	double sigmaValues[octaves][scales];
};

template <typename T>
struct scalespace_traits{
	enum{
		octaves = T::octaves,
		scales = T::scales
	};
};