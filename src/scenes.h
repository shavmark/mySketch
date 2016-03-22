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

	// convert name to object
	shared_ptr<Stage> getScene(const string&name);

	// contains  elements of a stage
	class Stage {
	public:
		void setup();
		void update();
		void draw();
		virtual bool create(const Json::Value &data) { return myCreate(data); };

		void clear(bool force=false);
		void pause();
		void resume();
		float findMaxWait();
		Settings settings;
		string &getKeyName() { return keyname; }
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
		void addAnimatable(shared_ptr<ActorBaseClass>p) { animatables.push_back(p); }
		vector<shared_ptr<ActorBaseClass>>& getAnimatables() { return animatables; }

		virtual void draw2d();
		virtual void draw3dFixed();
		virtual void draw3dMoving();
		virtual void pre3dDraw();
		virtual void post3dDraw();
		virtual void mySetup() {}
		virtual void myUpdate() {}
		virtual void myPause() {}
		virtual void myResume() {}
		virtual void myClear(bool force) {}
		virtual bool myCreate(const Json::Value &data) { return true; }
		virtual bool drawIn3d() { return false; }//derived classes make this call
		virtual bool drawIn2d() { return true; }
		virtual void installLightAndMaterialThenDraw(shared_ptr<Camera>); // derive to change where cameras are
		string keyname;

	private:
		static bool OKToRemove(shared_ptr<ActorBaseClass> me) {
			return me->getDefaultPlayer()->OKToRemoveNormalPointer(me->getDefaultPlayer());
		}
		void removeExpiredItems(vector<shared_ptr<ActorBaseClass>>&v) {
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
		vector<shared_ptr<ActorBaseClass>> animatables;
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
		bool myCreate(const Json::Value &data);

	private:
	};

	class TestBallScene :public Stage {
	public:
		bool myCreate(const Json::Value &data);
	private:
	};

	class TestScene :public Stage {
	public:
		TestScene():Stage() {}
		void mySetup();
		void myUpdate();
		bool myCreate(const Json::Value &data);
	private:
		bool drawIn3d() { return true; }//derived classes make this call
		bool drawIn2d() { return false; }//derived classes make this call
		void draw3dFixed();

		RoleCube cube;
		CrazyMesh mesh;
	};
	class SpaceScene : public Stage {
	public:
		void mySetup();
		void myUpdate();
		bool myCreate(const Json::Value &data);
	private:
		bool drawIn3d() { return true; }//derived classes make this call
		void draw2d();
		void draw3dFixed();
		void draw3dMoving();
		void addPlanet(const string&textureName, const ofVec3f& Start);
		// things a scene can have (infinte list really)
		RoleSphere	videoSphere;
		vector<shared_ptr<Planet>> pictureSpheres;//bugbug move up to baseclass with a pointer vector to drawing items
	};


}