#pragma once
#include "2552software.h"
#include "ofxBox2d.h"
#include "draw.h"

// home of custom scenes

namespace Software2552 {
	// calls the shots, uses the Animation classes to do so bugbug code this in
	class Director {
	public:
		// return a possibly changed and live value from the cameras vector
		shared_ptr<Camera> pickem(vector<shared_ptr<Camera>>&cameras, bool rotating);

	};

	// contains  elements of a stage
	class Stage {
	public:
		void setup();
		virtual void update();
		void draw();
		virtual void test();//shared_ptr<Ball2d> b = std::make_shared<Ball2d>();
		void setBackgroundImageName(const string&name) { backgroundImageName = name; }
		void add(shared_ptr<Camera> camera) { cameras.push_back(camera); };
		void add(shared_ptr<Light> light) { lights.push_back(light); };
		void add(shared_ptr<RoleRaster>image) { images.push_back(image); };
		void add(shared_ptr<RoleVideo> video) { videos.push_back(video); };
		void add(shared_ptr<Grabber> grabber) { grabbers.push_back(grabber); };
		void add(shared_ptr<TextureVideo>tv) { texturevideos.push_back(tv); };

		void add(shared_ptr<RoleSoundPlayer>s) { sounds.push_back(s); };
		void add(shared_ptr<RoleParagraph>p) { paragraphs.push_back(p); };
		void add(shared_ptr<RoleText>t) { texts.push_back(t); };
		void add(shared_ptr<RoleSphere>s) { spheres.push_back(s); };

		vector<shared_ptr<Grabber>>& getGrabbers() { return grabbers; }
		vector<shared_ptr<Camera>>& getCameras() { return cameras; }
		vector<shared_ptr<Light>>& getLights() { return lights; }
		vector<shared_ptr<RoleRaster>>& getImages() { return images; }
		vector<shared_ptr<RoleVideo>>& getVideos() { return videos; }
		vector<shared_ptr<TextureVideo>>& getTextureVideos() { return texturevideos; }
		vector<shared_ptr<RoleSoundPlayer>>& getSounds() { return sounds; }
		vector<shared_ptr<RoleParagraph>>& getParagraphs() { return paragraphs; }
		vector<shared_ptr<RoleText>>& getTexts() { return texts; }
		vector<shared_ptr<RoleSphere>>& getSpheres() { return spheres; }

	protected:
		virtual void draw2d();
		virtual void draw3dFixed() {};
		virtual void draw3dMoving() {};
		virtual void pre3dDraw();
		virtual void post3dDraw();
		virtual void installLightAndMaterialThenDraw(shared_ptr<Camera>); // derive to change where cameras are
	private:
		//bugbug maybe just animatables is needed, a a typeof or such can be used
		vector<shared_ptr<DrawingBasics>> animatables;//bugbug make all these ptrs so dervied classes can be used
		vector<shared_ptr<Grabber>> grabbers;
		vector<shared_ptr<Camera>>	cameras;
		vector<shared_ptr<Light>>	lights;
		vector<shared_ptr<RoleRaster>>		images;
		vector<shared_ptr<TextureVideo>>	texturevideos;
		vector<shared_ptr<RoleVideo>>		videos;
		vector<shared_ptr<RoleSoundPlayer>> sounds;
		vector<shared_ptr<RoleParagraph>>   paragraphs; 
		vector<shared_ptr<RoleText>>		texts;
		vector<shared_ptr<RoleSphere>>		spheres;
		//bugbug can put more things like spheres here once spheres work and if it makes sense

		//bugbug get the right classes so there is one type of animator then push them
		//all up here
		vector<shared_ptr<Animator>> animators;

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