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

	class Sphere : public ofSpherePrimitive {
	public:
		void draw() {
			// currenting thinking we let color operate on its own via its class
			ofSetColor(ofColor::white); 
			if (wireFrame) {
				drawWireframe();
			}
			else {
				ofSpherePrimitive::draw();
			}
			//drawFaces();
			//drawVertices();
		}
		void setForTexture(ofTexture& texture) {
			mapTexCoordsFromTexture(texture);
			//rotate(180, 0, 1, 0);// flip for camera
		}
		void setWireframe(bool b = true) { wireFrame = b; }
	private:
		bool wireFrame = true;
	};

	class Camera : public ofEasyCam {
	public:
		void setOrbit() {
			float time = ofGetElapsedTimef();
			float longitude = 10 * time;
			float latitude = 10 * sin(time*0.8);
			float radius = 800 + 50 * sin(time*0.4);
			orbit(longitude, latitude, radius, ofPoint(0, 0, 0));
		}
	};
	class Fbo : public ofFbo {
	public:
	};

	class TextureVideo : public ofVideoPlayer {
	public:
		void create(const string& name, float w, float h) {
			loadMovie(name);
			play();
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
	class Light : public ofLight {
	public:
	};
	class Material : public ofMaterial {
	public:
	};
	class SceneLearn {
	public:
		void setup() {
			//bugbug can we set size at update?
			texture.create("hubble1.jpg", ofGetWidth()/2, ofGetHeight() / 2) ;
			pictureSphere.setForTexture(texture);

			video.create("Clyde.mp4", ofGetWidth() / 2, ofGetHeight() / 2);

			videoSphere.set(250, 150);
			pictureSphere.set(100, 40);
			pictureSphere.setWireframe(false);
			videoSphere.move(ofVec3f(0, 0, 0));
			pictureSphere.move(ofVec3f(ofGetWidth()-(video.getWidth()),0, 0));

		}
		void update() {
			video.update();
		}
		void draw() {
			draw2d();
			draw3d();
		}
		void draw2d() {
			//fbo.draw(0, 0, ofGetWidth(), ofGetHeight());// fbo now drawing
		}
		void draw3d() {
			if (video.isFrameNew()) {
				videoSphere.setForTexture(video.getTexture());
			}
			ofSetSmoothLighting(true);
			ofBackgroundGradient(ofColor::blue, ofColor::aqua, OF_GRADIENT_BAR);
			video.bind();
			light.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, ofGetWidth());
			light.enable();
			material.begin();
			ofEnableDepthTest();
			camera.setOrbit();

			camera.begin();
			videoSphere.draw();
			video.unbind();
			texture.bind();
			pictureSphere.draw();
			texture.unbind();
			camera.end();

			ofDisableDepthTest();
			material.end();
			light.disable();
			ofDisableLighting();
		}
	private:
		// things a scene can have (infinte list really)
		Sphere	videoSphere, pictureSphere;
		Camera	camera;
		Light	light;
		Material material;
		TextureFromImage texture;
		TextureVideo video;
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