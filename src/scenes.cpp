#include "2552software.h"
#include "scenes.h"

namespace Software2552 {

	void Director::update() {
		// remove out of data cameras
	}
	// return a possibly modifed version such as camera moved
	Camera* Director::pickem(vector<Camera>&cameras, bool orbiting) {
		// maybe build this out using the color count techqiue (make that a base class?)
		for (int i = 0; i < cameras.size(); ++i) {
			if (orbiting) {
				if (cameras[i].isOrbiting()) {
					return &cameras[i];
				}
			}
			else {
				if (!cameras[i].isOrbiting()) {
					int j = ofRandom(0, 2);
					return &cameras[j];// stuff data in the Camera class
					// lots to do here to make things nice, learn and see how to bring this in
					//void lookAt(const ofNode& lookAtNode, const ofVec3f& upVector = ofVec3f(0, 1, 0));
					if (j == 1) {
						cameras[j].move(-100,0,0);
						cameras[j].pan(5);
					}
					else {
						cameras[j].move(100, 0, 0);
					}
					//bugbug just one thing we can direct http://halfdanj.github.io/ofDocGenerator/ofEasyCam.html#CameraSettings
					float f = cameras[j].getDistance();
					cameras[j].setDistance(cameras[j].getDistance() + ofRandom(-100,100));
					return &cameras[j];
				}
			}
		}
	}
	void Stage::draw2d() {
	}
	void Stage::draw() {

		ofBackground(ofColor::white); // white enables all colors in pictures/videos

		if (backgroundImageName.size() > 0) {
			imageForBackground.draw(0, 0);
		}

		ofPushStyle();
		draw2d();
		draw3d();
		ofPopStyle();
	}

	void Stage::test() {
		//bugbug get from json
		setBackgroundImageName("hubble1.jpg");
		Camera cam1; // learn camera bugbug
		cam1.setScale(-1, -1, 1);
		cam1.setOrbit(true); // rotating
		cameras.push_back(cam1);

		cam1.setScale(-1, -1, 1);
		cam1.setOrbit(false); // not rotating
		cameras.push_back(cam1);
		cameras.push_back(cam1); // just make a few

		Light light1;
		lights.push_back(light1);

		Light light2;
		lights.push_back(light2);
	}
	void Stage::draw3d() {
		// bugbug should this go into a virtual function so it can be changed by derived classes?
		ofSetSmoothLighting(true);
		ofDisableAlphaBlending();
		ofEnableDepthTest();

		for (auto& light : lights) {
			light.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2);
			light.enable();
		}

		camFixed = director.pickem(cameras, false);
		camMoving = director.pickem(cameras, true);

		material.begin();

		if (camFixed != nullptr) {
			camFixed->begin();
			draw3dFixed();
			camFixed->end();
		}

		if (camMoving != nullptr) {
			camMoving->begin();
			camMoving->orbit(); // bugbug make a few more scenes, maye this belongs in an "orbit" derived class
			draw3dMoving();
			camMoving->end();
		}
		
		material.end();

		ofDisableDepthTest();
		for (auto& light : lights) {
			light.disable();
		}

		ofDisableLighting();
	}
	void Stage::setup() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.load(backgroundImageName);
		}


		material.setShininess(120);
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);

	}
	void Stage::update() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.resize(ofGetWidth(), ofGetHeight());
		}
	}
	void SpaceScene::test() {
		//bugbug get from json
		Stage::test();
		setMainVideoName("Clyde.mp4");
		addPlanetName("hubble1.jpg");
		addPlanetName("earth_day.jpg");
		addPlanetName("g04_s65_34658.gif");
		addPlanetName("Floodwaters_of_Mars_highlight_std.jpg");

	}

	void SpaceScene::addPlanet(const string&textureName, const ofVec3f& start) {
		Planet p = Planet();
		float r = ofRandom(5, 100);
		p.set(r, 40);
		TextureFromImage texture;
		texture.create(textureName, r * 2, r * 2);
		p.setForTexture(texture);
		p.texture = texture;
		p.setWireframe(false);
		p.move(start);
		pictureSpheres.push_back(p);
	}
	void SpaceScene::setup() {
		test();//bugbug set via script 
		Stage::setup();
		video.create(mainVideoName, ofGetWidth() / 2, ofGetHeight() / 2);

		float xStart = (ofGetWidth() - video.getWidth())/2;
		int y = 0;
		for (const auto& name : planetimageNames) {
			float offset = abs(xStart);
			addPlanet(name, ofVec3f(xStart, y, offset +100));
			y = ofRandom(0, 100);
			xStart += ofRandom(xStart, xStart*2); // need to keep sign
		}

		///next draw video in fbo (put in video class) http://clab.concordia.ca/?page_id=944
		videoSphere.set(250, 180);
		videoSphere.move(ofVec3f(0, 0, 0));

	}
	void SpaceScene::update() {
		Stage::update();
		video.update();
	}
	void SpaceScene::draw2d() {
	}
	// juse need to draw the SpaceScene, base class does the rest
	void SpaceScene::draw3dMoving() {
		for (auto& pictureSphere : pictureSpheres) {
			pictureSphere.texture.bind();
			pictureSphere.rotate(30, 0, 2.0, 0.0);
			pictureSphere.draw();
			pictureSphere.texture.unbind();
		}
	}
	void SpaceScene::draw3dFixed() {
		// one time setup must be called to draw videoSphere
		if (video.isFrameNew()) {
			videoSphere.setForTexture(video.getTexture());
		}
		video.bind();
		videoSphere.draw();
		video.unbind();
	}
}