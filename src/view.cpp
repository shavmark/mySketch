#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract

	void DrawingTools::cleanup() {
		//removeExpiredItems(videoPlayers); // make derived classes to do fancy things beyond the scope here
		vector<Video>::iterator i = engines->videos.get()->begin();
		while (i != engines->videos.get()->end()) {
			if (i->okToRemove()) {
				i = engines->videos.get()->erase(i);
			}
			else {
				++i;
			}
		}

		removeExpiredItems(paragraphPlayers);
		removeExpiredItems(textPlayers);
		removeExpiredItems(audioPlayers);
	}

	void DrawingTools::setup(Audio& sound) {
		if (sound.getPlayer().load(sound.getLocation())) {
			audioPlayers.push_back(sound);
		}
		else {
			logErrorString("add sound Player");
		}
	}
	void DrawingTools::update() {
		cleanup();

		start(*engines->videos.get());// start if needed
		start(audioPlayers); // start if needed

		update(*engines->videos.get());
		update(textPlayers);

	}
	// draw all items in need of drawing
	void DrawingTools::draw() {
		draw(*engines->videos.get()); // make derived classes to do fancy things beyond the scope here
		draw(paragraphPlayers);
		draw(textPlayers);
	}
	void DrawingTools::pause() {
		pause(paragraphPlayers);
		pause(textPlayers);
		for (auto& v : *engines->videos.get()) {
			if (!v.getPlayer().isPlaying()) {
				v.pause();
				v.getPlayer().setPaused(true);
			}
		}
		for (auto& a : audioPlayers) {
			if (!a.getPlayer().isPlaying()) {
				a.pause();
				a.getPlayer().setPaused(true);
			}
		}
	}

	void DrawingTools::play() {
		startReadHead(paragraphPlayers);
		startReadHead(textPlayers);
		for (auto& v : *engines->videos.get()) {
			if (v.getPlayer().isPlaying()) {
				v.startReadHead();
				v.getPlayer().setPaused(false);
			}
		}
		for (auto& v : audioPlayers) {
			if (v.getPlayer().isPlaying()) {
				v.startReadHead();
				v.getPlayer().setPaused(false);
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