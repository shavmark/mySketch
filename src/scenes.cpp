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
		if (drawIn3d()) {
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
		//bugbug pause moving camera
		myPause();
	}
	void Stage::resume() {
		for (auto& a : animatables) {
			a->getDefaultPlayer()->getAnimationHelper()->resume();
		}
		//bugbug pause moving camera
		myResume();
	}
	// clear objects
	void Stage::clear(bool force) {
		if (force) {
			animatables.clear();
			cameras.clear();
			lights.clear();
			texturevideos.clear();
			grabbers.clear();
		}
		else {
			removeExpiredItems(animatables);
			removeExpiredItems(cameras);
			removeExpiredItems(lights);
			removeExpiredItems(texturevideos);
			removeExpiredItems(grabbers);
		}
		myClear(force);
	}

	void Stage::setup() {
		
		if (backgroundImageName.size() > 0) {
			imageForBackground.load(backgroundImageName);
		}
		for (auto& a : animatables) {
			a->getDefaultPlayer()->loadForDrawing();
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
	void Stage::installLightAndMaterialThenDraw(shared_ptr<Camera>cam) {
		if (cam != nullptr) {
			material.begin();//bugbug figure out material
			cam->getAnimationHelper()->setPosition(cam->getAnimationHelper()->getCurrentPosition());
			cam->begin();
			cam->orbit(); 
			for (auto& light : lights) {
				light->getAnimationHelper()->setPosition(light->getAnimationHelper()->getCurrentPosition());
				light->player.enable();
			}
			if (cam->isOrbiting()) {
				draw3dMoving();
			}
			else {
				draw3dFixed();
			}
			cam->end();
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
		material.end();
		ofDisableDepthTest();
		for (auto& light : lights) {
			light->player.disable();
		}
		ofDisableLighting();
	}
	void Stage::draw3d() {
		// setup bugbug consider a virtual setup/cleanup function so dervied class can change this around
		pre3dDraw();

		installLightAndMaterialThenDraw(director.pickem(cameras, false));
		installLightAndMaterialThenDraw(director.pickem(cameras, true));

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

	void Stage::draw2d() {
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
			b->getPlayer()->getAnimationHelper()->setPositionY(b->getPlayer()->floorLine - 100);
			b->getPlayer()->getAnimationHelper()->setCurve(EASE_IN);
			b->getPlayer()->getAnimationHelper()->setRepeatType(LOOP_BACK_AND_FORTH);
			b->getPlayer()->getAnimationHelper()->setDuration(0.55);
			readJsonValue(b->getPlayer()->radius, data["radius"]);
			ofPoint p;
			p.y = b->getPlayer()->floorLine;
			b->getPlayer()->getAnimationHelper()->animateTo(p);
			//b->setDuration(0.001);

			shared_ptr<ColorAnimation> c = std::make_shared<ColorAnimation>();
			c->setColor(Colors::getFirstColors(settings.getColor().getGroup())->getOfColor(Colors::foreColor));
			c->setDuration(0.5f);
			c->setRepeatType(LOOP_BACK_AND_FORTH);
			c->setCurve(LINEAR);
			c->animateTo(Colors::getLastColors(settings.getColor().getGroup())->getOfColor(Colors::foreColor));
			b->getPlayer()->setColorAnimation(c);

			addAnimatable(b);
			// read needed common types
			return true;

		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}
	}
	// juse need to draw the SpaceScene, base class does the rest
	void Stage::draw3dMoving() {
		for (auto& a : animatables) {
			a->getDefaultPlayer()->drawIt(DrawingBasics::draw3dMovingCamera);
		}
	}
	void Stage::draw3dFixed() {
		for (auto& a : animatables) {
			a->getDefaultPlayer()->drawIt(DrawingBasics::draw3dFixedCamera);
		}
	}
	// bugbug all items in test() to come from json or are this is done in Director
	bool TestScene::myCreate(const Json::Value &data) {

		shared_ptr<Camera> cam1 = std::make_shared<Camera>();
		add(cam1);

		shared_ptr<Light> pointLight = std::make_shared<Light>();
		pointLight->player.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		pointLight->player.setSpecularColor(ofColor(255.f, 0, 0));
		pointLight->getAnimationHelper()->setPositionX(ofGetWidth()*.2);
		pointLight->getAnimationHelper()->setPositionY(ofGetHeight()*.2);
		pointLight->player.setPointLight();
		add(pointLight);

		shared_ptr<Light> pointLight2 = std::make_shared<Light>();
		pointLight2->player.setDiffuseColor(ofColor(0.f, 0, 255.f));
		// specular color, the highlight/shininess color //
		pointLight2->player.setSpecularColor(ofColor(255.f, 0, 0));
		pointLight2->getAnimationHelper()->setPositionX(-ofGetWidth()*.2);
		pointLight2->getAnimationHelper()->setPositionY(ofGetHeight()*.2);
		pointLight2->player.setPointLight();
		add(pointLight2);

		shared_ptr<Light> directionalLight = std::make_shared<Light>();
		// Directional Lights emit light based on their orientation, regardless of their position //
		directionalLight->player.setDiffuseColor(ofColor(0.f, 0.f, 255.f));
		directionalLight->player.setSpecularColor(ofColor(255.f, 255.f, 255.f));
		directionalLight->player.setDirectional();

		// set the direction of the light
		// set it pointing from left to right -> //
		directionalLight->player.setOrientation(ofVec3f(0, 90, 0));
		add(directionalLight);

		shared_ptr<Light> basic = std::make_shared<Light>();
		basic->getAnimationHelper()->setPositionX(-100.0f);
		basic->getAnimationHelper()->setPositionZ(400.0f);
		add(basic);

		shared_ptr<Light> spotLight = std::make_shared<Light>();
		spotLight->player.setDiffuseColor(ofColor(255.f, 0.f, 0.f));
		spotLight->player.setSpecularColor(ofColor(255.f, 255.f, 255.f));

		// turn the light into spotLight, emit a cone of light //
		spotLight->player.setSpotlight();

		// size of the cone of emitted light, angle between light axis and side of cone //
		// angle range between 0 - 90 in degrees //
		spotLight->player.setSpotlightCutOff(50);

		// rate of falloff, illumitation decreases as the angle from the cone axis increases //
		// range 0 - 128, zero is even illumination, 128 is max falloff //
		spotLight->player.setSpotConcentration(2);
		ofPoint pos(-ofGetWidth()*.1, ofGetHeight()*.1, 100);
		spotLight->getAnimationHelper()->setPosition(pos);
		add(spotLight);

		shared_ptr<Grabber> grabber = std::make_shared<Grabber>("Logitech HD Pro Webcam C920");
		//grabber.w = ofGetWidth() / 3;
		//grabber.x = video.x + video.w;
		add(grabber);


		shared_ptr<Picture> raster = std::make_shared<Picture>("t1_0010.jpg");
		//raster.w = ofGetWidth() / 3;
		raster->getDefaultPlayer()->setType(DrawingBasics::draw3dMovingCamera);
		addAnimatable(raster);

		shared_ptr<Video> video = std::make_shared<Video>("carride.mp4");
		video->getDefaultPlayer()->setType(DrawingBasics::draw3dFixedCamera);
		//video.w = ofGetWidth() / 3;
		//video.x = raster.w;
		addAnimatable(video);
		return true;

	}
	void TestScene::myUpdate() {

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
			ofSetColor(light->player.getDiffuseColor());
			ofDrawSphere(light->player.getPosition(), 20.f);
		}

		cube.player.draw();

		return; // comment out to draw by vertice

		ofSetHexColor(0xffffff);
		glPointSize(2);
		glEnable(GL_POINT_SMOOTH);
		mesh.drawVertices();
	}
	void TestScene::mySetup() {
		ofSetSmoothLighting(true);
		mesh.setup();
		cube.setWireframe(false);
		cube.player.set(100);
	}
	void SpaceScene::myUpdate() {
	}
	bool SpaceScene::myCreate(const Json::Value &data) {
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

		//bugbug get from json
		videoSphere.player.set(250, 180);

		shared_ptr<Light> light1 = std::make_shared<Light>();
		ofPoint point(0,0, videoSphere.player.getRadius() * 2);
		light1->getAnimationHelper()->setPosition(point);
		add(light1);


		shared_ptr<Camera> cam1 = std::make_shared<Camera>();
		cam1->setScale(-1, -1, 1); // showing video
		cam1->setOrbit(true); // rotating
		cam1->getAnimationHelper()->setPositionZ(videoSphere.player.getRadius() * 2 + 300);
		add(cam1);

		shared_ptr<Camera> cam2 = std::make_shared<Camera>();
		cam2->setOrbit(false); // not rotating
		cam2->setScale(-1, -1, 1); // showing video
		cam2->getAnimationHelper()->setPositionZ(videoSphere.player.getRadius()*2 + 100);
		cam2->setFov(60);
		add(cam2);

		shared_ptr<TextureVideo> tv = std::make_shared<TextureVideo>();
		tv->create("Clyde.mp4", 0, 0);
		add(tv);

		setBackgroundImageName("hubble1.jpg");

		float xStart = (ofGetWidth() - tv->player.getWidth()) / 2;
		float offset = abs(xStart);
		addPlanet("earth_day.jpg", ofVec3f(xStart, 0, offset + 100));

		xStart += ofRandom(xStart, xStart * 2); // need to keep sign
		addPlanet("g04_s65_34658.gif", ofVec3f(xStart, ofRandom(0, 100), offset + 100));
		
		xStart += ofRandom(xStart, xStart * 2); // need to keep sign
		addPlanet("hubble1.jpg", ofVec3f(xStart, ofRandom(0, 100), offset + 100));
		
		xStart += ofRandom(xStart, xStart * 2); // need to keep sign
		addPlanet("Floodwaters_of_Mars_highlight_std.jpg", ofVec3f(xStart, ofRandom(0, 100), offset + 100));
		return true;

	}

	void SpaceScene::addPlanet(const string&textureName, const ofVec3f& start) {
		shared_ptr<Planet> p = std::make_shared<Planet>();

		float r = ofRandom(5, 100);
		p->player.set(r, 40);
		TextureFromImage texture;
		texture.create(textureName, r * 2, r * 2);
		p->player.mapTexCoordsFromTexture(texture);
		p->texture = texture;
		p->setWireframe(false);
		p->player.move(start);
		pictureSpheres.push_back(p);
	}
	void SpaceScene::mySetup() {

	}
	void SpaceScene::draw2d() {
	}
	// juse need to draw the SpaceScene, base class does the rest
	void SpaceScene::draw3dMoving() {
		for (auto& pictureSphere : pictureSpheres) {
			pictureSphere->texture.bind();
			pictureSphere->player.rotate(30, 0, 2.0, 0.0);
			pictureSphere->player.draw();
			pictureSphere->texture.unbind();
		}
	}
	void SpaceScene::draw3dFixed() {
		// one time setup must be called to draw videoSphere
		if (getTextureVideos()[0]->textureReady()) {
			videoSphere.player.mapTexCoordsFromTexture(getTextureVideos()[0]->player.getTexture());
		}
		getTextureVideos()[0]->bind();
		videoSphere.player.draw();
		getTextureVideos()[0]->unbind();
	}
}