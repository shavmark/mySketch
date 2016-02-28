#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract

	void DrawingTools::cleanup() {
		removeExpiredItems(videoPlayers); // make derived classes to do fancy things beyond the scope here
		removeExpiredItems(paragraphPlayers);
		removeExpiredItems(textPlayers);
		removeExpiredItems(audioPlayers);
	}

	void DrawingTools::setup(shared_ptr<Wrapper<ofVideoPlayer>> player) {
		if (player->load(player->getLocation())) {
			videoPlayers.push_back(player);
		}
		else {
			logErrorString("add video Player");
		}
	}
	void DrawingTools::setup(shared_ptr< Wrapper<ofSoundPlayer>> player) {
		if (player->load(player->getLocation())) {
			audioPlayers.push_back(player);
		}
		else {
			logErrorString("add sound Player");
		}
	}
	void DrawingTools::update() {
		cleanup();
		start(videoPlayers);
		start(audioPlayers);
		update(videoPlayers);
		update(textPlayers);

	}
	// draw all items in need of drawing
	void DrawingTools::draw() {
		draw(videoPlayers); // make derived classes to do fancy things beyond the scope here
		draw(paragraphPlayers);
		draw(textPlayers);
	}
	void DrawingTools::play() {
		startReadHead(videoPlayers);
		startReadHead(paragraphPlayers);
		startReadHead(textPlayers);
		startReadHead(audioPlayers);
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