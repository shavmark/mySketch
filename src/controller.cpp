#include "controller.h"

namespace Software2552 {

	Timeline::Timeline(){
		framerate = 60;//bugbug should we get this from data too at some point? these items cannot be set for each object
		engines = std::make_shared<GraphicEngines>();
	}
	// read one act and save it to the list of acts (ie a story)
	bool Timeline::readAct(const string& path) {
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Act act;
		if (act.read(path)) {
			acts.push_back(act);
			for (auto& item : act.getPlayList()) {
				engines->add(item.scene.getEngines());
			}
			return true;
		}
		return false;
	}
	void Timeline::play() { 
		for (auto& v : engines->videos) {
			if (v.okToDraw() && !v.getPlayer().isPlaying()) {
				v.startReadHead();
				v.getPlayer().setPaused(false);
				v.getPlayer().play();
			}
		}
	}
	void Timeline::pause() {

	}
	void Timeline::setup() {
		ofSetFrameRate(framerate);
		for (auto& a : acts) {
			for (auto& item : a.getPlayList()) {
				item.scene.setup();
			}
		}
	}
	// keep this super fast
	void Timeline::update() { 
		engines->cleanup();
		for (auto& v : engines->videos) {
			if (v.okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
				v.getPlayer().update();
			}
		}
		//start(engines->videos);// start if needed
	};

	// keep as fast as possible
	void Timeline::draw() {
		for (auto& v : engines->videos) {
			if (v.okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
				v.getPlayer().draw(v.getStartingPoint().x, v.getStartingPoint().y);
			}
		}

	};

}