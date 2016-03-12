#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "draw.h"
// home of custom scenes

namespace Software2552 {

	class Planet : public Sphere {
	public:
		Sphere sphere;
		ofTexture texture;
	};
	class SpaceScene {
	public:
		void addPlanet(const string&textureName, const ofVec3f& Start);
		void test();
		void setup();
		void update();
		void draw();
		void draw2d();
		void draw3d();
		void setMainVideoName(const string&name) { mainVideoName = name; }
		void setBackgroundImageName(const string&name) { backgroundImageName = name; }
		void addPlanetName(const string&name) { planetimageNames.push_back(name); }
	private:
		// things a scene can have (infinte list really)
		Sphere	videoSphere;
		vector<Planet> pictureSpheres;
		Camera	camera1, camera2;
		Light	light;
		Material material;
		TextureVideo video;
		ofImage image;
		string mainVideoName;
		string backgroundImageName;
		vector<string> planetimageNames;
	};


}