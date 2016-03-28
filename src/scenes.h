#pragma once
#include "2552software.h"
#include "model.h"
#include "ofxBox2d.h"
#include "draw.h"
#include <forward_list>

// home of custom scenes

namespace Software2552 {

	// calls the shots, uses the Animation classes to do so bugbug code this in, add it to its own file
	// it replaces time line
	class Director {
	public:
		// return a possibly changed and live value from the cameras vector
		shared_ptr<Camera> pickem(forward_list<shared_ptr<Camera>>&cameras, bool rotating);
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
		virtual bool create(const Json::Value &data);
		virtual void setBackground(const Json::Value &data, shared_ptr<Background> background);
		void clear(bool force=false);
		void pause();
		void resume();
		float findMaxWait();
		void drawlights();
		Settings settings;
		string &getKeyName() { return keyname; }
	protected:

		void add(shared_ptr<Camera> camera) { cameras.push_front(camera); };
		void add(shared_ptr<Light> light) { lights.push_front(light); };

		forward_list<shared_ptr<Camera>>& getCameras() { return cameras; }
		forward_list<shared_ptr<Light>>& getLights() { return lights; }

		// things to draw
		void addAnimatable(shared_ptr<ActorBasics>p) { animatables.push_front(p); }
		template<typename T> shared_ptr<T> CreateReadAndaddAnimatable(const Json::Value &data, const string&location);
		template<typename T> shared_ptr<T> CreateReadAndaddAnimatable(const Json::Value &data);
		shared_ptr<Camera> CreateReadAndaddCamera(const Json::Value &data, bool rotate=false);
		shared_ptr<VideoSphere> CreateReadAndaddVideoSphere(const Json::Value &data);
		template<typename T>shared_ptr<T> CreateReadAndaddLight(const Json::Value &data);
		void addPlanets(const Json::Value &data, ofPoint& min);

		forward_list<shared_ptr<ActorBasics>>& getAnimatables() { return animatables; }

		void draw2d();
		void draw3dFixed();
		void draw3dMoving();

		virtual void myDraw2d() {};
		virtual void myDraw3dFixed() {};
		virtual void myDraw3dMoving() {};

		virtual void pre3dDraw();
		virtual void post3dDraw();
		virtual void mySetup() {}
		virtual void myUpdate() {}
		virtual void myPause() {}
		virtual void myResume() {}
		virtual void myClear(bool force) {}
		virtual bool myCreate(const Json::Value &data) { return true; }
		virtual bool drawIn3dFixed() { return false; }//derived classes make this call
		virtual bool drawIn3dMoving() { return false; }//derived classes make this call
		virtual bool drawIn2d() { return true; }
		virtual void installLightAndMaterialThenDraw(shared_ptr<Camera>, bool drawfixed); // derive to change where cameras are
		string keyname;

	private:
		static bool OKToRemove(shared_ptr<ActorBasics> me) {
			return me->getDefaultPlayer()->OKToRemoveNormalPointer(me->getDefaultPlayer());
		}
		void removeExpiredItems(forward_list<shared_ptr<ActorBasics>>&v) {
			v.remove_if(OKToRemove);
		}
		void removeExpiredItems(forward_list<shared_ptr<Camera>>&v) {
			v.remove_if(objectLifeTimeManager::OKToRemove);
		}
		void removeExpiredItems(forward_list<shared_ptr<Light>>&v) {
			v.remove_if(objectLifeTimeManager::OKToRemove);
		}
		//bugbug maybe just animatables is needed, a a typeof or such can be used
		forward_list<shared_ptr<ActorBasics>> animatables;
		forward_list<shared_ptr<Camera>> cameras;
		forward_list<shared_ptr<Light>> lights;
		//bugbug can put more things like spheres here once spheres work and if it makes sense

		Material material;//bugbug need to learn this but I expect it pairs with material, just make a vector<pair<>>
		Director director;
		void draw3d();

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
		bool drawIn3dFixed() { return true; }// if no camera then use this only

		bool drawIn3dMoving() { return true; }//if set to true there must be a camera or things will draw 2x

		bool drawIn2d() { return true; }//derived classes make this call
		void myDraw3dFixed();

		CrazyMesh mesh;
	};
	class SpaceScene : public Stage {
	public:
		bool myCreate(const Json::Value &data);
	private:
		bool drawIn3dMoving() { return true; }//derived classes make this call
		bool drawIn3dFixed() { return true; }//derived classes make this call
	};


}