#pragma once

#include "2552software.h"
#include "animation.h"
#include "color.h"
#include "draw.h"
#include <forward_list>

// json driven model

namespace Software2552 {
	// model helpers

	void echoValue(const Json::Value &data, bool isError = false);
	bool echoJSONTree(const string& functionname, const Json::Value &root, bool isError = false);
#define ECHOAll(data) echoJSONTree(__FUNCTION__, data);
#define ERROR_ECHOAll(data) echoJSONTree(__FUNCTION__, data, true);
	template<typename T> shared_ptr<vector<shared_ptr<T>>> parse(const Json::Value &data, const string&location);

#if _DEBUG
	template<typename T> void traceVector(T& vec);
#endif // _DEBUG

	// readJsonValue only if value in json, readJsonValue does not support string due to templatle issues
	template<typename T> bool readJsonValue(T &value, const Json::Value& data);
	bool readStringFromJson(string &value, const Json::Value& data);

	// will only readJsonValue vars if there is a string to readJsonValue, ok to preserve existing values
	// always use this macro or the readJsonValue function to be sure errors are handled consitantly
#define READFLOAT(var, data) readJsonValue(var, data[#var])
#define READINT(var, data) readJsonValue(var, data[#var])
#define READBOOL(var, data) readJsonValue(var, data[#var])
#define READSTRING(var, data) readStringFromJson(var, data[#var])
#define READDATE(var, data) readStringFromJson(var, data[#var])

	class Point3D : public ofVec3f {
	public:
		bool readFromScript(const Json::Value &data);
	};

	class ColorChoice  {
	public:
		bool readFromScript(const Json::Value &data);
		ColorSet::ColorGroup getGroup() { return group; }
	private:
		ColorSet::ColorGroup group = ColorSet::ColorGroup::Default;
	};
	//http://pocoproject.org/slides/070-DateAndTime.pdf
	class DateAndTime : public Poco::DateTime {
	public:
		// default to no no date to avoid the current date being set for bad data
		DateAndTime();
		void operator=(const DateAndTime& rhs);
		const string format = "%dd %H:%M:%S.%i";
		string getDate() {
			return Poco::DateTimeFormatter::format(timestamp(), format);
		}
		bool readFromScript(const Json::Value &data);
	private:
		int timeZoneDifferential;
		int bc; // non zero if its a bc date
	};

	// simple helper to read in font data from json 
#define defaultFontSize 14
#define defaultFontFile "fonts/Raleway-Thin.ttf"
#define defaultFontName "Raleway-Thin"

	//bugbug do we want to expire fonts? maybe in 2.0
	class Font  { 
	public:
		ofTrueTypeFont& get();
		shared_ptr<ofxSmartFont> getPointer();
		bool readFromScript(const Json::Value &data);
	private:
		shared_ptr<ofxSmartFont> font;
	};


	//  settings get copied a lot as they are the default data for all classes so they need to stay small
	class Settings {
	public:
		Settings() {	}
		Settings(const string& nameIn) {name = nameIn;	}
		void operator=(const Settings& rhs) {setSettings(rhs);	}
		bool readFromScript(const Json::Value &data);

		ofTrueTypeFont& getFont() { return font.get(); }
		shared_ptr<ofxSmartFont> getFontPointer() { return font.getPointer(); }
		ColorChoice& getColor();
		void setColor(const ColorChoice& c) { colors = c; }
		bool operator==(const Settings& rhs) { return rhs.name == name; }
		string &getName() { return name; }
		void setSettings(const Settings& rhs);
		void setSettings(Settings* rhs);
		
	protected:
		Font   font;
		ColorChoice  colors;
		string notes;// unstructured string of info, can be shown to the user
		string title; // title object
		string name; // any object can have a name, note, date, reference, duration
	protected:

	private:
	};

	class Dates : public Settings {
	public:
		bool readFromScript(const Json::Value &data);
	protected:
		DateAndTime timelineDate; // date item existed
		DateAndTime lastUpdateDate; // last time object was updated
		DateAndTime itemDate; // date of reference vs the date of the referenced item 

	};
	// reference to a cited item
	class Reference : public Dates {
	public:
		bool readFromScript(const Json::Value &data);

	protected:
		string locationPath; // can be local too
		string location;
		string source;
	};

	// an actor is a drawable that contains the drawing object for that graphic
	class ActorBasics : public Settings {
	public:
		ActorBasics(DrawingBasics *p) :Settings() {
			references = nullptr;
			player = p;
		}
		~ActorBasics();

		template<typename T> T* getRole() { return (T*)getDefaultPlayer(); }
		DrawingBasics* getDefaultPlayer() { return player; }

		bool readFromScript(const Json::Value &data);

		shared_ptr<vector<shared_ptr<Reference>>>  getReferences() { return references; }

		void setAnimation(bool f = true) { getDefaultPlayer()->getAnimationHelper()->setAnimationEnabled(f); }
		void setType(DrawingBasics::drawtype type) { getDefaultPlayer()->setType(type); }
		void animateTo(const ofPoint& p) { getDefaultPlayer()->getAnimationHelper()->animateTo(p); }
		void setAnimationPosition(const ofPoint& p) { getDefaultPlayer()->getAnimationHelper()->setPosition(p); }
		void setAnimationPositionX(float x) { getDefaultPlayer()->getAnimationHelper()->setPositionX(x); }
		void setAnimationPositionY(float y) { getDefaultPlayer()->getAnimationHelper()->setPositionX(y); }
		void setAnimationPositionZ(float z) { getDefaultPlayer()->getAnimationHelper()->setPositionX(z); }

	private:
		virtual bool myReadFromScript(const Json::Value &data) { return true; };// for derived classes
		DrawingBasics* player=nullptr; // need a down cast
		shared_ptr<vector<shared_ptr<Reference>>> references=nullptr;
	};
	class TextureFromImage : public ofTexture {
	public:
		void create(const string& name, float w, float h);
		float getWidth() { return fbo.getWidth(); }
		float getHeight() { return fbo.getHeight(); }
		void bind() { fbo.getTexture().bind(); }
		void unbind() { fbo.getTexture().unbind(); }
		ofTexture& getTexture() { return fbo.getTexture(); }
	private:
		ofFbo fbo;
	};

	template<typename T> void createTimeLineItems(const Settings& settings, forward_list<shared_ptr<ActorBasics>>& vec, const Json::Value &data, const string& key) {
		for (Json::ArrayIndex j = 0; j < data[key].size(); ++j) {
			shared_ptr<T> v = std::make_shared<T>();
			v->setSettings(settings); // inherit settings
			if (v->readFromScript(data[key][j])) {
				// only save if data was read in 
				vec.push_front(v);
			}
		}
	}



	class AnimiatedColor : public ofxAnimatableOfColor {
	public:
		AnimiatedColor() :ofxAnimatableOfColor() {}
		AnimiatedColor(ColorChoice&color);
		// animates colors
		void draw();
		bool paused() { return paused_; }
		bool readFromScript(const Json::Value &data);
	private:
		ColorChoice color;
	};

	// wrap drawing object with references and settings data
	class Ball : public ActorBasics {
	public:
		// bouncy ball with nice colors is pretty nice, does not take too much really
		class Role : public DrawingBasics {
		public:
			void myDraw();

			int floorLine = 630;
			int xMargin = 0;
			int widthCol = 60;
			float radius = 100;
		};

		// data read during scene creation bugbug move all data reads to scene creation?
		Ball() :ActorBasics(new Role()) {  }
	private:
		bool myReadFromScript(const Json::Value &data);
	};

	// camera, lights etc
	class EquipementBaseClass : public objectLifeTimeManager {
	public:
		bool readFromScript(const Json::Value &data) { return myReadFromScript(data); }
	private:
		virtual bool myReadFromScript(const Json::Value &data) =0;// for derived classes
	};
	// cameras (and others like it) are not actors
	class Camera : public EquipementBaseClass {
	public:
		void orbit();
		void setOrbit(bool b = true) { useOrbit = b; }
		bool isOrbiting() const { return useOrbit; }
		ofEasyCam& getPlayer() { return player; }
	private:
		ofEasyCam player;
		bool myReadFromScript(const Json::Value &data);
		bool useOrbit = false;
	};
	class Light : public EquipementBaseClass {
	public:
		ofLight &getPlayer() { return player; }
		void setX(float x) { loc.x = x; }
		void setY(float y) { loc.y = y; }
		void setZ(float z) { loc.z = z; }
		void setLoc(float x, float y = 0, float z = 0) { loc.x = x; loc.y = y; loc.z = z; }
		ofPoint loc; // light is set on object within camera scope
	private:
		ofLight player;
		virtual bool myReadFromScript(const Json::Value &data);
	};
	class PointLight : public Light {
	public:
		PointLight() :Light() { getPlayer().setPointLight(); }
	private:
		virtual bool myReadFromScript(const Json::Value &data);
	};
	class DirectionalLight : public Light {
	public:
		DirectionalLight() :Light() { getPlayer().setDirectional(); }
	private:
		virtual bool myReadFromScript(const Json::Value &data);
	};
	class SpotLight : public Light {
	public:
		SpotLight() :Light() { getPlayer().setSpotlight(); }
	private:
		virtual bool myReadFromScript(const Json::Value &data);
	};
	class Grabber : public ActorBasics {
	public:
		class Role : public DrawingBasics {
		public:
			void myUpdate();
			void myDraw();
			void mySetup() { loadGrabber(w, h); }
			int w = 320;
			int h = 240;
			int x = 0;
			int y = 0;
			ofVideoGrabber player;
		private:
			bool loadGrabber(int wIn, int hIn);
			int find();
			int id = 0;
		};
		Grabber(const string&name) :ActorBasics(new Role()) { getRole<Role>()->setLocationPath(name); }
		ofVideoGrabber& getPlayer() { return getRole<Role>()->player; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};
	class ActorWithPrimativeBaseClass;
	class DrawingPrimitive3d : public DrawingBasics {
	public:
		~DrawingPrimitive3d() { }
		DrawingPrimitive3d() : DrawingBasics() { setType(draw3dFixedCamera); }
		void setWireframe(bool b = true) { wireFrame = b; }
		bool useWireframe() { return wireFrame; }
		void myDraw() { basedraw(); };
	private:
		void basedraw();
		virtual void basicdraw() = 0;
		virtual void basicdrawWire() = 0;
		bool wireFrame = true;
	};
	class ActorWithPrimativeBaseClass : public ActorBasics {
	public:
		ActorWithPrimativeBaseClass(DrawingPrimitive3d *p = nullptr) :ActorBasics(p){}
		DrawingPrimitive3d *getPrimative() { return (DrawingPrimitive3d*)getDefaultPlayer();  }
	};
	class Cube : public ActorWithPrimativeBaseClass {
	public:
		class Role : public DrawingPrimitive3d {
		public:
			Role() :DrawingPrimitive3d() {}
			void basicdraw() { player.draw(); }
			void basicdrawWire() { player.drawWireframe(); }
			ofBoxPrimitive player;
		};
		Cube() : ActorWithPrimativeBaseClass(new Role()) {		}
		ofBoxPrimitive* getPlayer() { return &getRole<Role>()->player; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};
	class Plane : public ActorWithPrimativeBaseClass {
	public:
		class Role : public DrawingPrimitive3d {
		public:
			Role() :DrawingPrimitive3d() {}
			void basicdraw() { player.draw(); }
			void basicdrawWire() { player.drawWireframe(); }
			ofPlanePrimitive player;
		};
		Plane() : ActorWithPrimativeBaseClass(new Role()) {		}
		ofPlanePrimitive* getPlayer() { return &getRole<Role>()->player; }

	private:
		bool myReadFromScript(const Json::Value &data);
	};
	class Sphere : public ActorWithPrimativeBaseClass {
	public:
		class Role : public DrawingPrimitive3d {
		public:
			Role() :DrawingPrimitive3d() {}
			void basicdraw() { player.draw(); }
			void basicdrawWire() { player.drawWireframe(); }
			ofSpherePrimitive player;
		};
		Sphere() : ActorWithPrimativeBaseClass(new Role()) {		}
		ofSpherePrimitive& getPlayer() { return getRole<Role>()->player; }

	private:
		bool myReadFromScript(const Json::Value &data);
	};

	class Text : public ActorBasics {
	public:
		class Role : public DrawingBasics {
		public:
			void myDraw();
			void drawText(const string &s, int x, int y);
			void setText(const string&t) { text = t; }
			string& getText() { return text; }
			string text;
		};
		Text() : ActorBasics(new Role) {  }
		void drawText(const string &s, int x, int y) { getRole<Role>()->drawText(s, x, y); };
		void setText(const string&t) { getRole<Role>()->setText(t); }
	private:
		bool myReadFromScript(const Json::Value &data);
	};

	class Paragraph : public ActorBasics {
	public:
		// put advanced drawing in these objects
		class Role : public DrawingBasics {
		public:
			Role() : DrawingBasics() {}
			void myDraw();
			ofxParagraph player;
		private:
		};

		Paragraph() :ActorBasics(new Role()) {  }
		ofxParagraph& getPlayer() { return getRole<Role>()->player; }

	private:
		bool myReadFromScript(const Json::Value &data);
	};

	
	class Audio : public ActorBasics {
	public:
		// sound gets drawing basics for path and possibly other items in the future
		class Role : public DrawingBasics {
		public:
			//bugbug tie into the main sound code we added
			void mySetup();
			ofSoundPlayer player;
		};

		Audio() :ActorBasics(new Role()) {  }
		ofSoundPlayer& getPlayer() { return getRole<Role>()->player; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};


	class Video : public ActorBasics {
	public:
		// put advanced drawing in these objects
		class Role :public DrawingBasics {
		public:
			Role() : DrawingBasics() {  }
			Role(const string& path) : DrawingBasics(path) { }
			void myUpdate() { player.update(); }
			void myDraw();
			void mySetup();
			float getTimeBeforeStart(float t);
			ofVideoPlayer player;
		};
		Video(const string&s) :ActorBasics(new Role(s)) {  }
		Video() :ActorBasics(new Role()) {  }
		ofVideoPlayer& getPlayer() { return getRole<Role>()->player; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};

	class TextureVideo : public ActorBasics {
	public:
		class Role : public DrawingBasics {
		public:
			Role() : DrawingBasics() {  }
			Role(const string& path) : DrawingBasics(path) { }
			void myUpdate() { player.update(); }
			void myDraw();
			void mySetup();
			bool textureReady() { return  player.isInitialized(); }
			bool mybind();
			bool myunbind();
			ofVideoPlayer player;
			ofTexture& getTexture();
			ofFbo fbo;
		private:
			ofTexture defaulttexture;
		};
		TextureVideo(const string&s) :ActorBasics(new Role(s)) {  }
		TextureVideo() :ActorBasics(new Role()) {  }
		ofVideoPlayer& getPlayer() { return getRole<Role>()->player; }
		ofTexture& getTexture() { return getRole<Role>()->getTexture(); }

	private:
		bool myReadFromScript(const Json::Value &data);

	};
	class VideoSphere : public ActorBasics {
	public:
		class Role : public DrawingBasics {
		public:
			friend VideoSphere;
			Role() : DrawingBasics() {	video = std::make_shared<TextureVideo>();	}
			Role(const string& path) : DrawingBasics(path) { video = std::make_shared<TextureVideo>(path); }
			void mySetup() {};
			void myDraw();
			Sphere sphere;
		private:
			bool set = false;
			shared_ptr<TextureVideo> video;//bugbug make this a base pointer to share this object with any 3d shape
		};
		VideoSphere(const string&s) :ActorBasics(new Role(s)) {		}
		VideoSphere() :ActorBasics(new Role()) {  }
		Sphere& getSphere() { return getRole<Role>()->sphere; }
		shared_ptr<TextureVideo> getTexture() { return getRole<Role>()->video; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};

	class Planet : public ActorBasics {
	public:
		class Role : public DrawingBasics {
		public:
			Role() : DrawingBasics() { texture = std::make_shared<TextureFromImage>(); }
			void mySetup();
			void myDraw();
			Sphere sphere;
			shared_ptr<TextureFromImage> getTexturePtr() {	return texture;		}
		private:
			shared_ptr<TextureFromImage> texture=nullptr;
		};
		Planet() :ActorBasics(new Role()) {  }
		Sphere& getSphere() { return getRole<Role>()->sphere; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};


	class Picture : public ActorBasics {
	public:
		class Role : public DrawingBasics {
		public:
			Role() : DrawingBasics() {  }
			Role(const string& path) : DrawingBasics(path) { }

			void myUpdate() { player.update(); }
			void mySetup();
			void myDraw();
		
			bool isLoaded = false;
			ofImage player;
		};

		Picture() :ActorBasics(new Role()) {  }
		Picture(const string&s) :ActorBasics(new Role(s)) {  }
		ofImage& getPlayer() { return getRole<Role>()->player; }
	private:
		bool myReadFromScript(const Json::Value &data);
	};
	class Background : public ActorBasics {
	public:
		enum TypeOfBackground { Image, ColorFixed, ColorChanging, GradientFixed, GradientChanging, none };
		class Role : public DrawingBasics {
		public:
			Role() { mode = OF_GRADIENT_LINEAR; }
			void myDraw();
			void myUpdate();// make image a vector then rotate via animation
			void setForegroundColor(const ofColor&c) { currentForegroundColor = c; }
			void setBackgroundColor(const ofColor&c) { currentBackgroundColor = c; }
			void setType(TypeOfBackground typeIn = ColorFixed) { type = typeIn; }
			void setGradientMode(const ofGradientMode& modeIn) { mode = modeIn; }
			shared_ptr<Picture> player=nullptr; // only set if an image is used
		private:
			ofGradientMode mode;
			TypeOfBackground type = ColorFixed;
			ofColor currentBackgroundColor;
			ofColor currentForegroundColor;
		};
		Background() :ActorBasics(new Role()) {  }
		shared_ptr<Picture> getPicture() { return getRole<Role>()->player; }

	private:
		bool myReadFromScript(const Json::Value &data);

	};

	// item in a play list

	class Stage;
	class Channel : public objectLifeTimeManager {
	public:
		Channel() :objectLifeTimeManager() {  }
		Channel(const string&keynameIn) { keyname = keynameIn;  }
		enum ChannelType{History, Art, Sports, Any};
		
		bool readFromScript(const Json::Value &data);
		bool operator==(const Channel rhs) { return rhs.keyname == keyname; }
		string &getKeyName() { return keyname; }
		shared_ptr<Stage> getStage() { return stage; }
		void setStage(shared_ptr<Stage>p) { stage = p; }
		bool getSkip() { return skip; }
		void setSkip(bool b = true) { skip = b; }

		ChannelType getType() { return channeltype; }
		void setType(ChannelType typeIn) { channeltype =typeIn; }
	private:
		shared_ptr<Stage> stage = nullptr;
		string keyname;
		bool skip = false;
		ChannelType channeltype = History;
	};
	class ChannelList {
	public:
		shared_ptr<Channel> getCurrent();
		shared_ptr<Channel> getbyType(Channel::ChannelType, int number = 0);
		shared_ptr<Channel> getbyName(const string&name);
		shared_ptr<Channel> getbyNumber(int i = 0);

		bool readFromScript(const Json::Value &data);
		void removeExpiredItems() {
			list.erase(std::remove_if(list.begin(), list.end(), objectLifeTimeManager::OKToRemove), list.end());
		}
		bool read(const string&path);
		bool setStage(shared_ptr<Stage> p);
		bool skipChannel(const string&keyname);
		vector<shared_ptr<Channel>>& getList();
	private:
		vector<shared_ptr<Channel>> list;
	};

}