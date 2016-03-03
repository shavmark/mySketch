#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract


	void DrawingTools::update(shared_ptr<GraphicEngines> engines) {
		engines->cleanup();
		engines->update();
	}
	// draw all items in need of drawing
	void DrawingTools::draw(shared_ptr<GraphicEngines> engines) {
		engines->draw();
	}
	void DrawingTools::pause(shared_ptr<GraphicEngines> engines) {
		engines->pause();
	}

	void DrawingTools::play(shared_ptr<GraphicEngines> engines) {
		engines->play();
	}

	// its ok also if Controller passes in an object such as a paragraph to copy in
	// bugbug move to a text object
	void Graphics2552::rotateToNormal(ofVec3f normal) {
		normal.normalize();

		float rotationAmount;
		ofVec3f rotationAngle;
		ofQuaternion rotation;

		ofVec3f axis(0, 0, 1);
		rotation.makeRotate(axis, normal);
		rotation.getRotate(rotationAmount, rotationAngle);
		logVerbose("ofRotate " + ofToString(rotationAmount));
		ofRotate(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);
	}

}