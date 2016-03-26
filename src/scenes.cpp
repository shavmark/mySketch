#include "2552software.h"
#include "model.h"
#include "scenes.h"
#include "animation.h"

//By default, the screen is cleared with every draw() call.We can change that with 
//  ofSetBackgroundAuto(...).Passing in a value of false turns off the automatic background clearing.

namespace Software2552 {


	// convert name to object
	shared_ptr<Stage> getScene(const string&name)
	{
		if (name == "TestBall") {
			return std::make_shared<TestBallScene>();
		}
		if (name == "Space") {
			return std::make_shared<SpaceScene>();
		}
		if (name == "Test") {
			return std::make_shared<TestScene>();
		}
		logTrace("name not known (ignored) using default scene " + name);
		return std::make_shared<GenericScene>();
	}

	// return a possibly modifed version such as camera moved
	shared_ptr<Camera> Director::pickem(vector<shared_ptr<Camera>>&cameras, bool orbiting) {
		if (cameras.size() == 0) {
			return nullptr;
		}
		// maybe build this out using the color count techqiue (make that a base class?)
		for (int i = 0; i < cameras.size(); ++i) {
			if (orbiting) {
				if (cameras[i]->isOrbiting()) {
					return cameras[i];
				}
			}
			else {
				if (!cameras[i]->isOrbiting()) {
					//bugbug return only fixed cameras
					return cameras[i];// stuff data in the Camera class
					// lots to do here to make things nice, learn and see how to bring this in
					//void lookAt(const ofNode& lookAtNode, const ofVec3f& upVector = ofVec3f(0, 1, 0));
					//if (j == 1) {
						//cameras[j].move(-100,0,0);
						//cameras[j].pan(5);
					//}
					//else {
						//cameras[j].move(100, 0, 0);
					//}
					//bugbug just one thing we can direct http://halfdanj.github.io/ofDocGenerator/ofEasyCam.html#CameraSettings
					//float f = cameras[j].getDistance();
					//figure all this out, with times, animation etc:
					//cameras[j].setDistance(cameras[j].getDistance() + ofRandom(-100,100));
					//return &cameras[j];
				}
			}
		}
		logErrorString("no camera");
		return nullptr;
	}
	void Stage::draw() {

		//bugbug code in the data driven background object
		if (backgroundImageName.size() > 0) {
			imageForBackground.draw(0, 0);
		}
		else {
			ofSetBackgroundColor(ofColor::white);
		}

		if (drawIn2d()) {
			ofPushStyle();
			draw2d();
			ofPopStyle();
		}
		if (drawIn3dMoving() || drawIn3dFixed()) {
			ofPushStyle();
			draw3d();
			ofPopStyle();
		}
	}
	// pause them all
	void Stage::pause() {
		for (auto& a : animatables) {
			a->getDefaultPlayer()->getAnimationHelper()->pause();
		}
		//bugbug pause moving camera, grabber etc
		myPause();
	}
	void Stage::resume() {
		for (auto& a : animatables) {
			a->getDefaultPlayer()->getAnimationHelper()->resume();
		}
		//bugbug pause moving camera, grabber etc
		myResume();
	}
	// clear objects
	void Stage::clear(bool force) {
		if (force) {
			animatables.clear();
			cameras.clear();
			lights.clear();
		}
		else {
			removeExpiredItems(animatables);
			removeExpiredItems(cameras);
			removeExpiredItems(lights);
		}
		myClear(force);
	}

	void Stage::setup() {
		
		if (backgroundImageName.size() > 0) {
			imageForBackground.load(backgroundImageName);
		}
		for (auto& a : animatables) {
			a->getDefaultPlayer()->setupForDrawing();
		}

		material.setShininess(90);
		material.setSpecularColor(ofColor::olive);

		mySetup();
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);
	}
	void Stage::update() {
		for (auto& a : animatables) {
			a->getDefaultPlayer()->updateForDrawing();
		}

		if (backgroundImageName.size() > 0) {
			imageForBackground.resize(ofGetWidth(), ofGetHeight());
		}

		myUpdate();

	}
	// setup light and material for drawing
	void Stage::installLightAndMaterialThenDraw(shared_ptr<Camera>cam, bool drawFixed) {
		if (cam != nullptr) {
			cam->getPlayer().begin();
			cam->orbit(); 
			for (auto& light : lights) {
				ofPoint p = light->getPlayer().getPosition();
				light->getPlayer().setPosition(light->loc);
				light->getPlayer().enable();
				light->getPlayer().draw();
			}
			material.begin();//bugbug figure out material
			if (cam->isOrbiting()) {
				if (drawIn3dMoving()&& !drawFixed) {
					draw3dMoving();
				}
			}
			else {
				if (drawIn3dFixed() && drawFixed) {
					draw3dFixed();
				}
			}
			cam->getPlayer().end();
		}
		else {
			// draw w/o a camera
			ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2); // center when not using a camera
			for (auto& light : lights) {
				light->getPlayer().setPosition(0, 0, 600);
				light->getPlayer().enable();
				light->getPlayer().draw();
			}
			if (drawIn3dMoving() && !drawFixed) {
				draw3dMoving();
			}
			if (drawIn3dFixed() && drawFixed) {
				draw3dFixed();
			}
		}
	}
	void Stage::pre3dDraw() {
		//ofBackground(ofColor::blue); // white enables all colors in pictures/videos
		ofSetSmoothLighting(true);
		ofDisableAlphaBlending();
		ofEnableDepthTest();
	}
	void Stage::post3dDraw() {
		// clean up
		ofDisableDepthTest();
		material.end();
		for (auto& light : lights) {
			light->getPlayer().disable();
		}
		ofDisableLighting();
	}
	void Stage::draw3d() {

		pre3dDraw();

		// fixed camera
		if (drawIn3dFixed()) {
			shared_ptr<Camera> cam = director.pickem(cameras, false);
			if (cam != nullptr) {
				installLightAndMaterialThenDraw(cam, true);
			}
		}
		if (drawIn3dMoving()) {
			shared_ptr<Camera> cam = director.pickem(cameras, true);
			if (cam != nullptr) {
				installLightAndMaterialThenDraw(cam, false);
			}
		}

		post3dDraw();
	}
	// find overall duration of a scene
	float Stage::findMaxWait() {
		float f = 0;
		
		for (const auto& a : getAnimatables()) {
			setIfGreater(f, a->getDefaultPlayer()->getAnimationHelper()->getObjectLifetime() + a->getDefaultPlayer()->getAnimationHelper()->getWait());
		}

		return f;
	}
	// show light location, great for debugging and kind of fun too
	void Stage::drawlights() {
		for (const auto& light : getLights()) {
			ofSetColor(light->getPlayer().getDiffuseColor());
			ofPoint pos = light->getPlayer().getPosition();
			ofDrawSphere(light->getPlayer().getPosition(), 20.f);
		}
	}
	void Stage::draw2d() {
		myDraw2d();
		for (auto& a : animatables) {
			a->getDefaultPlayer()->drawIt(DrawingBasics::draw2d);
		}

		//ofBackground(ofColor::black);
		//bugbug option is to add vs replace:ofEnableBlendMode(OF_BLENDMODE_ADD);//bugbug can make these attributes somewhere
		//ofEnableAlphaBlending();
	}
	bool GenericScene::myCreate(const Json::Value &data) {
		READSTRING(keyname, data);
		settings.readFromScript(data);
		// read needed common types

		createTimeLineItems<Video>(settings, getAnimatables(), data, "videos");
		createTimeLineItems<Audio>(settings, getAnimatables(), data, "audios");
		createTimeLineItems<Paragraph>(settings, getAnimatables(), data, "paragraphs");
		createTimeLineItems<Picture>(settings, getAnimatables(), data, "images");
		createTimeLineItems<Text>(settings, getAnimatables(), data, "texts");
		createTimeLineItems<Sphere>(settings, getAnimatables(), data, "spheres");
		return true;
	}
	//great animation example
	bool TestBallScene::myCreate(const Json::Value &data) {
		try {
		
			shared_ptr<Ball> b = std::make_shared<Ball>();
			b->readFromScript(data);

			addAnimatable(b);
			
			return true;
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}
	}
	// juse need to draw the SpaceScene, base class does the rest
	void Stage::draw3dMoving() {
		myDraw3dMoving();
		for (auto& a : animatables) {
			a->getDefaultPlayer()->drawIt(DrawingBasics::draw3dMovingCamera);
		}
	}
	void Stage::draw3dFixed() {
		myDraw3dFixed();
		for (auto& a : animatables) {
			a->getDefaultPlayer()->drawIt(DrawingBasics::draw3dFixedCamera);
		}
	}
	template<typename T> shared_ptr<T> Stage::CreateReadAndaddAnimatable(const Json::Value &data, const string&location) {
		shared_ptr<T> p = std::make_shared<T>(location);
		if (p && p->readFromScript(data)){
			addAnimatable(p);
		}
		return p;
	}
	shared_ptr<Planet> Stage::addPlanet(const string&textureName, const ofPoint& start, const Json::Value &data, const string&name) {
		shared_ptr<Planet> p = std::make_shared<Planet>(textureName);
		if (p) {
			p->getSphere().getPlayer().setPosition(start);
			p->readFromScript(data[name]);
			addAnimatable(p);
		}
		return p;
	}

	template<typename T> shared_ptr<T> Stage::CreateReadAndaddAnimatable(const Json::Value &data) {
		shared_ptr<T> p = std::make_shared<T>();
		if (p && p->readFromScript(data)) {
			addAnimatable(p);
		}
		return p;
	}
	shared_ptr<Camera> Stage::CreateReadAndaddCamera(const Json::Value &data, bool rotate) {
		shared_ptr<Camera> p = std::make_shared<Camera>();
		if (p && p->readFromScript(data)) {
			p->setOrbit(rotate);
			p->getPlayer().setPosition(0, 0, ofRandom(100,500));//bugbug clean up the rand stuff via data and more organized random
			add(p);
		}
		return p;
	}
	template<typename T>shared_ptr<T> Stage::CreateReadAndaddLight(const Json::Value &data) {
		shared_ptr<T> p = std::make_shared<T>();
		if (p && p->readFromScript(data)) {
			add(p);
		}
		return p;
	}
	shared_ptr<VideoSphere> Stage::CreateReadAndaddVideoSphere(const Json::Value &data, const string&location){

		shared_ptr<VideoSphere> vs = std::make_shared<VideoSphere>(location);
		if (vs) {
			vs->readFromScript(data);
			addAnimatable(vs->getTexture());
			addAnimatable(vs);
		}
		return vs;
	}



	// bugbug all items in test() to come from json or are this is done in Director
	bool TestScene::myCreate(const Json::Value &data) {

		CreateReadAndaddAnimatable<Video>(data["video"], "carride.mp4");
		CreateReadAndaddAnimatable<Picture>(data["picture"], "t1_0010.jpg");
		CreateReadAndaddAnimatable<Cube>(data["cube"]);
		CreateReadAndaddAnimatable<Grabber>(data["grabber"], "Logitech HD Pro Webcam C920");
		CreateReadAndaddCamera(data["cam1"]);
		CreateReadAndaddCamera(data["cam2"], true);
		CreateReadAndaddLight<PointLight>(data["pointLight1"]);
		CreateReadAndaddLight<PointLight>(data["pointLight2"]);
		CreateReadAndaddLight<DirectionalLight>(data["directionalLight"]);
		CreateReadAndaddLight<Light>(data["basicLight"]);
		CreateReadAndaddLight<Light>(data["spotLight"]);

		return true;

	}
	void TestScene::myUpdate() {
		mesh.update();
	}
	void TestScene::myDraw3dFixed() {
	
		//drawlights();

		return; // comment out to draw by vertice, less confusing but feel free to add it back in

		ofSetHexColor(0xffffff);
		glPointSize(2);
		glEnable(GL_POINT_SMOOTH);
		mesh.drawVertices();
	}
	void TestScene::mySetup() {
		ofSetSmoothLighting(true);
		mesh.setup();
	}

	bool SpaceScene::myCreate(const Json::Value &data) {

		shared_ptr<VideoSphere> vs = CreateReadAndaddVideoSphere(data["videosphere"], "Clyde.mp4");
		CreateReadAndaddLight<PointLight>(data["light1"]);
		CreateReadAndaddCamera(data["cam2"]);
		CreateReadAndaddCamera(data["cam1"], true);

		setBackgroundImageName("hubble1.jpg");//bugbug read from json

		float xStart = (vs->getPlayer().getRadius()*2) / 3;
		float offset = abs(xStart);
		addPlanet("earth_day.jpg", ofPoint(xStart, 0, offset + 100), data, "p1");

		xStart += ofRandom(xStart, xStart * 1.2); // need to keep sign
		addPlanet("g04_s65_34658.gif", ofPoint(xStart, ofRandom(0, 100), offset + 100), data, "p2");
		
		xStart += ofRandom(xStart, (vs->getPlayer().getRadius() * 2) / 2); // need to keep sign
		addPlanet("hubble1.jpg", ofPoint(xStart, ofRandom(0, 100), offset + 100), data, "p3");
		
		xStart = ofRandom(xStart, xStart * .2); // need to keep sign
		addPlanet("Floodwaters_of_Mars_highlight_std.jpg", ofPoint(xStart, ofRandom(0, 100), offset + 100), data, "p4");

		return true;
	}

}