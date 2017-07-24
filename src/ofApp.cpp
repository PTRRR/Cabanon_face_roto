#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofDirectory dir(landmarksPath);
	dir.allowExt("pts");
	dir.listDir();

	for (int i = 0; i < dir.size(); i++) {

		string fileName = ofSplitString(dir.getName(i), "_det_")[0];

		vector<ofVec2f> currentLandmarks = getLandmarks(dir.getPath(i));
		landmarks.push_back(currentLandmarks);

		string currentImagePath = imagesPath + "/" + fileName + ".jpg";
		ofImage image;
		image.load(currentImagePath);

		images.push_back(image);
		
		//ofImage cropedFace = cropFace(image, landmarks);
		//cropedFace.save(toPrintPath + ofToString(i) + ".jpg", OF_IMAGE_QUALITY_BEST);

		// Delete old files

		ofFile oldImageFile;
		oldImageFile.open(currentImagePath);
		//oldImageFile.remove();

		ofFile oldLandmarksFile;
		oldLandmarksFile.open(dir.getPath(i));
		//oldLandmarksFile.remove();

	}

	//ofExit();

}

ofImage ofApp::cropFace(ofImage image, vector<ofVec2f> landmarks) {

	ofImage capture;
	ofSetWindowShape(image.getWidth(), image.getHeight());
	image.draw(0, 0);
	//capture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	return capture;

}

vector<ofVec2f> ofApp::getLandmarks(string filePath) {

	vector<ofVec2f> landmarks;
	ifstream file; //declare a file stream  
	file.open(ofToDataPath(filePath).c_str()); //open your text file

	int lineIndex = 0;
	string line;
	int nPoints = 0;

	while (getline(file, line)) //as long as theres still text to be read  
	{

		if (lineIndex == 1) {

			nPoints = std::stoi(ofSplitString(line, ": ")[1]);

		}
		else if (lineIndex > 2 && lineIndex <= 2 + nPoints) {

			landmarks.push_back(ofVec2f(std::stof(ofSplitString(line, " ")[0]), std::stof(ofSplitString(line, " ")[1])));

		}

		lineIndex++;

	}

	file.close();

	return landmarks;

}

vector<ofVec2f> ofApp::getFacialContourLandmarks(vector<ofVec2f> landmarks) {

	vector<ofVec2f> facialContourLandmarks;

	for (int i = 0; i <= 16; i++) {

		facialContourLandmarks.push_back(landmarks[i]);

	}

	// Get bounding box

	auto boundingBox = getBoundingBox(landmarks);

	// Crate upper landmarks

	ofVec2f first = landmarks[0];
	ofVec2f last = landmarks[16];

	ofVec2f dir = first - last;
	float length = (last - first).length();
	ofVec2f perp = dir.getPerpendicular();

	ofVec2f midPoint = first + perp * ( first.y - boundingBox[0].y ) * 1.5 - dir * 0.5;

	facialContourLandmarks.push_back(midPoint);

	return facialContourLandmarks;

}

vector<ofVec2f> ofApp::getBoundingBox(vector<ofVec2f> landmarks) {

	ofVec2f topLeft = ofVec2f(INFINITE, INFINITE);
	ofVec2f	bottomRight = ofVec2f(0, 0);

	for (int i = 0; i < landmarks.size(); i++) {

		if (landmarks[i].x < topLeft.x) topLeft.x = landmarks[i].x;
		if (landmarks[i].y < topLeft.y) topLeft.y = landmarks[i].y;
		if (landmarks[i].x > bottomRight.x) bottomRight.x = landmarks[i].x;
		if (landmarks[i].y > bottomRight.y) bottomRight.y = landmarks[i].y;

	}

	vector<ofVec2f> boundingBox;
	boundingBox.push_back(topLeft);
	boundingBox.push_back(bottomRight);

	return boundingBox;

}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	ofEnableAlphaBlending();
	ofDrawCircle(30, 30, 20);

	for (int i = 0; i < images.size(); i++) {

		ofImage currentImage = images[i];
		vector<ofVec2f> currentLandmarks = landmarks[i];
		vector<ofVec2f> facialContour = getFacialContourLandmarks(currentLandmarks);
		vector<ofVec2f> boundingBox = getBoundingBox(facialContour);
		ofVec2f topLeft = boundingBox[0];
		ofVec2f dimensions = ofVec2f(boundingBox[1].x - boundingBox[0].x, boundingBox[1].y - boundingBox[0].y);
		ofSetWindowShape(currentImage.getWidth(), currentImage.getHeight());

		// Polyline & mask

		ofFbo fbo;
		ofPolyline faceContour;

		fbo.allocate(currentImage.getWidth(), currentImage.getHeight());
		fbo.begin();

		currentImage.draw(0, 0, currentImage.getWidth(), currentImage.getHeight());

		// Draw landmarks for debug

		for (int j = 0; j < facialContour.size(); j++) {

			faceContour.addVertex(facialContour[j].x, facialContour[j].y);
			ofDrawCircle(facialContour[j].x, facialContour[j].y, 2);
			//ofDrawBitmapString(ofToString(j), facialContour[j].x, facialContour[j].y);

		}

		faceContour.close();
		faceContour = faceContour.getSmoothed(1.2, 0.5);
		faceContour.draw();

		fbo.end();

		ofPixels pixels;
		fbo.readToPixels(pixels);

		ofDrawCircle(currentImage.getWidth() * 0.5, currentImage.getHeight() * 0.5, 10);

		ofPixels imagePixels = currentImage.getPixels();
		ofImage newImage;
		newImage.allocate(ceil(dimensions.x), ceil(dimensions.y), OF_IMAGE_COLOR_ALPHA);

		for (int j = 0; j < dimensions.x; j++) {

			for (int k = 0; k < dimensions.y; k++) {

				if(faceContour.inside(j + topLeft.x, k + topLeft.y)) newImage.setColor(j, k, pixels.getColor(j + topLeft.x, k + topLeft.y));
				else
				{
					newImage.setColor(j, k, ofColor(0, 0, 0, 0));
				}

			}

		}

		ofSaveImage(newImage.getPixels(), toPrintPath + ofToString(i) + ".png", OF_IMAGE_QUALITY_BEST);
		fbo.draw(0, 0);

	}

}
