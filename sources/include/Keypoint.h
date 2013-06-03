#pragma once

#include <vector>

using namespace std;

class Keypoint
{
public:
	float			x;
	float			y;
	vector<double>	magnitudes;		// The list of magnitudes at this point
	vector<double>	orientations;	// The list of orientations detected
	unsigned int	scale;			// The scale where this was detected

	Keypoint() { }
	Keypoint(float x, float y) : x(x), y(y) { }
	Keypoint(float x, float y, const vector<double>& m, const vector<double>& o, unsigned int s) :
		x(x), y(y), magnitudes(m), orientations(o), scale(s)	{ }
};