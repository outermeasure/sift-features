#pragma once

#include "Keypoint.h"
#include "Timer.h"

// Templated on Scale Space
template <typename T, typename V>
class Orientations{
public:
	std::vector<Keypoint> keypoints;
	const T scaleSpace;
	const V& extrema;
	Timer timer;

	enum{
		octaves = (int)T::octaves,
		scales = (int)T::scales-1
	};

	Orientations( T& sspace, V& extrema ) : scaleSpace( sspace ), extrema( extrema ) {};

	// Returns the size of the kernal for the Gaussian blur given the sigma and cutoff value.
	unsigned int GetKernelSize(double sigma, double cut_off = 0.001)
	{
		static int MAX_KERNEL_SIZE = 20;
		int i;
		for (i=0; i<MAX_KERNEL_SIZE; ++i)
		{
			if (exp(-((double)(i*i))/(2.0*sigma*sigma))<cut_off)
			{
				break;
			}
		}
		unsigned int size = 2*i-1;
		return size;
	}

	void Build(){
		timer.Reset();
		LogMessage("Calculating Orientations");

		int NUM_BINS = 8;
		double M_PI = 3.1415926;

		IplImage* magnitudes[octaves][scales];
		IplImage* orientations[octaves][scales];

		// Calculate magnitude and orientation for every pixel on every scale, every octave, ever. 
		// Yosh, ikuzo!
		for( int octave = 0; octave < octaves; ++octave ){
			for( int scale = 1; scale < scales+1; ++scale ){

				const IplImage* img = scaleSpace.GetImage(octave, scale-1);

				magnitudes[octave][scale-1] = cvCreateImage(cvGetSize(img), 32, 1);
				orientations[octave][scale-1] = cvCreateImage(cvGetSize(img), 32, 1);

				cvZero(magnitudes[octave][scale-1]);
				cvZero(orientations[octave][scale-1]);

				// Iterate over the gaussian image with the current octave and interval
				for( int xi=1; xi<img->width-1; xi++ )
				{
					for( int yi=1; yi<img->height-1; yi++ )
					{
						// Calculate gradient
						double dx = cvGetReal2D(img, yi, xi+1) - cvGetReal2D(img, yi, xi-1);
						double dy = cvGetReal2D(img, yi+1, xi) - cvGetReal2D(img, yi-1, xi);

						// Store magnitude
						cvSetReal2D(magnitudes[octave][scale-1], yi, xi, sqrt(dx*dx + dy*dy));

						// Store orientation as radians
						double ori=atan(dy/dx);					
						cvSet2D(orientations[octave][scale-1], yi, xi, cvScalar(ori));
					}
				}
			}
		}

		// The histogram
		double* hist_orient = new double[NUM_BINS];

		// Go through all octaves
		for( int octave = 0; octave < octaves; ++octave )
		{
			// Store current scale, width and height
			int scale = (unsigned int)pow(2.0, (double)octave);
			int width = scaleSpace.GetImage(octave, 0)->width;
			int height= scaleSpace.GetImage(octave, 0)->height;

			// Go through all intervals in the current scale
			for( int scale = 1; scale < scales+1; ++scale )
			{
				double absSigma = scaleSpace.GetSigma( octave, scale );
				// This is used for magnitudes
				IplImage* imgWeight = cvCreateImage(cvSize(width, height), 32, 1);
				cvSmooth(magnitudes[octave][scale-1], imgWeight, CV_GAUSSIAN, 0, 0, 1.5*absSigma);

				// Get the kernel size for the Guassian blur
				int hfsz = GetKernelSize(1.5*absSigma)/2;

				// Temporarily used to generate a mask of region used to calculate 
				// the orientations
				IplImage* imgMask = cvCreateImage(cvSize(width, height), 8, 1);
				cvZero(imgMask);

				// Iterate through all points at this octave and interval
				for( int xi = 0; xi < width; ++xi )
				{
					for( int yi = 0; yi <height; ++yi)
					{
						// skip if not a keypoint
						if( cvGetReal2D( extrema.GetImage(octave,scale-1), yi, xi)==0 ){
							continue;
						}

						// Reset the histogram
						for( int k=0; k<NUM_BINS; hist_orient[k++] = 0.0);

						// Go through all pixels in the window around the extrema
						for(int kk = -hfsz; kk <= hfsz; ++kk )
						{
							for(int tt = -hfsz; tt <= hfsz; ++tt )
							{
								// Ensure we're within the image
								if( xi + kk <0 || xi + kk >= width || yi + tt < 0 || yi + tt >= height )
									continue;

								double sampleOrient = cvGetReal2D(orientations[octave][scale-1], yi+tt, xi+kk);

								if( sampleOrient <= -M_PI || sampleOrient > M_PI )
									printf( "Bad Orientation: %f\n", sampleOrient );

								sampleOrient += M_PI;

								// Convert to degrees
								int sampleOrientDegrees = (int)(sampleOrient * (180.0 / M_PI));
								hist_orient[(int)sampleOrientDegrees / (360/NUM_BINS)] += cvGetReal2D(imgWeight, yi+tt, xi+kk);
								cvSetReal2D(imgMask, yi+tt, xi+kk, 255);
							}
						}
					}
				}

			}
		}

		// cleanup
		for( int octave = 0; octave < octaves; ++octave ){
			for( int scale = 1; scale < scales+1; ++scale ){
				cvReleaseImage(& magnitudes[octave][scale-1]);
				cvReleaseImage(& orientations[octave][scale-1]);
			}
		}
		LogTimedTask("- Magnitudes & Orientations", timer.Count());
		timer.Reset();
	}
};