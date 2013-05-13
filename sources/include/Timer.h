#pragma once

#include <chrono>

class Timer{
public:

	enum{
		SECONDS,
		MILISECONDS,
		MICROSECONDS,
		NANOSECONDS
	};

	void Reset(){
		using namespace std::chrono;
		startTime = high_resolution_clock::now();
	}

	int Count( int unit = Timer::MILISECONDS ){
		using namespace std::chrono;
		high_resolution_clock::time_point now = high_resolution_clock::now();

		switch ( unit )
		{
		case SECONDS:
			return (int)duration_cast<std::chrono::seconds>(now - startTime).count();
		case MILISECONDS:
			return (int)duration_cast<std::chrono::milliseconds>(now - startTime).count();
		case MICROSECONDS:
			return (int)duration_cast<std::chrono::microseconds>(now - startTime).count();
		case NANOSECONDS:
			return (int)duration_cast<std::chrono::nanoseconds>(now - startTime).count();

		default:
			return (int)duration_cast<seconds>(now - startTime).count();
		}
		return -1;
	}

	Timer(){
		Reset();
	}

private:
	std::chrono::high_resolution_clock::time_point startTime;
};