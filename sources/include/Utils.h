#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>

#include "opencv/cv.h"
#include "opencv/highgui.h"

class Priority{
public:
	enum{
		INFO,
		WARNING,
		ERR
	};

private:
	Priority();
};

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

// Batch save a matrix of images
template <typename T>
void SaveImageBatch( const char* filenameformat, int size_x, int  size_y, T* imageBatch ){
	char buffer[64];
	for( int x = 0; x < size_x; ++x ){
		for( int y = 0; y < size_y; ++y ){
			sprintf( buffer, filenameformat, x, y );
			SaveFloatingPointImage( buffer, imageBatch[x][y] );
		}
	}
}

std::string GetTimestamp(){
	using namespace std;
	time_t time = std::time(nullptr);
	stringstream stream;
	stream << setfill('0') << setw(2) << std::localtime(&time)->tm_hour 
		<< ":" << setfill('0') << setw(2) << std::localtime(&time)->tm_min
		<< ":" << setfill('0') << setw(2) << std::localtime(&time)->tm_sec;
	return stream.str();
}

void LogMessage( int priority, const std::string& message ){
	using namespace std;
	cout << "[" << GetTimestamp() << "] " << message << endl;
}

void LogMessage( const std::string& message ){
	LogMessage( Priority::INFO, message );
}

void LogTimedTask( const std::string& msg, int milis ){
	using namespace std;
	stringstream sstream;

	int align = 36 - msg.length();
	sstream << msg;
	for( int i = 0; i < align; ++i ){
		sstream << " ";
	}
	if( milis < 1000 ){		
		sstream << milis << "ms";
	} else {
		sstream << (float)milis / 1000.0 << "s";
	}
	LogMessage( sstream.str() );
}

template<typename T>
inline T clamp(T value, T low, T high)
{
	if (value < low ) return low;
	else if (value > high) return high;
	else return value;
}