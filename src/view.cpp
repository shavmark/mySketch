#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract


	void DrawingTools::update() {
		engines->cleanup();

		start(engines->videos);// start if needed
		start(engines->audios); // start if needed

		update(engines->videos);
		update(engines->texts);

	}
	// draw all items in need of drawing
	void DrawingTools::draw() {
		draw(engines->videos); // make derived classes to do fancy things beyond the scope here
		draw(engines->paragraphs);
		draw(engines->texts);
	}
	void DrawingTools::pause() {
		pause(engines->paragraphs);
		pause(engines->texts);
		for (auto& v : engines->videos) {
			if (!v.getPlayer().isPlaying()) {
				v.pause();
				v.getPlayer().setPaused(true);
			}
		}
		for (auto& a : engines->audios) {
			if (!a.getPlayer().isPlaying()) {
				a.pause();
				a.getPlayer().setPaused(true);
			}
		}
	}

	void DrawingTools::play() {
		startReadHead(engines->paragraphs);
		startReadHead(engines->texts);
		for (auto& v : engines->videos) {
			if (!v.getPlayer().isPlaying()) {
				v.startReadHead();
				v.getPlayer().setPaused(false);
				v.getPlayer().play();
			}
		}
		for (auto& v : engines->audios) {
			if (!v.getPlayer().isPlaying()) {
				v.startReadHead();
				v.getPlayer().setPaused(false);
				v.getPlayer().play();
			}
		}

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