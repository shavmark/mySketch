#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "color.h"
// home of custom drawing

namespace Software2552 {
	class Text;
	class Character;
	class Particles;
	class Video;
	class Settings;
	class Colors;

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

	// basic mesh stuff
	class MeshEngine : public ofMesh {
	public:
		void setup(ColorSet*colors);
	private:
	};
	//http://openframeworks.cc/ofBook/chapters/generativemesh.html
	class MoreMesh : public MeshEngine {
	public:
		void setup(ColorSet*colors);
		void draw();
	private:
		ofImage image;
		ofEasyCam easyCam;
		ofLight light;
	};
	class SceneLearn {
	public:
		void setup() {
			light.setup();
			light.setPosition(-100, 200, 0);
			ofEnableDepthTest();
			boxMaterial.setDiffuseColor(ofFloatColor::red);
			boxMaterial.setShininess(0.02);
		}

		void draw() {
			cam.begin();
			boxMaterial.begin();
			box.draw();
			boxMaterial.end();
			// here you will draw your object
			cam.end();
		}
		ofLight light;
		ofEasyCam cam;
		ofBoxPrimitive box;
		ofMaterial boxMaterial;

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
	class RoleVideo :public ofVideoPlayer {
	public:
		void draw(Video*v);
	private:
	};
	class RoleText : public Role<Text> {
	public:
		void draw(Text*);
		static void draw(const string &s, int x, int y);

	private:
	};

}