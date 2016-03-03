#pragma once
#include "2552software.h"
#include "ofxBox2d.h"

// home of custom drawing

namespace Software2552 {
	class Text;
	class Character;
	class Particles;
	class Video;

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
	template<typename T>
	class Engine {
	public:
		// fill in the ones you need in derived classes
		virtual void draw(T*) {};
		virtual void update(T*) {};
		virtual void setup(T*) {};
		virtual void play() {};
		virtual void stop() {};
		virtual void pause() {};
	};

	class ParticlesEngine : public Engine<Particles> {
	public:
		void draw(Particles*particles);
		void update(Particles*particles);
		void setup(Particles*particles);
		int years;
		vector <ofPtr<ofxBox2dCircle> >	circles;		  //	default box2d circles
		vector <ofPtr<ofxBox2dRect> > boxes;			  //	defalut box2d rects
	private:
		// use contains for multiple objects or complicated objects
		ofxBox2dParticleSystem part;
		ofxBox2d box2d;
	};
	
	class CharacterEngine : public Engine<Character> {
	public:
		CharacterEngine() {}

	private:
	};
	//ofVideoPlayer
	class VideoEngine :public ofVideoPlayer {
	public:
		VideoEngine() {}
		void draw(Video*v);
		void update(Video*v);
		void setup(Video*v);
	private:
	};
	class TextEngine : public Engine<Text> {
	public:
		TextEngine() {}
		void draw(Text*);

	private:
	};

}