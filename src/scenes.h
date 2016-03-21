#pragma once
#include "2552software.h"
#include "model.h"
#include "ofxBox2d.h"
#include "draw.h"

// home of custom scenes

namespace Software2552 {

	// calls the shots, uses the Animation classes to do so bugbug code this in, add it to its own file
	// it replaces time line
	class Director {
	public:
		// return a possibly changed and live value from the cameras vector
		shared_ptr<Camera> pickem(vector<shared_ptr<Camera>>&cameras, bool rotating);
		// owns scenes, read, run, delete when duration is over
		//objectLifeTimeManager
	};

	// contains  elements of a stage
	class Stage {
	public:
		void setup();
		virtual void update();
		void draw();
		virtual bool create(const Json::Value &data) { return true; };

		virtual void test();//shared_ptr<Ball2d> b = std::make_shared<Ball2d>();

		void clear(bool force=false);
		void pause();
		void resume();
		float findMaxWait();
		Settings settings;

	protected:
		// drawing tools
		void setBackgroundImageName(const string&name) { backgroundImageName = name; }

		void add(shared_ptr<Camera> camera) { cameras.push_back(camera); };
		void add(shared_ptr<Light> light) { lights.push_back(light); };
		void add(shared_ptr<TextureVideo>tv) { texturevideos.push_back(tv); };
		void add(shared_ptr<Grabber>g) { grabbers.push_back(g); };

		vector<shared_ptr<Camera>>& getCameras() { return cameras; }
		vector<shared_ptr<Light>>& getLights() { return lights; }
		vector<shared_ptr<TextureVideo>>& getTextureVideos() { return texturevideos; }
		vector<shared_ptr<Grabber>>& getGrabbers() { return grabbers; }

		// things to draw
		void addAnimatable(shared_ptr<Actor>p) { animatables.push_back(p); }
		vector<shared_ptr<Actor>>& getAnimatables() { return animatables; }

		virtual void draw2d();
		virtual void draw3dFixed();
		virtual void draw3dMoving();
		virtual void pre3dDraw();
		virtual void post3dDraw();

		virtual void installLightAndMaterialThenDraw(shared_ptr<Camera>); // derive to change where cameras are
		string keyname;

	private:
		static bool OKToRemove(shared_ptr<Actor> me) {
			return me->getPlayer()->OKToRemoveNormalPointer(me->getPlayer());
		}
		void removeExpiredItems(vector<shared_ptr<Actor>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), OKToRemove), v.end());
		}
		void removeExpiredItems(vector<shared_ptr<Camera>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), DrawingBasics::OKToRemove), v.end());
		}
		void removeExpiredItems(vector<shared_ptr<Light>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), DrawingBasics::OKToRemove), v.end());
		}
		void removeExpiredItems(vector<shared_ptr<TextureVideo>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), DrawingBasics::OKToRemove), v.end());
		}
		void removeExpiredItems(vector<shared_ptr<Grabber>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), DrawingBasics::OKToRemove), v.end());
		}
		//bugbug maybe just animatables is needed, a a typeof or such can be used
		vector<shared_ptr<Actor>> animatables;
		vector<shared_ptr<Camera>> cameras;
		vector<shared_ptr<Light>> lights;
		vector<shared_ptr<TextureVideo>> texturevideos;
		vector<shared_ptr<Grabber>>	grabbers;
		//bugbug can put more things like spheres here once spheres work and if it makes sense

		Material material;//bugbug need to learn this but I expect it pairs with material, just make a vector<pair<>>
		Director director;

		void draw3d();

		ofImage imageForBackground;//bugbug change this to use the background object that includes just a color background
		string backgroundImageName;

	};

	class Planet : public RoleSphere {
	public:
		RoleSphere sphere;
		ofTexture texture;
	};

	// over time this just does whats in the data
	class GenericScene :public Stage {
	public:
		bool create(const Json::Value &data);
	private:
	};

	class TestBallScene :public Stage {
	public:
		bool create(const Json::Value &data);
	private:
	};

	class TestScene :public Stage {
	public:
		TestScene():Stage() {}
		void setup();
		void update();
		void test();
		void draw3dFixed();
	private:

		RoleCube cube;
		CrazyMesh mesh;
	};
	class SpaceScene : public Stage {
	public:
		void setup();
		void test();
		void update();
		void draw2d();
		void draw3dFixed();
		void draw3dMoving();
		void addPlanet(const string&textureName, const ofVec3f& Start);
	private:
		// things a scene can have (infinte list really)
		RoleSphere	videoSphere;
		vector<shared_ptr<Planet>> pictureSpheres;//bugbug move up to baseclass with a pointer vector to drawing items
	};


}