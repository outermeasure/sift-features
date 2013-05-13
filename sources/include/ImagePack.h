#pragma once

#include "opencv/cv.h"

class ImagePack{
public:

	typedef IplImage* ImagePtr;

	ImagePack( PackBuilder builder ): x(x), y(y){
	}

	~ImagePack(){
		builder.destroy( images );
	}

private:
	const int x, y;
	ImagePtr** images;
};