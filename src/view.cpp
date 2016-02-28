#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract



	// specialzed drawing needed bugbug move to a text object
	void DrawingTools::drawText(GraphicID ID, int x, int y) {
		for (auto& t : textPlayers) {
			if (t.id() == ID) {
				if (t.font != nullptr) {
					ofPushStyle();
					ofSetColor(t.color);
					t.font->draw(t.text, x, y);
					ofPopStyle();
				}
			}
		}
	}

	// its ok also if Controller passes in an object such as a paragraph to copy in
	// bugbug move to a text object
	void DrawingTools::setupText(GraphicID ID, const string& text, shared_ptr<ofxSmartFont> font, int x, int y, const ofColor& color){
		Wrapper<TextToRender> newtext(ID);
		newtext.font = font;
		newtext.x = x;
		newtext.y = y;
		newtext.color = color;
		newtext.text = text;
		textPlayers.push_back(newtext);

	}
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