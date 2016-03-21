#include "timeline.h"

namespace Software2552 {

	Timeline::Timeline(){
		//bugbug should we get this from data too at some point? these items cannot be set for each object ofColor colorOne(255, 0, 0);
		// bugbug a timeline json?

	}
	// read one act and save it to the list of acts (ie a story)
	bool Timeline::readScript(const string& path) {

		ofxJSON json;

		if (!json.open(path)) {
			logErrorString("Failed to parse JSON " + path);
			return false;
		}


		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			playlist.readFromScript(json);
			if (playlist.getList() == nullptr) {
				logErrorString("missing playlist");
				return false;
			}
			// read all the scenes
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				string sceneType;
				if (readStringFromJson(sceneType, json["scenes"][i]["sceneType"])) {
					shared_ptr<Stage> p=nullptr;
					if (sceneType == "TestBall") {
						p = std::make_shared<TestBallScene>();
					}
					else {
						p = std::make_shared<GenericScene>();
					}
					if (p->create(json["scenes"][i])) {
						playlist.getCurrent()->setStage(p);
					}
				}
			}
					// if a scene is not in the play list do not save it
					//bugbug json needs to add in a named scene that data gets tied to, and what if there is no playlist? we do not want to blow up
					//for (auto& item : *getPlayList()) {
						//if (item->getKeyName() == keyname) {
							//item->scene.readFromScript(json["scenes"][i]);
						//}
					//}
				//}
			//shared_ptr<vector<shared_ptr<SceneReader>>>list = parse<SceneReader>(json["scenes"]);
			//for (auto& item : *playlist.getList()) {
				//logTrace("create look upjson[scenes][keyname][" + item->getKeyName() + "]");
				//for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
					//item->scene.readFromScript(json["scenes"][i]);
			//	}
				//shared_ptr<vector<shared_ptr<SceneReader>>> scenes = parse<SceneReader>(json["scenes"]);
				//item->scene.read(json["scenes"]["keyname"]);

				//string newkeyname;
				//if (readStringFromJson(newkeyname, json["scenes"]["keyname"]["\""+item->getKeyName()+ "\""])) {
					//if (newkeyname.size() == 0) {
						//logErrorString("missing scene " + item->getKeyName());
						//continue;// not found
				//	}
				//}
			//}
			// read all the scenes, the process them one by one
			/*
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				string keyname;
				if (readStringFromJson(keyname, json["scenes"][i]["keyname"])) {
					if (keyname.size() == 0) {
						logErrorString("missing scene " + keyname);
						continue;// not found
					}
					playlist.getList()[i].scene.read(stage, json["scenes"][i]);
					// if a scene is not in the play list do not save it
					//bugbug json needs to add in a named scene that data gets tied to, and what if there is no playlist? we do not want to blow up
					for (auto& item : *getPlayList()) {
						if (item->getKeyName() == keyname) {
							item->scene.read(stage, json["scenes"][i]);
						}
					}
				}
			}
			*/
			// space out waits, but calc out video and other lengths (will result in loading videos)
			float wait = 0;
			if (playlist.getList() == nullptr) {
				for (auto& item : *playlist.getList()) {
					//item.scene.getDrawingEngines()->bumpWaits(wait);
					//wait = item.scene.getDrawingEngines()->getLongestWaitTime();
				}
			}
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}
		// read in story then let those objects go free as they are set in enumerate(Setup)
		//scene.create();
		return false;
	}
	void Timeline::play() { 

	}
	void Timeline::pause() {
	}
	void Timeline::setup() {
		//ofSeedRandom(); // turn of to debug if needed
		ofSetFrameRate(frameRate);
		playlist.getCurrent()->start();
		playlist.getCurrent()->getStage()->setup();

		return;

		mesh.setup();
	}
	// keep this super fast
	void Timeline::update() { 
		playlist.removeExpiredItems();
		playlist.getCurrent()->getStage()->update();
	};

	// keep as fast as possible
	void Timeline::draw() {
		//mesh.draw();
		playlist.getCurrent()->getStage()->draw();
		return;
	};

}