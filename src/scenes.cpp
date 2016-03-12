#include "2552software.h"
#include "scenes.h"

namespace Software2552 {
	void BaseScene::test() {
		//bugbug get from json
		setBackgroundImageName("hubble1.jpg");
	}
	void BaseScene::draw() {
		ofBackground(ofColor::white); // white enables all colors in pictures/videos

		if (backgroundImageName.size() > 0) {
			imageForBackground.draw(0, 0);
		}
	}
	void BaseScene::setup() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.load(backgroundImageName);
		}
		mainCamera.setScale(-1, -1, 1);

		Camera cam1; // learn camera bugbug
		cam1.setScale(-1, -1, 1);
		cam1.setOrbit(true);
		//cameras.push_back(cam1);

		Light light1;
		lights.push_back(light1);

		Light light2;
		lights.push_back(light1);

		material.setShininess(120);
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);

	}
	void BaseScene::update() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.resize(ofGetWidth(), ofGetHeight());
		}
	}
	void SpaceScene::test() {
		//bugbug get from json
		BaseScene::test();
		setMainVideoName("Clyde.mp4");
		addPlanetName("hubble1.jpg");
		addPlanetName("earth_day.jpg");
		addPlanetName("g04_s65_34658.gif");
		addPlanetName("Floodwaters_of_Mars_highlight_std.jpg");
		addPlanetName("hubble1.jpg");
		addPlanetName("earth_day.jpg");
		addPlanetName("g04_s65_34658.gif");
		addPlanetName("Floodwaters_of_Mars_highlight_std.jpg");
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
		BaseScene::setup();
		video.create(mainVideoName, ofGetWidth() / 2, ofGetHeight() / 2);

		float xStart = (ofGetWidth() - video.getWidth())/2;
		for (auto& name : planetimageNames) {
			float offset = abs(xStart);
			addPlanet(name, ofVec3f(xStart, 0, offset +100));
			xStart += ofRandom(xStart, xStart*2); // need to keep sign
		}

		///next draw video in fbo (put in video class) http://clab.concordia.ca/?page_id=944
		videoSphere.set(250, 180);
		videoSphere.move(ofVec3f(0, 0, 0));

	}
	void SpaceScene::update() {
		BaseScene::update();
		video.update();
	}
	void SpaceScene::draw() {
		BaseScene::draw();

		draw2d();
		ofPushStyle();
		draw3d();
		ofPopStyle();
	}
	void SpaceScene::draw2d() {
	}
	void SpaceScene::draw3d() {
		if (video.isFrameNew()) {
			videoSphere.setForTexture(video.getTexture());
		}
		ofSetSmoothLighting(true);
		ofDisableAlphaBlending();
		ofEnableDepthTest();

		for (auto& light : lights) {
			light.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth() / 2);
			light.enable();
		}

		for (auto& camera : cameras) {
			camera.orbit();
			camera.begin();
		}
		mainCamera.begin();
		video.bind();
		videoSphere.draw();
		video.unbind();
		material.begin();
		for (auto& pictureSphere : pictureSpheres) {
			pictureSphere.texture.bind();
			pictureSphere.rotate(30, 0, 2.0, 0.0);
			pictureSphere.draw();
			pictureSphere.texture.unbind();
		}
		material.end();
		mainCamera.begin();
		for (auto& camera : cameras) {
			camera.end();
		}

		ofDisableDepthTest();
		for (auto& light : lights) {
			light.disable();
		}

		ofDisableLighting();
	}
}