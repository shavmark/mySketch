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

	class DrawingPrimitive : public DrawingBasics {
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
	class Fbo : public ofFbo {
	public:
	};

	class Grabber : public DrawingBasics {
	public:
		Grabber(const string&nameIn) : DrawingBasics(){ name = nameIn;  }
		void myUpdate() { if (player.isInitialized()) player.update(); }
		void myDraw();
		bool myObjectLoad() { return loadGrabber(w, h); }
		bool loadGrabber(int wIn, int hIn);
		ofVideoGrabber player;
	private:
		int find();
		string name;
		int id=0;
	};
	class RolePlane :  public DrawingPrimitive {
	public:
		void myDraw();
		ofPlanePrimitive player;
	};
	class RoleSphere : public DrawingPrimitive {
	public:
		void myDraw();
		ofSpherePrimitive player;
	private:
	};

	// sound gets drawing basics for path and possibly other items in the future
	class RoleSound :  public DrawingBasics {
	public:
		//bugbug tie into the main sound code we added

		void mySetup();
		ofSoundPlayer player;
	};
	class RoleRaster :  public DrawingBasics {
	public:
		RoleRaster() : DrawingBasics() {  }
		RoleRaster(const string& path) : DrawingBasics(path) { }

		void myUpdate() { player.update(); }
		bool myObjectLoad() {		return ofLoadImage(player, getLocationPath());	}
		void myDraw();
		ofImage player;
	};

	class TextureVideo :  public DrawingBasics {
	public:
		void create(const string& name, float w, float h) {
			player.load(name);
			player.play();
		}
		void myUpdate() { player.update(); }

		bool textureReady() {	return  player.isInitialized();		}
		bool bind();
		bool unbind();
		ofVideoPlayer player;
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
	class Light : public DrawingBasics {
	public:
		ofLight player;
	};
	class Material : public ofMaterial {
	public:
	};
	class RoleBackground : public DrawingBasics {
	public:
		RoleBackground(){ mode = OF_GRADIENT_LINEAR; }
		void mySetup();
		void myDraw();
		void myUpdate();
		Colors player;
	private:
		ofGradientMode mode;

		//ofBackgroundHex this is an option too bugbug enable background type
	};
	// put advanced drawing in these objects
	class RoleParagraph : public DrawingBasics {
	public:
		RoleParagraph() : DrawingBasics() {}
		void myDraw();
		ofxParagraph player;
	private:
	};
	// put advanced drawing in these objects
	class RoleVideo :public DrawingBasics {
	public:
		RoleVideo() : DrawingBasics() {  }
		RoleVideo(const string& path) : DrawingBasics(path) { }
		void myUpdate() { player.update(); }
		void myDraw();
		void mySetup();
		bool myObjectLoad();
		float getTimeBeforeStart(float t);
		ofVideoPlayer player;
	private:
	};

}