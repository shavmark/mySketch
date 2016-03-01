#include "controller.h"
//#include <functional>

namespace Software2552 {

	Timeline::Timeline(){
	}
	// read one act and save it to the list of acts (ie a story)
	bool Timeline::readAct(const string& path) {
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Act act;
		if (act.read(path)) {
			acts.push_back(act);
			return true;
			//drawingTools.addEngines(act.getEngines());
			//drawingTools.setupEngines();
		}
		return false;
	}
	void Timeline::play() { 
		for (auto& a : acts) {
			for (auto& v : a.getEngines()->videos) {
				if (!v.getPlayer().isPlaying()) {
					v.startReadHead();
					v.getPlayer().setPaused(false);
					v.getPlayer().play();
				}
			}
		}
	}

	void Timeline::setup() {
		for (auto& a : acts) {
			a.getEngines()->setup();
		}
	}
	void Timeline::update() { 
		for (auto& a : acts) {
			a.getEngines()->cleanup();
			for (auto& v : a.getEngines()->videos) {
				if (v.okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
					v.getPlayer().update();
				}
			}
		}
		//start(engines->videos);// start if needed
	};

	void Timeline::draw() {
		for (auto& a : acts) {
			for (auto& v : a.getEngines()->videos) {
				if (v.okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
					v.getPlayer().draw(v.getStartingPoint().x, v.getStartingPoint().y);
				}
			}
		}
	};

}