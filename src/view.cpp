#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract


	void DrawingTools::update(shared_ptr<GraphicEngines> engines) {
		engines->cleanup();

		//start(engines->videos);// start if needed
		//start(engines->audios); // start if needed

		update(engines->videos);
		update(engines->texts);
		update(engines->images);
		update(engines->characters);

	}
	// draw all items in need of drawing
	void DrawingTools::draw(shared_ptr<GraphicEngines> engines) {
		draw(engines->videos);
		draw(engines->paragraphs);
		draw(engines->texts);
		draw(engines->images);
		draw(engines->characters);
	}
	void DrawingTools::pause(shared_ptr<GraphicEngines> engines) {
		pause(engines->paragraphs);
		pause(engines->texts);
		pause(engines->images);
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

	void DrawingTools::play(shared_ptr<GraphicEngines> engines) {
		startReadHead(engines->paragraphs);
		startReadHead(engines->images);
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