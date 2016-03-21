#include "timeline.h"
#include "model.h"
namespace Software2552 {

	Timeline::Timeline(){
		//bugbug should we get this from data too at some point? these items cannot be set for each object ofColor colorOne(255, 0, 0);
		// bugbug a timeline json?

	}
	// read one act and save it to the list of acts (ie a story)
	bool Timeline::readScript(const string& path) {

		if (playlist.read(path)) {

			playlist.getCurrent()->start();
			playlist.getCurrent()->getStage()->setup();

		}
		return false;
	}
	void Timeline::play() { 

	}
	void Timeline::pause() {
	}
	void Timeline::setup() {
		//ofSeedRandom(); // turn of to debug if needed
		ofSetFrameRate(frameRate);
		Colors colors;// setup colors

		return;

		mesh.setup();
	}
	// keep this super fast
	void Timeline::update() { 
		playlist.removeExpiredItems();
		if (playlist.getCurrent() != nullptr) {
			playlist.getCurrent()->getStage()->update();
		}
	};

	// keep as fast as possible
	void Timeline::draw() {
		//mesh.draw();
		if (playlist.getCurrent() != nullptr) {
			playlist.getCurrent()->getStage()->draw();
		}
	};

}