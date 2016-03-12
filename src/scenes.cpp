#include "2552software.h"
#include "scenes.h"

namespace Software2552 {
	void SpaceScene::test() {
		//bugbug get from json
		setBackgroundImageName("hubble1.jpg");
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
		camera2.setScale(-1, -1, 1);
		
		if (backgroundImageName.size() > 0) {
			image.load(backgroundImageName);
		}
		light.setAmbientColor(ofFloatColor::white);
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
		material.setShininess(120);
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);

	}
	void SpaceScene::update() {
		video.update();
		if (backgroundImageName.size() > 0) {
			image.resize(ofGetWidth(), ofGetHeight());
		}
	}
	void SpaceScene::draw() {
		ofBackground(ofColor::white); // white enables all colors in pictures/videos
		if (backgroundImageName.size() > 0) {
			image.draw(0, 0);
		}

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

		light.setPosition(ofGetWidth()/2, ofGetHeight() / 2, ofGetWidth() / 2);
		light.enable();
		camera1.setOrbit();

		camera2.begin();
		video.bind();
		videoSphere.draw();
		video.unbind();
		camera2.end();
		camera1.begin();
		material.begin();
		for (auto& pictureSphere : pictureSpheres) {
			pictureSphere.texture.bind();
			pictureSphere.rotate(30, 0, 2.0, 0.0);
			pictureSphere.draw();
			pictureSphere.texture.unbind();
		}
		material.end();
		camera1.end();

		ofDisableDepthTest();
		light.disable();
		ofDisableLighting();
	}
}