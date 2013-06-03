#pragma once

#include <stdio.h>
#include <string>

#include "ScaleSpace.h"
#include "Utils.h"

template<typename S>
class DifferenceOfGaussians{
public:
	DifferenceOfGaussians( const S& scaleSpace ) : sourceScaleSpace(scaleSpace) {}

	enum class Sizes {
		size_x = scalespace_traits<S>::octaves,
		size_y = scalespace_traits<S>::scales - 1
	};

	void Build(){
		LogMessage("Building Difference of Gaussians");
		timer.Reset();

		for( int x = 0; x < (int)Sizes::size_x; ++x ){
			for( int y = 0; y < (int)Sizes::size_y; ++y ){
				images[x][y] = cvCreateImage(cvGetSize(sourceScaleSpace.GetImage(x, 0)), 32, 1);
				cvSub(sourceScaleSpace.GetImage(x, y), sourceScaleSpace.GetImage(x, y+1), images[x][y]);
			}
		}

		LogTimedTask( "Done building", timer.Count() );

	}

	IplImage* GetImage( int x, int y ) const{
		return images[x][y];
	}

	void DumpImages( const std::string& filenameformat ){
		timer.Reset();
		SaveImageBatch( filenameformat.c_str(), (int)Sizes::size_x, (int)Sizes::size_y, images );
		LogTimedTask( "Dumped images", timer.Count() );
	}

private:
	Timer timer;
	IplImage* images[Sizes::size_x][Sizes::size_y];
	S sourceScaleSpace;
};