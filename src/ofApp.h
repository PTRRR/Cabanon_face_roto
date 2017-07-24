#pragma once

#include "ofMain.h"
#include <fstream>
#include <stdlib.h>

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		string imagesPath = "C:/Users/Pietro Alberti/Documents/Code/Cabanon/downloaded";
		string toPrintPath = "C:/Users/Pietro Alberti/Documents/Code/Cabanon/to_print/";
		string landmarksPath = "C:/Users/Pietro Alberti/Documents/Code/Cabanon/landmark_files";

		vector<ofVec2f> getLandmarks(string filePath);
		vector<ofVec2f> getFacialContourLandmarks(vector<ofVec2f> landmarks);
		ofImage cropFace(ofImage image, vector<ofVec2f> landmarks);
		vector<ofVec2f> getBoundingBox(vector<ofVec2f> landmarks);

		vector<ofImage> images;
		vector<vector<ofVec2f>> landmarks;
		
};
