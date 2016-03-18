#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "color.h"
#include "animation.h"
#include "ofSoundPlayer.h"
// home of custom drawing

namespace Software2552 {
	class Text;
	class Character;
	class Particles;
	class Video;
	class Settings;
	class Colors;


	// simple drawing 
	class Rectangle : public DrawingBasics {
	public:

		void draw();
	};

	class RandomDots {
	public:

		void draw() {
			for (int i = 0; i < ofGetMouseX() * 5; i++) {
				ofSetColor(ofRandom(96));
				ofRect(ofRandom(ofGetWidth()), ofRandom(ofGetHeight()), 4, 4);
			}
		}
	};

	class Line {
	public:
		ofPoint a;
		ofPoint b;
	};

	// bouncy ball with nice colors is pretty nice, does not take too much really
	class Ball2d : public Animatable {
	public:
		void draw();
		int floorLine = 630;
		int xMargin = 0;
		int widthCol = 60;
		float width=100;
	};

	//bugbug many more moving shapes, or do all things move?

	// https://github.com/openframeworks/ofBook/blob/master/chapters/lines/chapter.md
	class ComplexLines {
	public:
		void draw() {
			for (const auto& line : lines) {
				ofDrawLine(line.a, line.b);
			}
		}
		//bugbug convert to code vs mouse using random
		void mouseDragged(int x, int y, int button) {
			for (auto point : drawnPoints) {
				ofPoint mouse;
				mouse.set(x, y);
				float dist = (mouse - point).length();
				if (dist < 30) {
					Line lineTemp;
					lineTemp.a = mouse;
					lineTemp.b = point;
					lines.push_back(lineTemp);
				}
			}
			drawnPoints.push_back(ofPoint(x, y));
		}
	private:
		vector < ofPoint > drawnPoints;
		vector < Line > lines;
	};
	class Line3D {
	public:
		void setup();
		void update();
		void draw();

	private:
		ofNode baseNode;
		ofNode childNode;
		ofNode grandChildNode;
		ofPolyline line;
	};
	class SoundOut {
	public:
		static void setup();
		static void update();
		static void draw();
		static void audioOut(ofSoundBuffer &outBuffer);

		class dataOut {
		public:
			double wavePhase;
			double pulsePhase;
			double sampleRate;
			mutex audioMutex;
			ofSoundBuffer lastBuffer;
			ofPolyline waveform;
			float rms;
		};

	private:
		static dataOut soundDataOut;
	};

	class SoundIn {
	public:
		static void setup();
		static void update();
		static void audioIn(float * input, int bufferSize, int nChannels);
		class dataIn {
		public:
			vector <float> left;
			vector <float> right;
			vector <float> volHistory;
			int 	bufferCounter;
			int 	drawCounter;
			float smoothedVol;
			float scaledVol;
			ofSoundStream soundStream;
		};

	private:
		static dataIn soundDataIn;
	};
	// drawing related items start here
	class BaseClass2552WithDrawing : public BaseClass {
	public:
		BaseClass2552WithDrawing() { valid = false; }

		bool objectValid() { return valid; } // data is in a good state
		void setValid(bool b = true) { valid = b; };

	private:
		bool valid; // true when data is valid and ready to draw
	};
	class Graphics2552 {
	public:
		static void rotateToNormal(ofVec3f normal);

	};

	template<typename T>
	class Role : public Animator {
	public:
		// fill in the ones you need in derived classes
		virtual void draw(T*) {};
		virtual void update(T*) {};
		virtual void setup(T*) {};
		virtual void play() {};
		virtual void stop() {};
		virtual void pause() {};
	};

	class CrazyMesh : public ofMesh {
	public:
		CrazyMesh(const ofFloatColor&colorIn = ofFloatColor::orange) : ofMesh() { color = colorIn; }
		virtual void setup();
		virtual void update();
		virtual void draw();
	private:
		ofFloatColor color;
		int w = 200;
		int h = 200;
	};

	// basic mesh stuff for learning
	class MeshEngine : public ofMesh {
	public:
		void setup();
	private:
	};

	//http://openframeworks.cc/ofBook/chapters/generativemesh.html
	class MoreMesh : public MeshEngine {
	public:
		void setup();
		void draw();
	private:
		ofImage		image;
		ofEasyCam	easyCam;
		ofLight		light;

	};

	// bugbug things that come from openframeworks do not get the anmation base class, too many collisions
	//bugbug figure out how to animate such things, like moving a camera, is such 
	// animation done in the Director?

	class Primitive : public DrawingBasics {
	public:
		void setWireframe(bool b = true) { wireFrame = b; }
		bool useWireframe() { return wireFrame; }
	private:
		bool wireFrame = true;

	};
	class Plane : public ofPlanePrimitive, public Primitive {
	public:
		void draw() {
			if (useWireframe()) {
				drawWireframe();
			}
			else {
				ofPlanePrimitive::draw();
			}
		}
	}; 
	class Cube : public ofBoxPrimitive, public Primitive {
	public:
		void draw() {
			if (useWireframe()) {
				drawWireframe();
			}
			else {
				ofBoxPrimitive::draw();
			}
		}
	}; 
	class Sphere : public ofSpherePrimitive, public Primitive {
	public:
		void draw() {
			if (useWireframe()) {
				drawWireframe();
			}
			else {
				ofSpherePrimitive::draw();
			}
			//drawFaces();
			//drawVertices();
		}

	private:
	};
	class VectorPattern : public Animator {
	public:
		void matrix(int twistx, int shifty) {
			ofPushMatrix();
			for (int i = -50; i < 50; ++i) {
				ofTranslate(0, i*shifty);
				ofRotate(i*twistx);
				stripe(true);
			}
			ofPopMatrix();
		}
		void stripe(bool rotate=false) {
			ofSetColor(ofColor::black);
			ofSetLineWidth(3);
			for (int i = -50; i < 50; ++i) {
				ofPushMatrix();
				ofTranslate(i * 20, 0);
				if (rotate) {
					ofRotate(i * 5);
				}
				ofLine(0, -100, 0, 100);
				ofPopMatrix();
			}
		}
		void triangle(bool rotate = false) {
			ofSetColor(ofColor::black);
			ofSetLineWidth(3);
			ofNoFill();
			for (int i = -50; i < 50; ++i) {
				ofPushMatrix();
				ofTranslate(i * 20, 0);
				if (rotate) {
					ofRotate(i * 5);
				}
				ofScale(6, 6); // enlarge 6x
				ofTriangle(0, 0, -50, 100, 50, 100);
				ofPopMatrix();
			}
		}
		void shape(int twistx, int shifty, bool rect, bool fill, int rotate, int alpha=100) {
			ofColor color = ofColor::black;
			color.a = alpha;
			ofSetColor(color);
			ofSetLineWidth(1);
			if (fill) {
				ofFill();
			}
			else {
				ofNoFill();
			}
			for (int i = -50; i < 50; ++i) {
				ofPushMatrix();
				ofRotate(i * twistx);
				ofTranslate(i * 20, shifty);
				ofRotate(rotate);
				ofScale(6, 6); // enlarge 6x
				if (rect) {
					ofRect(-50, -50, 100, 100);
				}
				else {
					ofTriangle(0, 0, -50, 100, 50, 100);
				}
				ofPopMatrix();
			}
			ofScale(6, 6); // enlarge 6x
			ofTriangle(0, 0, -50, 100, 50, 100);
		}
	};
	class Camera : public ofEasyCam, public DrawingBasics {
	public:
		void orbit();
		void setOrbit(bool b = true) { useOrbit = b; }
		bool isOrbiting() const { return useOrbit; }
	private:
		bool useOrbit = false;
	};
	class Fbo : public ofFbo {
	public:
	};

	class Grabber : public ofVideoGrabber, public DrawingBasics {
	public:
		Grabber(const string&nameIn) :ofVideoGrabber(), DrawingBasics(){ name = nameIn;  }
		void loadGrabber(int w, int h) {
			id = find();
			setDeviceID(id);
			setDesiredFrameRate(30);
			setup(w, h);
		}
		bool loadRaster() {
			return true;// ofLoadImage(*this, path);
		}
	private:
		int find() {
			//bugbug does Kintect show up?
			ofVideoGrabber grabber;
			vector<ofVideoDevice> devices = grabber.listDevices();
			for (vector<ofVideoDevice>::iterator it = devices.begin(); it != devices.end(); ++it) {
				if (it->deviceName == name) {
					return it->id;
				}
			}
		}
		string name;
		int id=0;
	};
	// sound gets drawing basics for path and possibly other items in the future
	class SoundPlayer : public ofFmodSoundPlayer, public DrawingBasics {
	public:
		//bugbug tie into the main sound code we added
	};
	class Raster : public ofImage, public DrawingBasics {
	public:
		Raster(const string&pathIn) :ofImage(), DrawingBasics() { path = pathIn; }
		bool loadRaster() {
			return ofLoadImage(*this, path);
		}
	private:
		string path;
	};

	class VideoPlayer :public ofVideoPlayer, public DrawingBasics {
	public:
		VideoPlayer(const string&pathIn) :ofVideoPlayer(), DrawingBasics() { path = pathIn; }
		bool loadVideo() {
			return load(path);
		}
	private:
		string path;
	};

	class TextureVideo : public ofVideoPlayer {
	public:
		void create(const string& name, float w, float h) {
			load(name);
			play();
		}
		bool textureReady() {
			return isInitialized();
		}
		bool bind() { 
			if (isInitialized() && isUsingTexture()) {
				getTexture().bind();
				return true;
			}
			return false;
		}
		bool unbind() {
			if (isInitialized() && isUsingTexture()) {
				getTexture().unbind();
				return true;
			}
			return false;
		}
	private:

	};
	class TextureFromImage : public ofTexture {
	public:
		void setup() {
		}
		void create(const string& name, float w, float h) {
			// create texture
			ofLoadImage(*this, name);
			fbo.allocate(w,h, GL_RGB);
			fbo.begin();//fbo does drawing
			ofSetColor(ofColor::white); // no image color change when using white
			draw(0, 0, w,h);
			fbo.end();// normal drawing resumes
		}
		float getWidth() { return fbo.getWidth(); }
		float getHeight() { return fbo.getHeight(); }
		void bind() { fbo.getTextureReference().bind(); }
		void unbind() { fbo.getTextureReference().unbind(); }
		
	private:
		Fbo	fbo;

	};
	class Light : public ofLight, public Animator {
	public:
	};
	class Material : public ofMaterial {
	public:
	};
	class RoleBackground : public Role<Colors> {
	public:
		RoleBackground(){ mode = OF_GRADIENT_LINEAR; }
		void setup(Colors* color);
		void draw(Colors* color);
		void update(Colors*color);
	private:
		ofGradientMode mode;

		//ofBackgroundHex this is an option too bugbug enable background type
	};
	class RoleCharacter : public Role<Character> {
	public:

	private:
	};
	// put advanced drawing in these objects
	class RoleParagraph :public ofxParagraph, public DrawingBasics {
	public:

	private:
	};
	// put advanced drawing in these objects
	class RoleVideo :public ofVideoPlayer, public DrawingBasics {
	public:
		void draw(Video*v);
		
	private:
	};
	class RoleText : public Role<Text> {
	public:
		void draw(Text*);
		static void draw(const string &s, int x, int y);

		string& getText() { return text; }
		void setText(const string&t) { text = t; }
	private:
		string text;
	};

}