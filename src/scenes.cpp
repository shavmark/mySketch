#include "2552software.h"
#include "scenes.h"

namespace Software2552 {

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
					int j = ofRandom(0, cameras.size());
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
			return nullptr;
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
	}

	void Stage::draw3d() {
		// bugbug should this go into a virtual function so it can be changed by derived classes?
		ofSetSmoothLighting(true);
		ofDisableAlphaBlending();
		ofEnableDepthTest();

		for (auto& light : lights) {
			light.enable();
		}

		camFixed = director.pickem(cameras, false);
		camMoving = director.pickem(cameras, true);

		material.begin();//bugbug figure out material

		if (camFixed != nullptr) {
			camFixed->begin();
			draw3dFixed();
			camFixed->end();
		}
		else {
			draw3dFixed();
		}

		if (camMoving != nullptr) {
			camMoving->begin();
			camMoving->orbit(); // bugbug make a few more scenes, maye this belongs in an "orbit" derived class
			draw3dMoving();
			camMoving->end();
		}
		else {
			draw3dMoving();
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
		material.setSpecularColor(ofColor(255, 255, 255, 255));
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);
	}
	// juse need to draw the SpaceScene, base class does the rest
	void TestScene::draw3dMoving() {
	}
	// all items in test() to come from json
	void TestScene::test() {
		Stage::test();
		Camera cam1;
		add(cam1);

		Light light;//bugbug make arrays of spot etc light types for director to use
		
		light.setAmbientColor(ofFloatColor::blue);
		light.setDiffuseColor(ofFloatColor::red);
		light.setSpecularColor(ofFloatColor::green);
		light.setPosition(300, 0, 0);
		light.setDiffuseColor(ofFloatColor(255.0, 0.0, 0.0f));
		light.setSpecularColor(ofColor(0, 0, 255));
		light.setDirectional();
		light.setSpotConcentration(5);
		light.setSpotlightCutOff(10);

		add(light);
	}
	void TestScene::update() {
		mesh.update();

	}
	void TestScene::draw3dFixed() {
		//ofBackground(0);
		cube.draw();
		ofSetHexColor(0xffffff);
		glPointSize(2);
		glEnable(GL_POINT_SMOOTH);
		mesh.drawVertices();
	}
	void TestScene::setup() {
		ofSetGlobalAmbientColor(ofColor(0, 0, 0));
		ofSetSmoothLighting(true);
		test();//bugbug set via script 
		mesh.setup();
		cube.setWireframe(false);
		cube.set(500);
	}
	void Stage::update() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.resize(ofGetWidth(), ofGetHeight());
		}
	}
	void SpaceScene::test() {
		//bugbug get from json
		Stage::test();
		//bugbug get from json
		Camera cam1; // learn camera bugbug
		cam1.setScale(-1, -1, 1); // showing video
		cam1.setOrbit(true); // rotating
		add(cam1);

		cam1.setOrbit(false); // not rotating
		cam1.setScale(-1, -1, 1); // showing video
		add(cam1);

		Light light1;
		add(light1);

		Light light2;
		add(light2);

		setBackgroundImageName("hubble1.jpg");
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
		p.mapTexCoordsFromTexture(texture);
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
			videoSphere.mapTexCoordsFromTexture(video.getTexture());
		}
		video.bind();
		videoSphere.draw();
		video.unbind();
	}
}