#include "controller.h"
#include <functional>

namespace Software2552 {
	template<typename T> void Timeline::setupTools(T& v) {
		for (auto& a : v) {
			drawingTools.setup(a.getPlayer());
		}
	}

	Timeline::Timeline(){
	}
	void Timeline::enumerateSetup(Scene &scene) {

		setupTools(scene.getVideo());
		setupTools(scene.getParagraphs());
		setupTools(scene.getAudio());
		setupTools(scene.getTexts());

		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}
	}
	void Timeline::setup() { 
		// read in story then let those objects go free as they are set in enumerate(Setup)
		Story story;
		story.read();
		
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
	}
	void Timeline::update() { 
		drawingTools.update();
	};

	void Timeline::draw() {
		drawingTools.draw();
	};

}