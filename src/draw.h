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
		void setup();
	private:
	};
	//http://openframeworks.cc/ofBook/chapters/generativemesh.html
	class MoreMesh : public MeshEngine {
	public:
		void setup();
		void draw();
	private:
		ofImage image;
		ofEasyCam easyCam;
		ofLight light;

	};
	class SceneLearn {
	public:
		void setup() {
			// turn on smooth lighting //
			ofSetSmoothLighting(true);

			center.set(ofGetWidth()*.5, ofGetHeight()*.5, 0);

			// Point lights emit light in all directions //
			// set the diffuse color, color reflected from the light source //
			light.setDirectional();
			// specular color, the highlight/shininess color //
			ColorSet set = Colors::getFirstColor(ColorSet::RedBlue);
			ofColor y = ofColor().fromHex(set.getHex(3));
			ofFloatColor x = Colors::getFloatColor(Colors::lightColor);
			ofColor y2 = Colors::getOfColor(Colors::lightColor);
			light.setSpecularColor(x);
			//setDiffuseColor
			light.setPosition(center.x, center.y, 0);

			// shininess is a value between 0 - 128, 128 being the most shiny //
			boxMaterial.setShininess(64);

			materialColor.setBrightness(250.f);
			materialColor.setSaturation(200);

			light.setup();
			light.setPosition(-100, 200, 0);
			ofEnableDepthTest();
			
			x = Colors::getFloatColor(Colors::backColor);
			boxMaterial.setDiffuseColor(Colors::getFloatColor(Colors::backColor));
			//boxMaterial.setShininess(0.02);
			roadMaterial.setDiffuseColor(Colors::getFloatColor(Colors::foreColor));
			roadMaterial.setShininess(0.01);

			box.move(200, 0, -200);
			sphere.setParent(box);
			sphere.move(-100, 0, 0);
		}

		void draw() {
			ofSetColor(light.getDiffuseColor());

			// enable lighting //
			ofEnableLighting();
			// the position of the light must be updated every frame, 
			// call enable() so that it can update itself //
			light.enable();
			cam.begin();
			roadMaterial.begin();
			roadMaterial.end();
			boxMaterial.begin();
			box.draw();
			sphere.draw();
			boxMaterial.end();
			cam.end();
			// turn off lighting //
			ofDisableLighting();
		}
		void update() {
			
			light.setDiffuseColor(Colors::getFloatColor(Colors::lightColor));

			materialColor.setHue(Colors::getHue(Colors::backColor));
			// the light highlight of the material //
			boxMaterial.setSpecularColor(materialColor);

		}
		ofLight light;
		ofEasyCam cam;
		ofBoxPrimitive box;
		ofMaterial boxMaterial;
		ofMaterial  roadMaterial;
		ofSpherePrimitive sphere;
		ofVec3f center;
		ofColor materialColor;
    

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