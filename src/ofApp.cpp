#include "ofApp.h"
#include "2552software.h"

// follow this https://github.com/openframeworks/openFrameworks/wiki/oF-code-style, but let them w/o bugs cast the first flames

//http://openframeworks.cc/ofBook/chapters/c++11.html\

//http://openframeworks.cc/tutorials/graphics/generativemesh.html

//https://github.com/Kinect/Docs

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);

	pathLines.setMode(OF_PRIMITIVE_LINE_STRIP);
	ofDisableArbTex(); // we need GL_TEXTURE_2D for our models coords.
	ofEnableDepthTest();
	light.enable();
	cam.setPosition(0, 0, 100);

	headTrackedCamera.setNearClip(0.01f);
	headTrackedCamera.setFarClip(1000.0f);

	//defining the real world coordinates of the window which is being headtracked is important for visual accuracy
	windowWidth = 0.3f;
	windowHeight = 0.2f;

	windowTopLeft = ofVec3f(-windowWidth / 2.0f,
		+windowHeight / 2.0f,
		0.0f);
	windowBottomLeft = ofVec3f(-windowWidth / 2.0f,
		-windowHeight / 2.0f,
		0.0f);
	windowBottomRight = ofVec3f(+windowWidth / 2.0f,
		-windowHeight / 2.0f,
		0.0f);

	//we use this constant since we're using a really hacky headtracking in this example
	//if you use something that can properly locate the head in 3d (like a kinect), you don't need this fudge factor
	viewerDistance = 0.4f;

	model.setup("C:\\Users\\mark\\Documents\\iclone\\heidi\\heidi.fbx");
	model2.setup("C:\\Users\\mark\\Documents\\iclone\\reggie\\reggie_Catwalk.fbx");

	model.setPlay(aiString("AnimStack::Dance graceful"));
	//model2.setPlay(aiString("AnimStack::Catwalk"));

	//From2552Software::Sound sound;
	//sound.test();

	//myKinect.open();
	//audio.setup(&myKinect);
	//audio.setup(&myKinect);
	//audio.setup(&myKinect);
	
	//faces.setup(&myKinect);
	//bodies.useFaces();
	//bodies.setup(&myKinect);

	//ofSetWindowShape(640 * 2, 480 * 2);

}


//--------------------------------------------------------------
void ofApp::update(){

	ofVec3f headPosition(0, 0, viewerDistance);
	
#if 0
	if (finder.blobs.size() > 0) {
		//get the head position in camera pixel coordinates
		float cameraHeadX = blob.centroid.x;
		float cameraHeadY = blob.centroid.y;

		//do a really hacky interpretation of this, really you should be using something better to find the head (e.g. kinect skeleton tracking)

		//since camera isn't mirrored, high x in camera means -ve x in world
		float worldHeadX = ofMap(cameraHeadX, 0, video.getWidth(), windowBottomRight.x, windowBottomLeft.x);

		//low y in camera is +ve y in world
		float worldHeadY = ofMap(cameraHeadY, 0, video.getHeight(), windowTopLeft.y, windowBottomLeft.y);

		//set position in a pretty arbitrary way
		headPosition = ofVec3f(worldHeadX, worldHeadY, viewerDistance);
	}
	else {
		if (!video.isInitialized()) {
			//if video isn't working, just make something up
			headPosition = ofVec3f(0.5f * windowWidth * sin(ofGetElapsedTimef()), 0.5f * windowHeight * cos(ofGetElapsedTimef()), viewerDistance);
		}
	}

#endif // 0

	//if video isn't working, just make something up REPLACE with Kinect
	headPosition = ofVec3f(0.5f * windowWidth * sin(ofGetElapsedTimef()), 0.5f * windowHeight * cos(ofGetElapsedTimef()), viewerDistance);
	headPositionHistory.push_back(headPosition);
	while (headPositionHistory.size() > 50.0f) {
		headPositionHistory.pop_front();
	}

	//these 2 lines of code must be called every time the head position changes
	headTrackedCamera.setPosition(headPosition);
	headTrackedCamera.setupOffAxisViewPortal(windowTopLeft, windowBottomLeft, windowBottomRight);

	previous = current;

	// generate a noisy 3d position over time 
	float t = (2 + ofGetElapsedTimef()) * .1;
	current.x = ofSignedNoise(t, 0, 0);
	current.y = ofSignedNoise(0, t, 0);
	current.z = ofSignedNoise(0, 0, t);
	current *= 400; // scale from -1,+1 range to -400,+400

					// add the current position to the pathVertices deque
	pathVertices.push_back(current);
	// if we have too many vertices in the deque, get rid of the oldest ones
	while (pathVertices.size() > 200) {
		pathVertices.pop_front();
	}

	// clear the pathLines ofMesh from any old vertices
	pathLines.clear();
	// add all the vertices from pathVertices
	for (unsigned int i = 0; i < pathVertices.size(); i++) {
		pathLines.addVertex(pathVertices[i]);
	}	// here's where the transformation happens, using the orbit and roll member functions of the ofNode class, 
	// since angleH and distance are initialised to 0 and 500, we start up as how we want it

	model.update();
	model2.update();
	//faces.baseline(); //use to debug, can do what ever needed to get things to work, to create a working base line
	//faces.update();
	//bodies.update();
	//audio.update();
	//audio.update();
#if sample	
	//--
	//Getting joint positions (skeleton tracking)
	//--
	//
	{
		auto bodies = kinect.getBodySource()->getBodies();
		for (auto body : bodies) {
			for (auto joint : body.joints) {
				//now do something with the joints
			}
		}
	}
	//
	//--



	//--
	//Getting bones (connected joints)
	//--
	//
	{
		// Note that for this we need a reference of which joints are connected to each other.
		// We call this the 'boneAtlas', and you can ask for a reference to this atlas whenever you like
		auto bodies = kinect.getBodySource()->getBodies();
		auto boneAtlas = ofxKinectForWindows2::Data::Body::getBonesAtlas();

		for (auto body : bodies) {
			for (auto bone : boneAtlas) {
				auto firstJointInBone = body.joints[bone.first];
				auto secondJointInBone = body.joints[bone.second];

				//now do something with the joints
			}
		}
	}
	//
	//--
#endif
}
void ofApp::drawScene() {

	ofPushStyle();
	ofNoFill();
	ofColor col(200, 100, 100);
	for (float z = 0.0f; z > -40.0f; z -= 0.1f) {
		col.setHue(int(-z * 100.0f + ofGetElapsedTimef() * 10.0f) % 360);
		ofSetColor(col);
		ofDrawRectangle(-windowWidth / 2.0f, -windowHeight / 2.0f, z, windowWidth, windowHeight);
	}
	ofPopStyle();

	ofPushStyle();
	ofEnableSmoothing();
	ofSetColor(255);
	ofSetLineWidth(5.0f);
	ofBeginShape();
	for (unsigned int i = 0; i<headPositionHistory.size(); i++) {
		ofPoint vertex(headPositionHistory[i].x, headPositionHistory[i].y, -float(headPositionHistory.size() - i) * 0.05f);
		ofCurveVertex(vertex);
	}
	ofEndShape(false);
	ofPopStyle();

}
//--------------------------------------------------------------
void ofApp::draw(){

	headTrackedCamera.begin();
	//drawScene();
	headTrackedCamera.end();

	//
	//------
	ofColor cyan = ofColor::fromHex(0x00abec);
	ofColor magenta = ofColor::fromHex(0xec008c);
	ofColor yellow = ofColor::fromHex(0xffee00);

	ofBackgroundGradient(magenta * .6, magenta * .4);
	ofNoFill();

	easyCam.begin();
	ofRotateX(15);

	ofSetColor(0);
	ofDrawGrid(500, 10, false, false, true, false);

	// draw the path of the box
	ofSetLineWidth(2);
	ofSetColor(cyan);
	pathLines.draw();

	// draw a line connecting the box to the grid
	ofSetColor(yellow);
	ofDrawLine(current.x, current.y, current.z, current.x, 0, current.z);

	// translate and rotate to the current position and orientation
	ofTranslate(current.x, current.y, current.z);
	if ((current - previous).length() > 0.0) {
		From2552Software::Graphics2552::rotateToNormal(current - previous);
	}
	ofSetColor(255);
	ofDrawBox(32);
	ofDrawAxis(32);
	ofFill();
	model.draw();
	//model.drawMesh();
	easyCam.end();
	return;

	material.begin();

	ofNode node= camera.getTarget();

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofEnableDepthTest();

#ifndef TARGET_PROGRAMMABLE_GL    
	glShadeModel(GL_SMOOTH); //some model / light stuff
#endif
	
	//ofEnableSeparateSpecularLight();
	ofPushMatrix();
	//ofRotateZ(180);
	model.draw();
	model2.draw();
#ifndef TARGET_PROGRAMMABLE_GL    
	glEnable(GL_NORMALIZE);
#endif
	model.drawMesh();
	model2.drawMesh();
	ofPopMatrix();


	
	/*
	kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark

																	   // Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio

	kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
	kinect.getBodySource()->drawProjected(previewWidth, 0 + colorTop, previewWidth, colorHeight);

	kinect.getInfraredSource()->draw(0, previewHeight, previewWidth, previewHeight);

	kinect.getBodyIndexSource()->draw(previewWidth, previewHeight, previewWidth, previewHeight);
	kinect.getBodySource()->drawProjected(previewWidth, previewHeight, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);
	*/
	//float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
	//float colorTop = (previewHeight - colorHeight) / 2.0;
	//kinect.getBodySource()->drawProjected(previewWidth, 0 + colorTop, previewWidth, colorHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);
	//faces.drawProjected(kinect.getBodySource()->getBodies(), previewWidth, 0 + colorTop, previewWidth, colorHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);
	//faces.draw();
	//bodies.draw();
	ofDisableDepthTest();
	camera.end();
	material.begin();
	light.disable();

	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate(), 2), 10, 15);
	ofDrawBitmapString("keys 1-5 load models, spacebar to trigger animation", 10, 30);
	ofDrawBitmapString("drag to control animation with mouseY", 10, 45);
	ofDrawBitmapString("num animations for this model: " + ofToString(model.getAnimationCount()), 10, 60);

}

void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}


//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
void ofApp::keyPressed(int key) {
}