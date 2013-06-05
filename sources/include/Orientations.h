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
		scales = (int)T::scales-2
	};

	Orientations( T& sspace, V& extrema ) : scaleSpace( sspace ), extrema( extrema ) {};

	void DumpImages( const std::string& filenameTemplate ){
		timer.Reset();
		//SaveImageBatch( filenameTemplate.c_str(), octaves, scales, keypoints );
		LogTimedTask( "Dumped images", timer.Count() );
	}

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

	void Build()
	{
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
						double ori = atan2(dy, dx);

						//assert(dx != 0.0);
						cvSetReal2D(orientations[octave][scale-1], yi, xi, ori);
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
			int scalepow = (unsigned int)pow(2.0, (double)octave);
			int width = extrema.GetImage(octave, 0)->width;
			int height = extrema.GetImage(octave, 0)->height;

			int originalw = width, originalh = height;

			// Go through all intervals in the current scale
			for( int scale = 1; scale < scales; ++scale )
			{
				
				double absSigma = scaleSpace.GetSigma( octave, scale );
				// This is used for magnitudes
				IplImage* imgWeight = cvCreateImage(cvSize(originalw, originalh), 32, 1);
				cvSmooth(magnitudes[octave][scale-1], imgWeight, CV_GAUSSIAN, 0, 0, 1.5*absSigma);

				// Get the kernel size for the Guassian blur
				int wsize = GetKernelSize(1.5*absSigma)/2;

				// Temporarily used to generate a mask of region used to calculate 
				// the orientations
				IplImage* imgMask = cvCreateImage(cvSize(originalw, originalh), 8, 1);
				cvZero(imgMask);


				// Iterate through all points at this octave and interval
				for( int xi = 1; xi < width - 1; ++xi )
				{
					for( int yi = 1; yi < height - 1; ++yi)
					{
						// skip if not a keypoint

						double keypointv = cvGetReal2D( extrema.GetImage(octave,scale-1), yi, xi);
						if(  keypointv == 0 )
						{
							continue;
						}

						// Reset the histogram
						for( int k=0; k<NUM_BINS; hist_orient[k++] = 0.0);

						// Go through all pixels in the window around the extrema
						for(int kk = -wsize; kk <= wsize; ++kk )
						{
							for(int tt = -wsize; tt <= wsize; ++tt )
							{
								// Ensure we're within the image
								if( xi + kk <= 0 || xi + kk >= width - 1 || yi + tt <= 0 || yi + tt >= height - 1 )
									continue;

								double sampleOrient = cvGetReal2D(orientations[octave][scale-1], yi + tt, xi + kk);

								if( sampleOrient <= -M_PI || sampleOrient > M_PI )
									printf( "Orientation BAD!%f\n", sampleOrient );

								sampleOrient += M_PI;

								// Convert to degrees
								int sampleOrientDegrees = (int)(sampleOrient * (180.0 / M_PI));
								hist_orient[(int)sampleOrientDegrees / (360/NUM_BINS)] += cvGetReal2D(imgWeight, yi + tt, xi + kk);
								cvSetReal2D(imgMask, yi+tt, xi+kk, 255);
							}
						}						

						double max_peak = hist_orient[0];
						unsigned int max_peak_index = 0;
						for(int k = 1; k < NUM_BINS; k++)
						{
							if(hist_orient[k] > max_peak)
							{
								max_peak = hist_orient[k];
								max_peak_index = k;
							}
						}

						// List of magnitudes and orientations at the current extrema
						vector<double> orien;
						vector<double> mag;
						for(int k = 0; k < NUM_BINS; k++)
						{
							// Do we have a good peak?
							if(hist_orient[ k ] > 0.8 * max_peak)
							{

								double x1 = k - 1;
								double y1;
								double x2 = k;
								double y2 = hist_orient[k];
								double x3 = k + 1;
								double y3;

								if( k == 0 )
								{
									y1 = hist_orient[NUM_BINS-1];
									y3 = hist_orient[1];
								}
								else if(k == NUM_BINS - 1)
								{
									y1 = hist_orient[NUM_BINS-1];
									y3 = hist_orient[0];
								}
								else
								{
									y1 = hist_orient[k-1];
									y3 = hist_orient[k+1];
								}

								double b[3];
								CvMat *X = cvCreateMat(3, 3, CV_32FC1);
								CvMat *matInv = cvCreateMat(3, 3, CV_32FC1);

								cvSetReal2D(X, 0, 0, x1*x1);
								cvSetReal2D(X, 1, 0, x1);
								cvSetReal2D(X, 2, 0, 1);

								cvSetReal2D(X, 0, 1, x2*x2);
								cvSetReal2D(X, 1, 1, x2);
								cvSetReal2D(X, 2, 1, 1);

								cvSetReal2D(X, 0, 2, x3*x3);
								cvSetReal2D(X, 1, 2, x3);
								cvSetReal2D(X, 2, 2, 1);

								// Invert the matrix
								cvInv(X, matInv);

								b[0] = cvGetReal2D(matInv, 0, 0)*y1 + cvGetReal2D(matInv, 1, 0)*y2 + cvGetReal2D(matInv, 2, 0)*y3;
								b[1] = cvGetReal2D(matInv, 0, 1)*y1 + cvGetReal2D(matInv, 1, 1)*y2 + cvGetReal2D(matInv, 2, 1)*y3;
								b[2] = cvGetReal2D(matInv, 0, 2)*y1 + cvGetReal2D(matInv, 1, 2)*y2 + cvGetReal2D(matInv, 2, 2)*y3;

								double x0 = -b[1]/(2*b[0]);

								// Anomalous situation
								if(fabs(x0) > 2*NUM_BINS || b[0] == 0)
									x0=x2;

								while(x0 < 0)
									x0 += NUM_BINS;
								while(x0>= NUM_BINS)
									x0-= NUM_BINS;

								// Normalize it
								double x0_n = x0*(2*M_PI/NUM_BINS);

								assert(x0_n>=0 && x0_n<2*M_PI);
								x0_n -= M_PI;
								assert(x0_n>=-M_PI && x0_n<M_PI);

								orien.push_back(x0_n);
								mag.push_back(hist_orient[k]);
							}
						}					
						keypoints.push_back(
							Keypoint(xi * scalepow / 2, yi * scalepow / 2, mag, orien, scale * scales + octave - 1)
							);
					}
				}
				cvReleaseImage(&imgMask);
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