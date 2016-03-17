#include "2552software.h"
#include "scenes.h"

//By default, the screen is cleared with every draw() call.We can change that with 
//  ofSetBackgroundAuto(...).Passing in a value of false turns off the automatic background clearing.

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
					//bugbug return only fixed cameras
					return &cameras[i];// stuff data in the Camera class
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
		width = 100;
		ofCircle((2 * ofGetFrameNum()) % ofGetWidth(), ball.val(), width);
		glColor4ub(255, 255, 255, 255);
		ofRect(0, floorLine + width, ofGetWidth(), 1);

		//vertical lines
		ofRect(xMargin, 0, 1, floorLine + width);
		ofRect(xMargin + widthCol + width, 0, 1, floorLine + width);

		return;
		ofBackground(ofColor::white); // white enables all colors in pictures/videos

		if (backgroundImageName.size() > 0) {
			imageForBackground.draw(0, 0);
		}

		ofPushStyle();
		//draw2d();
		ofPopStyle();

		ofPushStyle();
		draw3d();
		ofPopStyle();
	}

	void Stage::test() {
	}
	// setup light and material for drawing
	void Stage::installLightAndMaterialThenDraw(Camera*cam) {
		material.begin();//bugbug figure out material
		if (cam != nullptr) {
			cam->setPosition(cam->pos);
			cam->begin();
			cam->orbit(); 
			for (auto& light : lights) {
				light.setPosition(light.pos);
				light.enable();
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
			light.disable();
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

		ofBackground(ofColor::black);
		ofEnableBlendMode(OF_BLENDMODE_ADD);//bugbug can make these attributes somewhere
		ofSetColor(255, 100);//bugbug figure out alpha in color.h
		for (const auto& image : images) {
			image.draw(image.pos.x, image.pos.y, image.w, image.h);
		}
		ofSetColor(255, 125);
		for (const auto& video : videos) {
			video.draw(video.pos.x, video.pos.y, video.w, video.h);
		}
		ofSetColor(255, 175);
		for (auto& grabber : grabbers) {
			if (grabber.isInitialized()) {
				grabber.draw(grabber.pos.x, grabber.pos.y, grabber.w, grabber.h);
			}
		}
		ofEnableAlphaBlending();
	}
	void Stage::setup() {

		ball.reset(floorLine - 100);
		ball.setCurve(EASE_IN);
		ball.setRepeatType(LOOP_BACK_AND_FORTH);
		//ball.setDuration(0.55);
		ball.animateTo(floorLine);
		ball.setDuration(0.001);

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
		float dt = 1.0f / 60.0f;
		ball.update(dt);

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
		for (auto& video : texturevideos) {
			video.update();
		}
	}
	// juse need to draw the SpaceScene, base class does the rest
	void TestScene::draw3dMoving() {
	}
	void TestScene::draw2d() {
		Stage::draw2d();
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2); // move 0,0 to center
		for (int i = 0; i < 10; i++) {
			rect[i].draw();
		}
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

		// bugbug just for learning as there is no screen size adjustment in update
		for (int i = 0; i < 10; i++) {
			rect[i].posA.x = -ofGetWidth() / 2;
			rect[i].posA.y = 10 + i * 20;
			rect[i].posB.x = ofGetWidth();
			rect[i].posB.y = 10 + i * 20;
			rect[i].shaper = 0.4 + 0.2 * i;
			rect[i].updateMovement();	// start at 0 pct
			rect[i].h = 40;
			rect[i].w = 40;
		}

		Light pointLight;
		pointLight.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		pointLight.setSpecularColor(ofColor(255.f, 0, 0));
		pointLight.pos.x = ofGetWidth()*.2;
		pointLight.pos.y = ofGetHeight()*.2;
		pointLight.setPointLight();
		add(pointLight);

		Light pointLight2;
		pointLight2.setDiffuseColor(ofColor(0.f, 0, 255.f));
		// specular color, the highlight/shininess color //
		pointLight2.setSpecularColor(ofColor(255.f, 0, 0));
		pointLight2.pos.x = -ofGetWidth()*.2;
		pointLight2.pos.y = ofGetHeight()*.2;
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
		basic.pos.x = -100;
		basic.pos.z = 400;
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
		spotLight.pos.x = -ofGetWidth()*.1;
		spotLight.pos.y = ofGetHeight()*.1;
		spotLight.pos.z = 100;
		add(spotLight);
		

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

		for (int i = 0; i < 10; i++) {
			rect[i].updateMovement();  // go between pta and ptb
		}
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
	void SpaceScene::update() {
		Stage::update();
	}
	void SpaceScene::test() {
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

		//bugbug get from json
		Stage::test();
		//bugbug get from json
		videoSphere.set(250, 180);

		Light light1;
		light1.pos.x = 0;
		light1.pos.y = 0;
		light1.pos.z = videoSphere.getRadius() * 2 ;
		add(light1);


		Camera cam1; // learn camera bugbug
		cam1.setScale(-1, -1, 1); // showing video
		cam1.setOrbit(true); // rotating
		cam1.pos.z = videoSphere.getRadius() * 2 + 300;
		add(cam1);

		Camera cam2; // learn camera bugbug
		cam2.setOrbit(false); // not rotating
		cam2.setScale(-1, -1, 1); // showing video
		cam2.pos.z = videoSphere.getRadius()*2 + 100;
		cam2.setFov(60);
		add(cam2);

		TextureVideo tv;
		tv.create("Clyde.mp4", 0, 0);
		add(tv);

		setBackgroundImageName("hubble1.jpg");

		float xStart = (ofGetWidth() - tv.getWidth()) / 2;
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
		///next draw video in fbo (put in video class) http://clab.concordia.ca/?page_id=944

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
		if (getTextureVideos()[0].textureReady()) {
			videoSphere.mapTexCoordsFromTexture(getTextureVideos()[0].getTexture());
		}
		getTextureVideos()[0].bind();
		videoSphere.draw();
		getTextureVideos()[0].unbind();
	}
}