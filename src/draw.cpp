#include "draw.h"
#include "model.h"

// this is the drawing  module where most drawing work is done

namespace Software2552 {
	void VideoEngine::draw(Video*v) {
		ofVideoPlayer::draw(v->getStartingPoint().x, v->getStartingPoint().y);
	}
	void BackgroundEngine::setup(Colors* colors) { 
		mode = OF_GRADIENT_LINEAR; 
		setRefreshRate(60000);// just set something different while in dev
	}
	// colors and background change over time but not at the same time
	void BackgroundEngine::update(Colors*colors) {
		if (colors->refresh()) {
			colors->getNextColors();
		}
		//bugbug can add other back grounds like a video loop, sound
		// picture, any graphic etc
		if (refresh()) {
			switch ((int)ofRandom(0, 3)) {
			case 0:
				mode = OF_GRADIENT_LINEAR;
				break;
			case 1:
				mode = OF_GRADIENT_CIRCULAR;
				break;
			case 2:
				mode = OF_GRADIENT_BAR;
				break;
			}
		}

	}

	void BackgroundEngine::draw(Colors* colors){
		ofBackgroundGradient(ofColor::fromHex(colors->getCurrentColors().getForeground()),
			ofColor::fromHex(colors->getCurrentColors().getBackground()), mode);
		TextEngine::draw("print", 100,200);
	}
	void TextEngine::draw(const string &s, int x, int y) {
		ofPushStyle();
		ofSetHexColor(Colors::getCurrentColors().getFontColor());
		Font font;
		font.get().drawString(s, x, y);
		ofPopStyle();
	}
	void MeshEngine::setup(ColorSet*colors) {
		//setMode(OF_PRIMITIVE_POINTS);
		setMode(OF_PRIMITIVE_LINES);
		//setMode(OF_PRIMITIVE_LINE_STRIP);
		//setMode(OF_PRIMITIVE_LINE_LOOP);
		enableColors();
		enableIndices();
		ofVec3f top(100.0, 50.0, 0.0);
		ofVec3f left(50.0, 150.0, 0.0);
		ofVec3f right(150.0, 150.0, 0.0);
		addVertex(top);
		addColor(ofFloatColor::fromHex(colors->getForeground()));

		addVertex(left);
		addColor(ofFloatColor::fromHex(colors->getBackground()));

		addVertex(right);
		//bugbug this is where we add in more colors, lets see how many before we make 
		// changes, but somthing like Color1, Color2 etc
		addColor(ofFloatColor::fromHex(colors->getFontColor()));

		addIndex(0);
		addIndex(1);
		addIndex(1);
		addIndex(2);
		addIndex(2);
		addIndex(0);

	}

	void TextEngine::draw(Text* t) {
		ofPushStyle();
		ofSetHexColor(Colors::getCurrentColors().getFontColor());
		t->getFont().drawString(t->getText(), t->getStartingPoint().x, t->getStartingPoint().y);
		ofPopStyle();
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
