#pragma once

#include "2552software.h"
#include "color.h"
#include "draw.h"

// json driven model

namespace Software2552 {
	// model helpers

	void echoValue(const Json::Value &data, bool isError = false);
	bool echoJSONTree(const string& functionname, const Json::Value &root, bool isError = false);
#define ECHOAll(data) echoJSONTree(__FUNCTION__, data);
#define ERROR_ECHOAll(data) echoJSONTree(__FUNCTION__, data, true);

	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data);

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
	//http://pocoproject.org/slides/070-DateAndTime.pdf
	class DateAndTime : public Poco::DateTime {
	public:
		// default to no no date to avoid the current date being set for bad data
		DateAndTime() : Poco::DateTime(0, 1, 1) {
			timeZoneDifferential = 0;
			bc = 0;
		}
		void operator=(const DateAndTime& rhs) {
			timeZoneDifferential = rhs.timeZoneDifferential;
			bc = rhs.bc;
			assign(rhs.year(), rhs.month(), rhs.day(), rhs.hour(), rhs.minute(), rhs.second(), rhs.microsecond(), rhs.microsecond());
		}
		const string format = "%dd %H:%M:%S.%i";
		string getDate() {
			return Poco::DateTimeFormatter::format(timestamp(), format);
		}
		bool readFromScript(const Json::Value &data);
		int timeZoneDifferential;
		int bc; // non zero if its a bc date

#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(DateAndTime));
			if (bc) {
				logVerbose("BC");
				logVerbose(ofToString(bc));
			}
			else {
				logVerbose(getDate());
				logVerbose(ofToString(timeZoneDifferential));
			}
		}
#endif // _DEBUG
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


	// one set per 
	class TheSet : public Animator {
	public:
		TheSet() {
			framerate = 30;
		}
		void setup();
		void draw();
		void update();

		bool readFromScript(const Json::Value &data) {
			//floatColor colorAmbient(light.getAmbientColor());
			//colorAmbient.read(data["ambientColor"]);
			//light.setAmbientColor(colorAmbient.get());

			//floatColor colorDiffuse(light.getDiffuseColor());
			//colorAmbient.read(data["diffuseColor"]);
			//light.setDiffuseColor(colorDiffuse.get());
			return true;
		}

		/// use pointers if data set gets too large

		RoleBackground background; // there is only one background, but it can have different settings
		int framerate; // only one framerate
		vector<ofLight>	light;
		vector<ofEasyCam> camera;
		Colors colors;
	};
	//  settings get copied a lot as they are the default data for all classes so they need to stay small
	class Settings {
	public:
		Settings() {
			init();
		}
		Settings(const string& nameIn) {
			init();
			name = nameIn;
		}
		void operator=(const Settings& rhs) {
			setSettings(rhs);
		}
		bool readFromScript(const Json::Value &data);

		ofTrueTypeFont& getFont() { return font.get(); }
		shared_ptr<ofxSmartFont> getFontPointer() { return font.getPointer(); }
		bool operator==(const Settings& rhs) { return rhs.name == name; }
		string &getName() { return name; }

		void setSettings(Settings* rhs) {
			if (rhs != nullptr) {
				setSettings(*rhs);
			}
		}

#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(Settings));

			logVerbose(name);
			logVerbose(notes);
		}
#endif

	protected:
		Font   font;
		string notes;// unstructured string of info, can be shown to the user
		string title; // title object
		string name; // any object can have a name, note, date, reference, duration
	protected:
		void setSettings(const Settings& rhs);

	private:
		void init() {
			//Poco::Timespan totalTime = 1 * 1000 * 1000;
		}

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

		// echo object (debug only)
#if _DEBUG
		void trace() {
			logVerbose(STRINGIFY(Reference));
			Settings::trace();
			logVerbose(location);
			logVerbose(locationPath);
			logVerbose(source);
		}
#endif
	protected:
		string locationPath; // can be local too
		string location;
		string source;
	};

	// true for all time based items
	class ReferencedItem : public Dates {
	public:
		bool readFromScript(const Json::Value &data);
#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(ReferencedItem));
			Dates::trace();
		}

#endif // _DEBUG


	protected:
		vector <Reference> references;

	};

	// basic graphic like SUN etc to add flavor
	class Graphic : public ReferencedItem, public Animator {
	public:
		Graphic();

		bool readFromScript(const Json::Value &data);
		
		GraphicID id() { return myID; }
		int getWidth() { return width; }
		int getHeight() { return height; }
		string &getLocation() { return locationPath; }
		float  getVolume() { return volume; }
		Point3D& getStartingPoint() { return startingPoint; }

#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(Graphic));
			ReferencedItem::trace();
			logVerbose(type);
		}
#endif // _DEBUG
	protected:
		string type; // 2d, 3d, other
		int width;
		int height;
		string locationPath; // remote or local bugbug maybe make a better name?  not sure
		float volume;
		Point3D startingPoint; // starting point of object for drawing

	private:
		GraphicID myID;// every graphic item gets a unique ID for deletion and etc
	};

	// an actor if you will
	template <class T> class Actor : public Graphic
	{
	public:
		virtual bool readFromScript(const Json::Value &data) = 0;
		T &getPlayer() {
			test();
			return player;
		}

#if _DEBUG
		// echo object (debug only)
		virtual void test() {
			logVerbose(STRINGIFY(T));
			Graphic::trace();
		}

#endif // _DEBUG

	protected:
		T player;
	};

	
	class Background : public Actor<RoleBackground> {
	};
		// some objects just use the OpenFrameworks objects if things are basic enough
	class Picture : public Actor<ofImage> {
	public:
		bool readFromScript(const Json::Value &data);
		void draw() {
			if (okToDraw()) {
				player.draw(getStartingPoint().x, getStartingPoint().y);
			}
		}
		void update() {
			player.update();
		};
	};
	
	class Text : public Actor<RoleText> {
	public:
		bool readFromScript(const Json::Value &data);
		void draw();
		string& getText() { return text; }
		void setText(const string&t) {
			text = t;
		}
	private:
		string text;
	};

	class Paragraph : public Actor<ofxParagraph> {
	public:
		bool readFromScript(const Json::Value &data);
		void draw();
	};


	// audio gets an x,y,z which can be ignored for now but maybe surround sound will use these for depth
	class Audio : public Actor<ofSoundPlayer> {
	public:
		bool readFromScript(const Json::Value &data);
		void setup();
	};

	class Video : public Actor<RoleVideo> {
	public:

		bool readFromScript(const Json::Value &data);
		void setup();
		void update() {
			player.update();
		};
		void draw() {
			if (okToDraw()) {
				player.setPaused(false);
				player.draw(this);
			}
			else {
				player.setPaused(true);
			}
		}
		void pause() {
			Graphic::pause();
			player.setPaused(true);
		}
		void stop() {
			Graphic::stop();
			player.stop();
		}
		void play() {
			Graphic::play();
			player.play();
		}

		// 
		virtual uint64_t getTimeBeforeStart(uint64_t t = 0);
		string test;
	};

	// 3d, 2d, talking, movment, etc will get complicated but put in basics for now
	class Character : public Actor<RoleCharacter> {
	public:

		bool readFromScript(const Json::Value &data);
	private:
		// player is just a string object
	};

	class Scene;
	class GraphicEngines {
	public:

		void bumpWait(uint64_t wait) {
			for (auto& t : get()) {
				t->addWait(wait);
			}
		}
		void play() {
			for (auto& t : get()) {
				t->play();
			}
		}
		void pause() {
			for (auto& t : get()) {
				t->pause();
			}
		}
		void setup() {
			for (auto& t : get()) {
				t->setup();
			}
		}
		void draw() {
			for (auto& t : get()) {
				t->draw();
			}
		}
		void update() {
			for (auto& t : get()) {
				t->update();
			}
		}
		uint64_t findMaxWait() {
			uint64_t f = 0;
			for (auto& t : get()) {
				setIfGreater(f, t->getDuration() + t->getWait());
			}
			return f;
		}
		// remove items that are timed out
		void removeExpiredItems();

		bool dataAvailable();
		void bumpWaits(uint64_t wait);
		uint64_t getLongestWaitTime();
		//		std::shared_ptr<Video> sp1 =
//			std::dynamic_pointer_cast<Video>(graphicsHelpers[0]);
		/* example
				shared_ptr<Video> v = std::make_shared<Video>();
		v->test = "hi";
		graphicsHelpers.push_back(v);
		shared_ptr<Paragraph> v2 = std::make_shared<Paragraph>();
		graphicsHelpers.push_back(v2);
*/
		vector<shared_ptr<Graphic>>& get() { 
			return graphicsHelpers;
		}
	private:
		vector<shared_ptr<Graphic>> graphicsHelpers;
	};

	class Scene : public Settings {
	public:
		Scene(const string&keynameIn);
		Scene();
		void setEngine(shared_ptr<GraphicEngines> enginesIn = nullptr);
		bool operator==(const Scene& rhs) { return rhs.keyname == keyname; }
		bool readFromScript(const Json::Value &data);
		template<typename T> void createTimeLineItems(const Json::Value &data, const string& key);
		string &getKey() { return keyname; }

		// we want folks to use wrappers to avoid tons of dependicies on this
		shared_ptr<GraphicEngines> getDrawingEngines() {
			if (engines == nullptr) {
				setEngine();
			}
			return engines;
		}

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Scene));
			traceVector(getDrawingEngines()->get());
		}

#endif // _DEBUG

	protected:
		string keyname;

	private:
		shared_ptr<GraphicEngines> engines;

	};
	// item in a play list
	class PlayItem {
	public:
		PlayItem() {}
		PlayItem(const string&keynameIn) { keyname = keynameIn; }
		bool readFromScript(const Json::Value &data);
		bool operator==(const PlayItem& rhs) { return rhs.keyname == keyname; }
		string &getKeyName() { return keyname; }
		Scene scene;
#if _DEBUG
		void trace() {
			logVerbose(STRINGIFY(PlayItem));
			scene.trace();
		}
#endif

	private:
		string keyname;
	};
	class Playlist {
	public:
		bool readFromScript(const Json::Value &data);
		vector<PlayItem>& getList() { return list; }
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Playlist));
		}
#endif
	private:
		vector<PlayItem> list;
	};


	// an Act is one json file that contains 1 or more scenes
	class Act {
	public:
		Act() {}

		// read a deck from json (you can also just build one in code)
		bool readFromScript(const string& fileName = "json.json");
		vector<PlayItem> &getPlayList() { return playlist.getList(); };

#if _DEBUG
		// echo object (debug only) 
		void trace() {
			logVerbose(STRINGIFY(Act));
		}
#endif
	private:
		Playlist playlist;

	};

}