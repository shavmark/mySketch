#include "draw.h"
#include "model.h"

// this is the drawing  module where most drawing work is done

namespace Software2552 {
	// add this one http://clab.concordia.ca/?page_id=944
	void RoleVideo::draw(Video*v) {
		ofVideoPlayer::draw(v->getStartingPoint().x, v->getStartingPoint().y);
	}
	void RoleBackground::setup(Colors* colors) { 
		mode = OF_GRADIENT_LINEAR; 
		setRefreshRate(60000);// just set something different while in dev
	}
	// colors and background change over time but not at the same time
	void RoleBackground::update(Colors*colors) {
		//bugbug can add other back grounds like a video loop, sound
		// picture, any graphic etc
		if (refresh()) {
			switch ((int)ofRandom(0, 3)) {
			case 0:
				mode = OF_GRADIENT_LINEAR;
				break;
			case 1:
				mode = OF_GRADIENT_CIRCULAR;
				break;
			case 2:
				mode = OF_GRADIENT_BAR;
				break;
			}
		}

	}

	void Camera::orbit() {
		if (useOrbit) {
			float time = ofGetElapsedTimef();
			float longitude = 10 * time;
			float latitude = 10 * sin(time*0.8);
			float radius = 600 + 50 * sin(time*0.4);
			ofEasyCam::orbit(longitude, latitude, radius, ofPoint(0, 0, 0));
		}
	}

	void RoleBackground::draw(Colors* colors){
		//ofBackgroundHex this is an option too bugbug enable background type

		ofBackgroundGradient(ofColor::fromHex(colors->getForeground()),
			ofColor::fromHex(colors->getBackground()), mode);
		RoleText::draw("print", 100,200);
	}
	void RoleText::draw(const string &s, int x, int y) {
		ofPushStyle();
		Colors::setFontColor();
		Font font;
		font.get().drawString(s, x, y);
		ofPopStyle();
	}
	void MeshEngine::setup() {
		//setMode(OF_PRIMITIVE_POINTS);
		setMode(OF_PRIMITIVE_LINES);
		//setMode(OF_PRIMITIVE_LINE_STRIP);
		//setMode(OF_PRIMITIVE_LINE_LOOP);
		enableColors();
		enableIndices();
		ofVec3f top(100.0, 50.0, 0.0);
		ofVec3f left(50.0, 150.0, 0.0);
		ofVec3f right(150.0, 150.0, 0.0);
		addVertex(top);
		addColor(Colors::getFloatColor(Colors::foreColor));

		addVertex(left);
		addColor(Colors::getFloatColor(Colors::backColor));

		addVertex(right);
		//bugbug this is where we add in more colors, lets see how many before we make 
		// changes, but somthing like Color1, Color2 etc
		addColor(Colors::getFloatColor(Colors::fontColor));

		addIndex(0);
		addIndex(1);
		addIndex(1);
		addIndex(2);
		addIndex(2);
		addIndex(0);

	}
	void MoreMesh::setup() {
		image.load("stars.png");
		image.resize(200, 200); // use less data
		setMode(OF_PRIMITIVE_LINES);
		enableColors();

		float intensityThreshold = 150.0;
		int w = image.getWidth();
		int h = image.getHeight();
		for (int x = 0; x<w; ++x) {
			for (int y = 0; y<h; ++y) {
				ofColor c = image.getColor(x, y);
				float intensity = c.getLightness();
				if (intensity >= intensityThreshold) {
					float saturation = c.getSaturation();
					float z = ofMap(saturation, 0, 255, -100, 100);
					// We shrunk our image by a factor of 4, so we need to multiply our pixel
					// locations by 4 in order to have our mesh cover the openFrameworks window
					ofVec3f pos(x * 4, y * 4,z);
					addVertex(pos);
					// When addColor(...), the mesh will automatically convert
					// the ofColor to an ofFloatColor
					addColor(c);
				}
			}
		}
		// Let's add some lines!
		float connectionDistance = 30;
		int numVerts = getNumVertices();
		for (int a = 0; a < numVerts; ++a) {
			ofVec3f verta = getVertex(a);
			for (int b = a + 1; b < numVerts; ++b) {
				ofVec3f vertb = getVertex(b);
				float distance = verta.distance(vertb);
				if (distance <= connectionDistance) {
					// In OF_PRIMITIVE_LINES, every pair of vertices or indices will be
					// connected to form a line
					addIndex(a);
					addIndex(b);
				}
			}
		}
	}
	void MoreMesh::draw(){
		ofColor centerColor = ofColor(85, 78, 68);
		ofColor edgeColor(0, 0, 0);
		ofBackgroundGradient(centerColor, edgeColor, OF_GRADIENT_CIRCULAR);
		easyCam.begin();
		ofPushMatrix();
		ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2);
		ofMesh::draw();
		ofPopMatrix();
		easyCam.end();
	}
	void RoleText::draw(Text* t) {
		ofPushStyle();
		ofSetHexColor(Colors::getForeground());
		t->getFont().drawString(t->getText(), t->getStartingPoint().x, t->getStartingPoint().y);
		ofPopStyle();
	}

	// its ok also if Controller passes in an object such as a paragraph to copy in
	// bugbug move to a text object
	void Graphics2552::rotateToNormal(ofVec3f normal) {
		normal.normalize();

		float rotationAmount;
		ofVec3f rotationAngle;
		ofQuaternion rotation;

		ofVec3f axis(0, 0, 1);
		rotation.makeRotate(axis, normal);
		rotation.getRotate(rotationAmount, rotationAngle);
		logVerbose("ofRotate " + ofToString(rotationAmount));
		ofRotate(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);
	}

}
