#include "controller.h"
#include <functional>

namespace Software2552 {
	Timeline::Timeline(){
	}
	void Timeline::setup() { 
		story.setup();
		Scene scene;
		// thanks to http://stackoverflow.com/questions/12662891/c-passing-member-function-as-argument
		auto fpSetup = std::bind(&Timeline::enumerateSetup, *this, scene);
		enumerate(fpSetup);
	};
	void Timeline::update() { 
		story.update();
		Scene scene;
		auto fp = std::bind(&Timeline::enumerateUpdate, *this, scene);
		enumerate(fp);
	};

	void Timeline::draw() {
		// Story does not draw, its just data
		Scene scene;
		auto fp = std::bind(&Timeline::enumerateDraw, *this, scene);
		enumerate(fp);
	};
	// enumerate and call passed function
	void Timeline::enumerate(std::function<void(Scene&scene)>func) {
		
		for (auto& act : story.getActs()) {
			// always play the first key onwards
			for (auto& play : act.getPlayList().plays()) {
				Scene lookupscene(play.getKeyName());
				std::vector<Scene>::iterator findscene = find(act.getScenes().begin(), act.getScenes().end(), lookupscene);
				if (findscene != act.getScenes().end()) {
					func(*findscene);
					if (findscene->waitOnScene()) {
						break; // only draw one time if blocking, do  not go on to the next one yet
					}
				}
			}
		}
	}
	void Timeline::enumerateDraw(Scene &scene) {
		
		for (auto& a : scene.getText()) {
			if (a.okToDraw()) {
				drawingTools.drawText(a.id());
			}
		}
		for (auto& a : scene.getAudio()) {
		}
		for (auto& a : scene.getVideo()) {
			if (a.okToDraw()) {
				drawingTools.drawVideo(a.id(), a.getStartingPoint().x, a.getStartingPoint().y);
			}
		}
		
		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}
	}
	void Timeline::enumerateSetup(Scene &scene) {
		
		// setup is called in Story for each object, calls here do updates for DrawingTools etc
		for (auto& a : scene.getText()) {
			a.setup();
			drawingTools.setupText(a.id(), a.text, a.getFont(), a.getStartingPoint().x, 
				a.getStartingPoint().y, a.width, a.getForeground(), a.alignment, a.indent, a.leading, a.spacing);
		}

		for (auto& a : scene.getAudio()) {
		}
		for (auto& a : scene.getVideo()) {
			a.setup();
			drawingTools.setupVideoPlayer(a.id(), a.getVolume(), a.getLocation());
		}
		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}
	}
	void Timeline::enumerateUpdate(Scene &scene) {

		// update is called in Story for each object, calls here do updates for DrawingTools etc

		for (auto& a : scene.getAudio()) {
		}
		for (auto& a : scene.getText()) {
			a.update();
			drawingTools.updateText(a.id());
		}
		for (auto& a : scene.getVideo()) {
			a.update();
			drawingTools.updateVideo(a.id());
		}
		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}
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
					//drawingTools.removeVector(it->getText(), 0);
					for (auto& a : it->getVideo()) {
						drawingTools.removeVideoPlayers(a.id());
					}
					for (auto& a : it->getText()) {
						drawingTools.removeTextPlayers(a.id());
					}
						
					//lef off here for (auto& a : it->getAudio()) {
						//drawingTools.remove(drawingTools.audioPlayers, a.id());
				//	}

					//deleteVector(it->getAudio());
					//deleteVector(it->getVideo());
					//deleteVector(it->getCharacters());
					//deleteVector(it->getImages());
					//deleteVector(it->getGraphics());

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