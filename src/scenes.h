#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "draw.h"
// home of custom scenes

namespace Software2552 {

	class BaseScene {
	public:
		void setup();
		void update();
		void draw();
		void test();
		void setBackgroundImageName(const string&name) { backgroundImageName = name; }
	protected:
		vector<Camera>	cameras; // secondary cameras
		Camera			mainCamera; 
		vector<Light>	lights;
		Material material;//bugbug need to learn this but I expect it pairs with material, just make a vector<pair<>>
	private:
		ofImage imageForBackground;
		string backgroundImageName;
	};

	class Planet : public Sphere {
	public:
		Sphere sphere;
		ofTexture texture;
	};
	class SpaceScene : public BaseScene {
	public:
		void setup();
		void update();
		void draw();
		void test();
		void draw2d();
		void draw3d();
		void setMainVideoName(const string&name) { mainVideoName = name; }
		void addPlanet(const string&textureName, const ofVec3f& Start);
		void addPlanetName(const string&name) { planetimageNames.push_back(name); }
	private:
		// things a scene can have (infinte list really)
		Sphere	videoSphere;
		vector<Planet> pictureSpheres;
		TextureVideo video;
		string mainVideoName;
		vector<string> planetimageNames;
	};


}