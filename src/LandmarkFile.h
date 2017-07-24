#pragma once
#include "ofMain.h"

class LandmarkFile
{
public:

	LandmarkFile();
	~LandmarkFile();

	int version = 0;
	int nPoints = 0;
	vector<ofVec2f> points;
	ofVec3f pose;

};

