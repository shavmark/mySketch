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
	void Timeline::enumerateSetup(Scene &scene) {
		drawingTools.addEngines(scene.getEngines());
		scene.getEngines()->videos.setup(longestWaitTime);
		setup(scene.getAudio());
		setup(scene.getParagraphs());
		setup(scene.getTexts());

		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}

		// find the longest duration to be added to the next scene (duration of 0 does not block)
		longestWaitTime += drawingTools.getLongestWaitTime(); // add it to subsequent items
	}
	void Timeline::readStory(const string& path, const string& title) {
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Story story;
		story.read(path, title);

		// drop story? to many unneeded layers?

		// create drawing objects for all items in play list, then list the Story free up

		for (auto& act : story.getActs()) {
			// setup all items in the play list (the rest will not be drawn)
			for (auto& play : act.getPlayList().plays()) {
				Scene lookupscene(play.getKeyName());
				std::vector<Scene>::iterator findscene = find(act.getScenes().begin(), act.getScenes().end(), lookupscene);
				if (findscene != act.getScenes().end()) {
					enumerateSetup(*findscene);
				}
			}
		}
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