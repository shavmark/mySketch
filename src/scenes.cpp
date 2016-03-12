#include "2552software.h"
#include "scenes.h"

namespace Software2552 {
	void SpaceScene::addPlanet(ofTexture& texture, ofVec3f& start) {
		Planet p = Planet();
		p.texture = texture;
		p.setForTexture(texture);
		float r = ofRandom(5, 100);
		p.set(r, 40);
		p.setWireframe(false);
		p.move(start);
		pictureSpheres.push_back(p);
	}
	void SpaceScene::setup() {
		camera2.setScale(-1, -1, 1);
		image.load("hubble1.jpg");
		light.setAmbientColor(ofFloatColor::white);
		video.create("Clyde.mp4", ofGetWidth() / 2, ofGetHeight() / 2);
		TextureFromImage texture, texture2, texture3, texture4;
		texture.create("hubble1.jpg", ofGetWidth() / 2, ofGetHeight() / 2);

		texture2.create("earth_day.jpg", ofGetWidth() / 2, ofGetHeight() / 2);
		texture3.create("g04_s65_34658.gif", ofGetWidth() / 2, ofGetHeight() / 2);
		texture4.create("Floodwaters_of_Mars_highlight_std.jpg", ofGetWidth() / 2, ofGetHeight() / 2);

		float w = ofGetWidth() - video.getWidth();
		addPlanet(texture, ofVec3f(w, 0, 0));
		addPlanet(texture2, ofVec3f(w + ofGetWidth() / ofRandom(4, 16) + 100, 0, ofGetHeight() / 2));
		addPlanet(texture3, ofVec3f(w + ofGetWidth() / ofRandom(4, 16), 0, ofGetHeight()));
		addPlanet(texture4, ofVec3f(w + ofGetWidth() / ofRandom(4, 16) - 100, 0, ofGetHeight() * 2));

		///next draw video in fbo (put in video class) http://clab.concordia.ca/?page_id=944
		videoSphere.set(250, 180);
		videoSphere.move(ofVec3f(0, 0, 0));
		material.setShininess(120);
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);

	}
	void SpaceScene::update() {
		video.update();
		image.resize(ofGetWidth(), ofGetHeight());
	}
	void SpaceScene::draw() {
		ofBackground(ofColor::white);
		image.draw(0, 0);
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

		light.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth());
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