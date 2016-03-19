#include "2552software.h"
#include "scenes.h"

//By default, the screen is cleared with every draw() call.We can change that with 
//  ofSetBackgroundAuto(...).Passing in a value of false turns off the automatic background clearing.

namespace Software2552 {

	// return a possibly modifed version such as camera moved
	shared_ptr<Camera> Director::pickem(vector<shared_ptr<Camera>>&cameras, bool orbiting) {
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
		for (auto& a : animatables) {
			a->drawIt();
		}

		if (backgroundImageName.size() > 0) {
			imageForBackground.draw(0, 0);
		}

		ofPushStyle();
		//ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2); // move 0,0 to center
		draw2d();
		ofPopStyle();

		ofPushStyle();
		//draw3d();
		ofPopStyle();
	}
	void Stage::setup() {
		test();
		if (backgroundImageName.size() > 0) {
			imageForBackground.load(backgroundImageName);
		}
		for (auto& a : animatables) {
			a->loadForDrawing();
		}

		material.setShininess(90);
		material.setSpecularColor(ofColor::olive);
		//material.setColors(ofFloatColor::pink, ofFloatColor::green, ofFloatColor::orange, ofFloatColor::aliceBlue);
	}
	void Stage::update() {

		for (auto& a : animatables) {
			a->updateForDrawing();
		}

		if (backgroundImageName.size() > 0) {
			imageForBackground.resize(ofGetWidth(), ofGetHeight());
		}

	}
	//great animation example
	void Stage::test() {
		shared_ptr<Ball2d> b = std::make_shared<Ball2d>();
		b->setPositionY(b->floorLine - 100);
		b->setCurve(EASE_IN);
		b->setRepeatType(LOOP_BACK_AND_FORTH);
		b->setDuration(0.55);
		ofPoint p;
		p.y = b->floorLine;
		b->animateTo(p);
		//b->setDuration(0.001);

		shared_ptr<ColorAnimation> c = std::make_shared<ColorAnimation>();

		c->setColor(ofColor::blue);
		c->setDuration(0.5f);
		c->setRepeatType(LOOP_BACK_AND_FORTH);
		c->setCurve(LINEAR);
		c->animateTo(ofColor::red);
		b->setColorAnimation(c);

		animatables.push_back(b);


	}
	// setup light and material for drawing
	void Stage::installLightAndMaterialThenDraw(shared_ptr<Camera>cam) {
		material.begin();//bugbug figure out material
		if (cam != nullptr) {
			cam->setAnimationPosition(cam->getCurrentPosition());
			cam->begin();
			cam->orbit(); 
			for (auto& light : lights) {
				light->setAnimationPosition(light->getCurrentPosition());
				light->enable();
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
			light->disable();
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
	void Stage::draw2d() {
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2); // move 0,0 to center
		for (auto& a : animatables) {
			a->drawIt();
		}
		
		//ofBackground(ofColor::black);
		//bugbug option is to add vs replace:ofEnableBlendMode(OF_BLENDMODE_ADD);//bugbug can make these attributes somewhere
		//ofEnableAlphaBlending();
	}
	// juse need to draw the SpaceScene, base class does the rest
	void TestScene::draw3dMoving() {
	}
	void TestScene::draw2d() {
		Stage::draw2d();
		
		
		//for (int i = 0; i < 10; i++) {
			//rect[i].draw();
		//}
		VectorPattern p;
		//p.stripe(true);
		//p.triangle(true);
		//p.shape(15, 5, false, true, 5);
		//p.matrix(10, 20);
	}
	// all items in test() to come from json
	void TestScene::test() {
		Stage::test();
		

		shared_ptr<Camera> cam1 = std::make_shared<Camera>();
		add(cam1);

		shared_ptr<Light> pointLight = std::make_shared<Light>();
		pointLight->setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		pointLight->setSpecularColor(ofColor(255.f, 0, 0));
		pointLight->setPositionX(ofGetWidth()*.2);
		pointLight->setPositionY(ofGetHeight()*.2);
		pointLight->setPointLight();
		add(pointLight);

		shared_ptr<Light> pointLight2 = std::make_shared<Light>();
		pointLight2->setDiffuseColor(ofColor(0.f, 0, 255.f));
		// specular color, the highlight/shininess color //
		pointLight2->setSpecularColor(ofColor(255.f, 0, 0));
		pointLight2->setPositionX(-ofGetWidth()*.2);
		pointLight2->setPositionY(ofGetHeight()*.2);
		pointLight2->setPointLight();
		add(pointLight2);

		shared_ptr<Light> directionalLight = std::make_shared<Light>();
		// Directional Lights emit light based on their orientation, regardless of their position //
		directionalLight->setDiffuseColor(ofColor(0.f, 0.f, 255.f));
		directionalLight->setSpecularColor(ofColor(255.f, 255.f, 255.f));
		directionalLight->setDirectional();

		// set the direction of the light
		// set it pointing from left to right -> //
		directionalLight->setOrientation(ofVec3f(0, 90, 0));
		add(directionalLight);

		shared_ptr<Light> basic = std::make_shared<Light>();
		basic->setPositionX(-100.0f);
		basic->setPositionZ(400.0f);
		add(basic);

		shared_ptr<Light> spotLight = std::make_shared<Light>();
		spotLight->setDiffuseColor(ofColor(255.f, 0.f, 0.f));
		spotLight->setSpecularColor(ofColor(255.f, 255.f, 255.f));

		// turn the light into spotLight, emit a cone of light //
		spotLight->setSpotlight();

		// size of the cone of emitted light, angle between light axis and side of cone //
		// angle range between 0 - 90 in degrees //
		spotLight->setSpotlightCutOff(50);

		// rate of falloff, illumitation decreases as the angle from the cone axis increases //
		// range 0 - 128, zero is even illumination, 128 is max falloff //
		spotLight->setSpotConcentration(2);
		ofPoint pos(-ofGetWidth()*.1, ofGetHeight()*.1, 100);
		spotLight->setAnimationPosition(pos);
		add(spotLight);
		

		shared_ptr<RoleRaster> raster = std::make_shared<RoleRaster>("t1_0010.jpg");
		//raster.w = ofGetWidth() / 3;
		addAnimatable(raster);

		shared_ptr<RoleVideo> video = std::make_shared<RoleVideo>("carride.mp4");
		//video.w = ofGetWidth() / 3;
		//video.x = raster.w;
		addAnimatable(video);

		shared_ptr<Grabber> grabber = std::make_shared<Grabber>("Logitech HD Pro Webcam C920");
		//grabber.w = ofGetWidth() / 3;
		//grabber.x = video.x + video.w;
		addAnimatable(grabber);


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
			ofSetColor(light->getDiffuseColor());
			ofDrawSphere(light->getPosition(), 20.f);
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
	void SpaceScene::update() {
		Stage::update();
	}
	void SpaceScene::test() {
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

		//bugbug get from json
		Stage::test();
		//bugbug get from json
		videoSphere.set(250, 180);

		shared_ptr<Light> light1 = std::make_shared<Light>();
		ofPoint point(0,0, videoSphere.getRadius() * 2);
		light1->setAnimationPosition(point);
		add(light1);


		shared_ptr<Camera> cam1 = std::make_shared<Camera>();
		cam1->setScale(-1, -1, 1); // showing video
		cam1->setOrbit(true); // rotating
		cam1->setPositionZ(videoSphere.getRadius() * 2 + 300);
		add(cam1);

		shared_ptr<Camera> cam2 = std::make_shared<Camera>();
		cam2->setOrbit(false); // not rotating
		cam2->setScale(-1, -1, 1); // showing video
		cam2->setPositionZ(videoSphere.getRadius()*2 + 100);
		cam2->setFov(60);
		add(cam2);

		shared_ptr<TextureVideo> tv = std::make_shared<TextureVideo>();
		tv->create("Clyde.mp4", 0, 0);
		add(tv);

		setBackgroundImageName("hubble1.jpg");

		float xStart = (ofGetWidth() - tv->getWidth()) / 2;
		float offset = abs(xStart);
		addPlanet("earth_day.jpg", ofVec3f(xStart, 0, offset + 100));

		xStart += ofRandom(xStart, xStart * 2); // need to keep sign
		addPlanet("g04_s65_34658.gif", ofVec3f(xStart, ofRandom(0, 100), offset + 100));
		
		xStart += ofRandom(xStart, xStart * 2); // need to keep sign
		addPlanet("hubble1.jpg", ofVec3f(xStart, ofRandom(0, 100), offset + 100));
		
		xStart += ofRandom(xStart, xStart * 2); // need to keep sign
		addPlanet("Floodwaters_of_Mars_highlight_std.jpg", ofVec3f(xStart, ofRandom(0, 100), offset + 100));

	}

	void SpaceScene::addPlanet(const string&textureName, const ofVec3f& start) {
		shared_ptr<Planet> p = std::make_shared<Planet>();

		float r = ofRandom(5, 100);
		p->set(r, 40);
		TextureFromImage texture;
		texture.create(textureName, r * 2, r * 2);
		p->mapTexCoordsFromTexture(texture);
		p->texture = texture;
		p->setWireframe(false);
		p->move(start);
		pictureSpheres.push_back(p);
	}
	void SpaceScene::setup() {
		test();//bugbug set via script 
		Stage::setup();
		///next draw video in fbo (put in video class) http://clab.concordia.ca/?page_id=944

	}
	void SpaceScene::draw2d() {
	}
	// juse need to draw the SpaceScene, base class does the rest
	void SpaceScene::draw3dMoving() {
		for (auto& pictureSphere : pictureSpheres) {
			pictureSphere->texture.bind();
			pictureSphere->rotate(30, 0, 2.0, 0.0);
			pictureSphere->draw();
			pictureSphere->texture.unbind();
		}
	}
	void SpaceScene::draw3dFixed() {
		// one time setup must be called to draw videoSphere
		if (getTextureVideos()[0]->textureReady()) {
			videoSphere.mapTexCoordsFromTexture(getTextureVideos()[0]->getTexture());
		}
		getTextureVideos()[0]->bind();
		videoSphere.draw();
		getTextureVideos()[0]->unbind();
	}
}