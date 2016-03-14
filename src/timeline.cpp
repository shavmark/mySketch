#include "timeline.h"

namespace Software2552 {

	Timeline::Timeline(){
		//bugbug should we get this from data too at some point? these items cannot be set for each object ofColor colorOne(255, 0, 0);
		// bugbug a timeline json?

	}
	// read one act and save it to the list of acts (ie a story)
	bool Timeline::readScript(const string& path) {
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Act act;
		if (act.readFromScript(path)) {
			acts.push_back(act);
			return true;
		}
		return false;
	}
	void Timeline::play() { 
		return;

		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->play();
			}
		}
	}
	void Timeline::pause() {
		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->pause();
			}
		}
	}
	void Timeline::setup() {
		ofSeedRandom(); // turn of to debug if needed

		ofSetFrameRate(60);
		scene.setup();
		return;
		theSet.setup();
		mesh.setup();
		return;

		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->setup();
			}
		}
	}
	// keep this super fast
	void Timeline::update() { 
		scene.update();
		//theSet.update();
		return;

		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->removeExpiredItems();
				item.scene.getDrawingEngines()->update();
			}
		}
	};

	// keep as fast as possible
	void Timeline::draw() {
		//theSet.draw();
		//mesh.draw();
		scene.draw();
		return;
		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->draw();
			}
		}
	};

}