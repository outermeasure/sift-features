#pragma once

#include <stdio.h>
#include <string>

#include "ScaleSpace.h"
#include "Utils.h"

template<typename S>
class DifferenceOfGaussians{
public:
	DifferenceOfGaussians( const S& scaleSpace ) : sourceScaleSpace(scaleSpace) {}

	enum {
		size_x = scalespace_traits<S>::octaves,
		size_y = scalespace_traits<S>::scales - 1
	};

	void Build(){
		for( int x = 0; x < size_x; ++x ){
			for( int y = 0; y < size_y; ++y ){
				images[x][y] = cvCreateImage(cvGetSize(sourceScaleSpace.GetImage(x, 0)), 32, 1);
				cvSub(sourceScaleSpace.GetImage(x, y), sourceScaleSpace.GetImage(x, y+1), images[x][y]);
			}
		}
	}

	void DumpImages( const std::string& filenameformat ){
		char buffer[64];
		for( int x = 0; x < size_x; ++x ){
			for( int y = 0; y < size_y; ++y ){
				sprintf( buffer, filenameformat.c_str(), x, y );
				SaveFloatingPointImage( buffer, images[x][y] );
			}
		}
	}

private:
	IplImage* images[size_x][size_y];
	S sourceScaleSpace;
};