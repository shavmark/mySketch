#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "color.h"
#include "animation.h"

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
		ofImage		image;
		ofEasyCam	easyCam;
		ofLight		light;

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
		}
		void setWireframe(bool b = true) { wireFrame = b; }

	private:
		bool wireFrame = true;
	};

	class Camera : public ofEasyCam, public Animator {
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