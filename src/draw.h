#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "color.h"
#include "animation.h"
#include "ofSoundPlayer.h"
// home of custom drawing

namespace Software2552 {

	// simple drawing 
	class Rectangle : public DrawingBasics {
	public:

		void draw();
	};

	// internal helpers
	class RandomDots {
	public:

		void draw();
	};

	class Line {
	public:
		ofPoint a;
		ofPoint b;
	};

	// bouncy ball with nice colors is pretty nice, does not take too much really
	class Ball2d : public DrawingBasics {
	public:
		void myDraw();

		int floorLine = 630;
		int xMargin = 0;
		int widthCol = 60;
		float radius=100;
	};
	//bugbug many more moving shapes, or do all things move?

	// https://github.com/openframeworks/ofBook/blob/master/chapters/lines/chapter.md
	class ComplexLines {
	public:
		void draw();
		//bugbug convert to code vs mouse using random
		void mouseDragged(int x, int y, int button);
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
	class VectorPattern : public DrawingBasics {
	public:
		void matrix(int twistx, int shifty);
		void stripe(bool rotate = false);
		void triangle(bool rotate = false);
		void shape(int twistx, int shifty, bool rect, bool fill, int rotate, int alpha = 100);
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
		void myUpdate() { if (isInitialized()) ofVideoGrabber::update(); }
		void myDraw();
		bool myObjectLoad() { return loadGrabber(w, h); }
		bool loadGrabber(int wIn, int hIn);
	private:
		int find();
		string name;
		int id=0;
	};
	class RolePlane : public ofPlanePrimitive, public Primitive {
	public:
		void myDraw();
	};
	class RoleCube : public ofBoxPrimitive, public Primitive {
	public:
		void myDraw();
	};
	class RoleSphere : public ofSpherePrimitive, public Primitive {
	public:
		void myDraw();

	private:
	};

	// sound gets drawing basics for path and possibly other items in the future
	class RoleSound : public ofSoundPlayer, public DrawingBasics {
	public:
		//bugbug tie into the main sound code we added

		void mySetup();

	};
	class RoleRaster : public ofImage, public DrawingBasics {
	public:
		RoleRaster() :ofImage(), DrawingBasics() {  }
		RoleRaster(const string& path) :ofImage(), DrawingBasics(path) { }

		void myUpdate() { ofImage::update(); }
		bool myObjectLoad() {		return ofLoadImage(*this, getLocationPath());	}
		void myDraw();
	};

	class TextureVideo : public ofVideoPlayer, public DrawingBasics {
	public:
		void create(const string& name, float w, float h) {
			load(name);
			play();
		}
		void myUpdate() { ofVideoPlayer::update(); }

		bool textureReady() {	return isInitialized();		}
		bool bind();
		bool unbind();
	private:

	};
	class TextureFromImage : public ofTexture {
	public:
		void create(const string& name, float w, float h);
		float getWidth() { return fbo.getWidth(); }
		float getHeight() { return fbo.getHeight(); }
		void bind() { fbo.getTextureReference().bind(); }
		void unbind() { fbo.getTextureReference().unbind(); }
		
	private:
		Fbo	fbo;

	};
	class Light : public ofLight, public DrawingBasics {
	public:
	};
	class Material : public ofMaterial {
	public:
	};
	class RoleBackground : public Colors, public DrawingBasics {
	public:
		RoleBackground(){ mode = OF_GRADIENT_LINEAR; }
		void mySetup();
		void myDraw();
		void myUpdate();
	private:
		ofGradientMode mode;

		//ofBackgroundHex this is an option too bugbug enable background type
	};
	// put advanced drawing in these objects
	class RoleParagraph :public ofxParagraph, public DrawingBasics {
	public:
		RoleParagraph() : ofxParagraph() {}
		void myDraw();

	private:
	};
	// put advanced drawing in these objects
	class RoleVideo :public ofVideoPlayer, public DrawingBasics {
	public:
		RoleVideo() :ofVideoPlayer(), DrawingBasics() {  }
		RoleVideo(const string& path) :ofVideoPlayer(), DrawingBasics(path) { }
		void myUpdate() { ofVideoPlayer::update(); }
		void myDraw();
		void mySetup();
		bool myObjectLoad();
		float getTimeBeforeStart(float t);
	private:
	};
	class RoleText :  public DrawingBasics {
	public:
		void myDraw();
		void drawText(const string &s, int x, int y);
		void setText(const string&t) { text = t; }
		string& getText() { return text; }
	private:
		string text;
	};

}