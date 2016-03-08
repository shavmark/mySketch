#include "timeline.h"

namespace Software2552 {

	Timeline::Timeline(){
		//bugbug should we get this from data too at some point? these items cannot be set for each object ofColor colorOne(255, 0, 0);
		// bugbug a timeline json?

	}
	// read one act and save it to the list of acts (ie a story)
	bool Timeline::readAct(const string& path) {
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Act act;
		if (act.read(path)) {
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
		theSet.setup();
		return;

		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->setup();
			}
		}
	}
	// keep this super fast
	void Timeline::update() { 
		theSet.update();
		return;

		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->cleanup();
				item.scene.getDrawingEngines()->update();
			}
		}
	};

	// keep as fast as possible
	void Timeline::draw() {
		theSet.draw();
		ColorSet color = Colors::getCurrentColors();
		mesh.draw(&color);
		return;
		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.getDrawingEngines()->draw();
			}
		}
	};

}