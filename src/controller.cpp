#include "controller.h"
#include <functional>

namespace Software2552 {

	void Timeline::setup() { 
		story.setup(); 
		Scene scene;
		// thanks to http://stackoverflow.com/questions/12662891/c-passing-member-function-as-argument
		auto fp = std::bind(&Timeline::enumerateSetup, *this, scene);
		enumerate(fp);
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
		
		for (auto& scenes : story.getScenes()) {
			// always play the first key onwards
			for (auto& play : scenes.getPlayList().plays()) {
				Scene lookupscene(play.getKeyName());
				std::vector<Scene>::iterator findscene = find(scenes.getScenes().begin(), scenes.getScenes().end(), lookupscene);
				if (findscene != scenes.getScenes().end()) {
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
				a.getText().draw(a.getStartingPoint().x, a.getStartingPoint().y);
			}
		}
		for (auto& a : scene.getAudio()) {
		}

		for (int i = 0; i < scene.getVideo().size();++i ) {
			if (scene.getVideo()[i].okToDraw()) {
				tools.videoPlayers[i].draw(scene.getVideo()[i].getStartingPoint().x, scene.getVideo()[i].getStartingPoint().y);
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
		
		// setup is called in Story for each object, calls here do updates for Tools etc

		for (auto& a : scene.getAudio()) {
		}
		for (int i = 0; i < scene.getVideo().size(); ++i) {
			scene.getVideo()[i].setup();
			tools.videoPlayers[i].loadMovie(scene.getVideo()[i].getLocation());
			tools.videoPlayers[i].setVolume(scene.getVideo()[i].getVolume());
			tools.videoPlayers[i].play();
		}
		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}
	}
	void Timeline::enumerateUpdate(Scene &scene) {

		// update is called in Story for each object, calls here do updates for Tools etc

		for (auto& a : scene.getAudio()) {
		}
		for (int i = 0; i < scene.getVideo().size(); ++i) {
			scene.getVideo()[i].update();
			tools.videoPlayers[i].update();
		}
		for (auto& a : scene.getCharacters()) {
		}
		for (auto& a : scene.getImages()) {
		}
		for (auto& a : scene.getGraphics()) {
		}
	}

	void Timeline::removeExpiredScenes() {
		//bugbug seems to be one layer too many, can one layer be removed?
		for (auto& scenes : story.getScenes()) {
			vector<Scene>::iterator it = scenes.getScenes().begin();
			while (it != scenes.getScenes().end()) {
				if (!it->dataAvailable()) {
					PlayItem item(it->getKey());
					scenes.getPlayList().remove(item);
					it = scenes.getScenes().erase(it);
					//bugbug -- remove related Tools resources, not obvious -- good time to break
				}
				else {
					++it;
				}
			}
		}
	}

}