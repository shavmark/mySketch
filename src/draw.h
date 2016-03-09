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
	class Engine : public Animator {
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
	};

	class BackgroundEngine : public Engine<Colors> {
	public:
		BackgroundEngine(){ mode = OF_GRADIENT_LINEAR; }
		void setup(Colors* color);
		void draw(Colors* color);
		void update(Colors*color);
	private:
		ofGradientMode mode;
		//ofBackgroundHex this is an option too bugbug enable background type
	};
	class CharacterEngine : public Engine<Character> {
	public:

	private:
	};
	// put advanced drawing in these objects
	class VideoEngine :public ofVideoPlayer {
	public:
		void draw(Video*v);
	private:
	};
	class TextEngine : public Engine<Text> {
	public:
		void draw(Text*);
		static void draw(const string &s, int x, int y);

	private:
	};
	//  ofxBox2dParticleSystem Created by Atsushi Tadokoro on 8/23/14.  Pulled here to break free and use standard ofxbox2d plugin
	class ofxBox2dParticleSystem : public ofxBox2dBaseShape {

	private:
		b2World * b2dworld;

	public:
		b2ParticleSystemDef particleSystemDef;
		b2ParticleSystem* particleSystem;
		ofVboMesh mesh;
		float particleSize;
		float lifetime;
		ofColor color;
		b2ParticleFlag flag;
		ofImage textureImage;
		bool useTexture;

		ofxBox2dParticleSystem();
		void setup(b2World * b2dworld);
		void setup(b2World * b2dworld, int maxCount);
		void setup(b2World * b2world, int maxCount, float lifetime, float radius, float particleSize, ofColor color);
		void draw();

		int32 createParticle(ofVec2f position, ofVec2f velocity);
		int32 createParticle(float position_x, float position_y, float velocisty_x, float velocisty_y);

		void createRectParticleGroup(ofVec2f position, ofVec2f size, ofColor color);
		void createCircleParticleGroup(ofVec2f position, float radius, ofColor color);

		void loadImage(string fileName);

		void setRadius(float radius);
		void setParticleLifetime(float lifetime);
		void setColor(ofColor color);
		void setParticleFlag(b2ParticleFlag flag);

		int getParticleCount();

		void applyForce(int32 particle_index, const ofVec2f& force);
		void applyForce(int32 particle_index, float force_x, float force_y);

	};

}