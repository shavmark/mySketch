#include "controller.h"
#include <functional>

namespace Software2552 {

	Timeline::Timeline(){
	}
	template<typename T, typename T2> void Timeline::updateTools(T& v, T2 &v2) {
		for (auto& a : v) {
			drawingTools.update(a.id(), v2);
		}
	}
	template<typename T, typename T2> void Timeline::removeTools(T& v, T2 &v2) {
		for (auto& a : v) {
			drawingTools.removePlayers(a.id(), v2);
		}
	}
	template<typename T> void Timeline::setupTools(T& v) {
		for (auto& a : v) {
			drawingTools.setup(a.getPlayer());
		}
	}

	// if x or y are < 0 then the current x,y are used
	template<typename T, typename T2> void Timeline::drawTools(T& v, T2 &v2) {
		for (auto& a : v) {
			if (a.okToDraw()) {
				drawingTools.draw(a.id(), v2, a.getStartingPoint().x, a.getStartingPoint().y);
			}
		}
	}

	void Timeline::setup() { 
		story.setup();
		enumerate(Setup);
	};
	void Timeline::update() { 
		story.update();
		enumerate(Update);
	};

	void Timeline::draw() {
		// Story does not draw, its just data
		
		enumerate(Draw);
	};
	// enumerate and call passed function
	void Timeline::enumerate(Type type) {
		
		for (auto& act : story.getActs()) {
			// always play the first key onwards
			for (auto& play : act.getPlayList().plays()) {
				Scene lookupscene(play.getKeyName());
				std::vector<Scene>::iterator findscene = find(act.getScenes().begin(), act.getScenes().end(), lookupscene);
				if (findscene != act.getScenes().end()) {
					switch (type) {
					case Setup:
						enumerateSetup(*findscene);
						break;
					case Update:
						enumerateUpdate(*findscene);
						break;
					case Draw:
						enumerateDraw(*findscene);
						break;
					}
					if (findscene->waitOnScene()) {
						break; // only draw one time if blocking, do  not go on to the next one yet
					}
				}
			}
		}
	}
	void Timeline::enumerateDraw(Scene &scene) {

		// ofSoundPlayer does not have a draw function

		drawTools(scene.getParagraphs(), drawingTools.paragraphPlayers);
		drawTools(scene.getVideo(), drawingTools.videoPlayers);
		drawTools(scene.getTexts(), drawingTools.textPlayers);

		//drawTools(scene.getCharacters(), drawingTools.characterPlayers);
		//drawTools(scene.getImages(), drawingTools.imagePlayers);
		//drawTools(scene.getGraphics(), drawingTools.graphicPlayers);

		
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
	// tie data to drawing objects
	void Timeline::enumerateUpdate(Scene &scene) {

		// update is called in Story for each object, calls here do updates for DrawingTools etc
		updateTools(scene.getTexts(), drawingTools.textPlayers);
		updateTools(scene.getVideo(), drawingTools.videoPlayers);

		// ofSoundPlayer ofxParagraph and do not have an update funciton

		//updateTools(scene.getCharacters(), drawingTools.characterPlayers);
		//updateTools(scene.getImages(), drawingTools.imagePlayers);
		//updateTools(scene.getGraphics(), drawingTools.graphicPlayers);

	}

	// remove items that have timed out
	void Timeline::removeExpiredScenes() {
		drawingTools.start(); // start draw
		for (auto& act : story.getActs()) {
			// remove scenes
			vector<Scene>::iterator it = act.getScenes().begin();
			while (it != act.getScenes().end()) {
				// the data is timed out
				if (!it->dataAvailable()) {
					//drawingTools.removeVector(it->getParagraphs(), 0);
					removeTools(it->getVideo(), drawingTools.videoPlayers);
					removeTools(it->getTexts(), drawingTools.textPlayers);
					removeTools(it->getParagraphs(), drawingTools.paragraphPlayers);
					removeTools(it->getAudio(), drawingTools.audioPlayers);
					//removeTools(scene.getCharacters(), drawingTools.characterPlayers);
					//removeTools(scene.getImages(), drawingTools.imagePlayers);
					//removeTools(scene.getGraphics(), drawingTools.graphicPlayers);

					PlayItem item(it->getKey());
					act.getPlayList().remove(item);
					it = act.getScenes().erase(it);
				}
				else {
					++it;
				}
			}
		}
		drawingTools.end(); // end draw
	}
}