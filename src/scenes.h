#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "draw.h"
// home of custom scenes

namespace Software2552 {
	// calls the shots
	class Director {
	public:
		// return a possibly changed and live value from the cameras vector
		Camera* pickem(vector<Camera>&cameras, bool rotating);

	};

	// contains  elements of a stage
	class Stage {
	public:
		void setup();
		void update();
		void draw();
		void test();
		void setBackgroundImageName(const string&name) { backgroundImageName = name; }
		void add(const Camera& camera) { cameras.push_back(camera); };
		void add(const Light& light) { lights.push_back(light); };
		void add(const Raster& image) { images.push_back(image); };
		void add(const VideoPlayer& video) { videos.push_back(video); };
		void add(const Grabber& grabber) { grabbers.push_back(grabber); };
		vector<Grabber>& getGrabbers() { return grabbers; }
		vector<Camera>& getCameras() { return cameras; }
		vector<Light>& getLights() { return lights; }
		vector<Raster>& getImages() { return images; }
		vector<VideoPlayer>& getVideos() { return videos; }
	protected:
		virtual void draw2d();
		virtual void draw3dFixed() {};
		virtual void draw3dMoving() {};
	private:
		vector<Grabber> grabbers;
		vector<Camera>	cameras; 
		vector<Light>	lights;
		vector<Raster>	images;
		vector<VideoPlayer>   videos;
		Material material;//bugbug need to learn this but I expect it pairs with material, just make a vector<pair<>>
		Director director;
		void draw3d();
		Camera *camFixed  = nullptr; // set via draw3dStart
		Camera *camMoving = nullptr; // set via draw3dStart
		ofImage imageForBackground;//bugbug change this to use the background object that includes just a color background
		string backgroundImageName;
	};

	class Planet : public Sphere {
	public:
		Sphere sphere;
		ofTexture texture;
	};

	class TestScene :public Stage {
	public:
		TestScene():Stage() {}
		void setup();
		void update();
		void test();
		void draw2d();
		void draw3dFixed();
		void draw3dMoving();
	private:
		Cube cube;
		CrazyMesh mesh;
	};
	class SpaceScene : public Stage {
	public:
		void setup();
		void update();
		void test();
		void draw2d();
		void draw3dFixed();
		void draw3dMoving();
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