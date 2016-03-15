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
	void Stage::draw() {

		ofBackground(ofColor::white); // white enables all colors in pictures/videos

		if (backgroundImageName.size() > 0) {
			imageForBackground.draw(0, 0);
		}

		ofPushStyle();
		//draw2d();
		ofPopStyle();

		ofPushStyle();
		ofBackground(ofColor::blue); // white enables all colors in pictures/videos
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

		camFixed = director.pickem(cameras, false);
		camMoving = director.pickem(cameras, true);


		if (camFixed != nullptr) {
			camFixed->begin();
			for (auto& light : lights) {
				light.enable();
			}
			material.begin();//bugbug figure out material
			draw3dFixed();
			camFixed->end();
		}
		else {
			for (auto& light : lights) {
				light.enable();
			}
			material.begin();//bugbug figure out material
			draw3dFixed();
		}

		if (camMoving != nullptr) {
			camMoving->begin();
			for (auto& light : lights) {
				light.enable();
			}
			camMoving->orbit(); // bugbug make a few more scenes, maye this belongs in an "orbit" derived class
			material.begin();//bugbug figure out material
			draw3dMoving();
			camMoving->end();
		}
		else {
			for (auto& light : lights) {
				light.enable();
			}
			material.begin();//bugbug figure out material
			draw3dMoving();
		}

		material.end();

		ofDisableDepthTest();
		for (auto& light : lights) {
			light.disable();
		}

		ofDisableLighting();
	}
	void Stage::draw2d() {

		ofBackground(ofColor::black);
		ofEnableBlendMode(OF_BLENDMODE_ADD);//bugbug can make these attributes somewhere
		ofSetColor(255, 100);//bugbug figure out alpha in color.h
		for (const auto& image : images) {
			image.draw(image.x, image.y, image.w, image.h);
		}
		ofSetColor(255, 125);
		for (const auto& video : videos) {
			video.draw(video.x, video.y, video.w, video.h);
		}
		ofSetColor(255, 175);
		for (auto& grabber : grabbers) {
			if (grabber.isInitialized()) {
				grabber.draw(grabber.x, grabber.y, grabber.w, grabber.h);
			}
		}
		ofEnableAlphaBlending();
	}
	void Stage::setup() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.load(backgroundImageName);
		}
		for (auto& grabber : grabbers) {
			grabber.loadGrabber(grabber.w, grabber.h);
		}
		for (auto& image : images) {
			image.loadRaster();
		}
		for (auto& video : videos) {
			video.loadVideo();
			video.play();
		}


		material.setShininess(90);
		material.setSpecularColor(ofColor::olive);
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);
	}
	void Stage::update() {
		if (backgroundImageName.size() > 0) {
			imageForBackground.resize(ofGetWidth(), ofGetHeight());
		}

		for (auto& grabber : grabbers) {
			if (grabber.isInitialized()) {
				grabber.update();
			}
		}
		for (auto& image : images) {
			image.update();
		}
		for (auto& video : videos) {
			video.update();
		}
		for (auto& light : lights) {
			//light.setOrientation(ofVec3f(0, cos(ofGetElapsedTimef()) * RAD_TO_DEG, 0));
			//light.setPosition(ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY, 200);
		}

	}
	// juse need to draw the SpaceScene, base class does the rest
	void TestScene::draw3dMoving() {
	}
	void TestScene::draw2d() {
		Stage::draw2d();
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2); // move 0,0 to center
		VectorPattern p;
		//p.stripe(true);
		//p.triangle(true);
		//p.shape(15, 5, false, true, 5);
		//p.matrix(10, 20);
	}
	// all items in test() to come from json
	void TestScene::test() {
		Stage::test();
		Camera cam1;
		add(cam1);

		Light pointLight;
		pointLight.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		pointLight.setSpecularColor(ofColor(255.f, 0, 0));
		pointLight.setPosition(ofGetWidth()*.2, ofGetHeight()*.2, 0);
		pointLight.setPointLight();
		add(pointLight);

		Light pointLight2;
		pointLight2.setDiffuseColor(ofColor(0.f, 0, 255.f));
		// specular color, the highlight/shininess color //
		pointLight2.setSpecularColor(ofColor(255.f, 0, 0));
		pointLight2.setPosition(-ofGetWidth()*.2, ofGetHeight()*.2, 0);
		pointLight2.setPointLight();
		add(pointLight2);

		Light directionalLight;
		// Directional Lights emit light based on their orientation, regardless of their position //
		directionalLight.setDiffuseColor(ofColor(0.f, 0.f, 255.f));
		directionalLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));
		directionalLight.setDirectional();

		// set the direction of the light
		// set it pointing from left to right -> //
		directionalLight.setOrientation(ofVec3f(0, 90, 0));
		add(directionalLight);

		Light basic;
		basic.setPosition(-100, 0, 400);
		add(basic);
		

		Light spotLight;
		spotLight.setDiffuseColor(ofColor(255.f, 0.f, 0.f));
		spotLight.setSpecularColor(ofColor(255.f, 255.f, 255.f));

		// turn the light into spotLight, emit a cone of light //
		spotLight.setSpotlight();

		// size of the cone of emitted light, angle between light axis and side of cone //
		// angle range between 0 - 90 in degrees //
		spotLight.setSpotlightCutOff(50);

		// rate of falloff, illumitation decreases as the angle from the cone axis increases //
		// range 0 - 128, zero is even illumination, 128 is max falloff //
		spotLight.setSpotConcentration(2);
		spotLight.setPosition(-ofGetWidth()*.1, ofGetHeight()*.1, 100);
		add(spotLight);
		return;

		Raster raster("t1_0010.jpg");
		//raster.w = ofGetWidth() / 3;
		add(raster);

		VideoPlayer video("carride.mp4");
		//video.w = ofGetWidth() / 3;
		//video.x = raster.w;
		add(video);

		Grabber grabber("Logitech HD Pro Webcam C920");
		//grabber.w = ofGetWidth() / 3;
		//grabber.x = video.x + video.w;
		add(grabber);


	}
	void TestScene::update() {
		Stage::update();
		mesh.update();
		/* use to show 3 at once
		for (auto& image : getImages()) {
			image.w = ofGetWidth() / 3;
		}
		for (auto& video : getVideos()) {
			video.w = ofGetWidth() / 3;
			video.x = ofGetWidth() / 3;
		}
		for (auto& grabber : getGrabbers()) {
			grabber.w = ofGetWidth() / 3;
			grabber.x = (ofGetWidth() / 3) * 2;
		}
		*/
	}
	void TestScene::draw3dFixed() {
		//ofBackground(0);

		// draw a little light sphere
		for (const auto& light : getLights()) {
			ofSetColor(light.getDiffuseColor());
			ofDrawSphere(light.getPosition(), 20.f);
		}

		cube.draw();

		return; // comment out to draw by vertice

		ofSetHexColor(0xffffff);
		glPointSize(2);
		glEnable(GL_POINT_SMOOTH);
		mesh.drawVertices();
	}
	void TestScene::setup() {
		test();//bugbug set via script 
		Stage::setup();
		ofSetSmoothLighting(true);
		mesh.setup();
		cube.setWireframe(false);
		cube.set(100);
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