#include "controller.h"
#include <functional>

namespace Software2552 {
	template<typename T> void Timeline::setup(T& v) {
		for (auto& a : v) {
			a.addWait(longestWaitTime);
			drawingTools.setup(a);
		}
	}

	Timeline::Timeline(){
		longestWaitTime = 0;
	}
	void Timeline::readStory(const string& path, const string& title) {
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Story story;
		story.read(path, title);
		drawingTools.addEngines(story.getEngines());
		drawingTools.setupEngines();
		// create drawing objects for all items in play list, then list the Story free up

		setup();

	}
	void Timeline::setup() {
		drawingTools.play();
	}
	void Timeline::update() { 
		drawingTools.update();
	};

	void Timeline::draw() {
		drawingTools.draw();
	};

}