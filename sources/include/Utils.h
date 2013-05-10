#pragma once

#include <stdio.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

// SaveFloatingPointImage()
// The standard HighGUI functions can save only 8bit images. This
// function converts a floating point image (with values 0..1) into
// a normal 8bit image (with values 0..255), and then saves it.
void SaveFloatingPointImage(const char *filename, IplImage* img)
{
	IplImage* dup = cvCreateImage(cvGetSize(img), 8, 1);
	cvCvtScale(img, dup, 255.0);

	cvSaveImage(filename, dup);
	cvReleaseImage(&dup);
}