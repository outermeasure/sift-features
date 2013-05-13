#pragma once

template< typename T >
class MaximaAndMinima{
public:
	enum{
		size_x = T::size_x,
		size_y = T::size_y - 2
	};

	MaximaAndMinima( const T& source ): source(source) {
	}

	void Build(){
		timer.Reset();

		for( int octave = 0; octave < size_x; ++octave ){
			for( int scale = 1; scale <= size_y; ++ scale ){

				// for every pixel
				CvSize imageSize = cvGetSize(source.GetImage(octave, scale));
				images[octave][scale-1] = cvCreateImage( imageSize, 32, 1 );
				for( int x = 1; x < imageSize.height-1; ++x ){
					for( int y = 1; y < imageSize.width-1; ++y ){
						double elem, _max, _min;
						elem = _max = _min = cvGetReal2D( source.GetImage( octave, scale ), x, y );

						if( elem < 0.01 ) continue;

						for( int dscale = -1; dscale <= 1; ++dscale ){
							for( int dx = -1; dx <= 1; ++dx ){
								for( int dy = -1; dy <= 1; ++dy ){
									double val = cvGetReal2D( source.GetImage( octave, scale + dscale ), x + dx, y + dy );
									if( val > _max ) _max = val;
									if( val < _min ) _min = val;
								}
							}
						}

						if( elem == _max || elem == _min ){
							cvSetReal2D( images[octave][scale-1], x, y, 1.0f );
						}
					}
				}
			}
		}

		LogTimedTask( "Built Maxima and Minima", timer.Count() );

	}

	void DumpImages( const std::string& filenameTemplate ){
		timer.Reset();
		SaveImageBatch( filenameTemplate.c_str(), size_x, size_y, images );
		LogTimedTask( "Dumped images", timer.Count() );
	}

private:
	Timer timer;
	IplImage* images[size_x][size_y];
	const T& source;
};