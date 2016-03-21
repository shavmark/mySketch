#pragma once

#include "2552software.h"
#include "animation.h"
#include "color.h"
#include "draw.h"

// json driven model

namespace Software2552 {
	// model helpers

	void echoValue(const Json::Value &data, bool isError = false);
	bool echoJSONTree(const string& functionname, const Json::Value &root, bool isError = false);
#define ECHOAll(data) echoJSONTree(__FUNCTION__, data);
#define ERROR_ECHOAll(data) echoJSONTree(__FUNCTION__, data, true);
	template<typename T> shared_ptr<vector<shared_ptr<T>>> parse(const Json::Value &data);

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
		void operator=(const Settings& rhs) {setSettings(rhs);	}
		bool readFromScript(const Json::Value &data);

		ofTrueTypeFont& getFont() { return font.get(); }
		shared_ptr<ofxSmartFont> getFontPointer() { return font.getPointer(); }
		bool operator==(const Settings& rhs) { return rhs.name == name; }
		string &getName() { return name; }
		void setSettings(const Settings& rhs);

		void setSettings(Settings* rhs) {
			if (rhs != nullptr) {
				setSettings(*rhs);
			}
		}

	protected:
		Font   font;
		string notes;// unstructured string of info, can be shown to the user
		string title; // title object
		string name; // any object can have a name, note, date, reference, duration
	protected:

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

	protected:
		string locationPath; // can be local too
		string location;
		string source;
	};

	// an actor is a drawable that contains the drawing object for that graphic
	class Actor : public Settings {
	public:
		Actor(DrawingBasics *p) :Settings() {
			references = nullptr;
			player = p;
		}
		~Actor() {
			if (player != nullptr) {
				delete player;
			}
		}

		bool read(const Json::Value &data);

		shared_ptr<vector<shared_ptr<Reference>>>  getReferences() { return references; }

		virtual bool readFromScript(const Json::Value &data) { return true; };

		DrawingBasics* getPlayer() { return player; }
	private:
		DrawingBasics* player; // need a down cast
		shared_ptr<vector<shared_ptr<Reference>>> references;
	};
	template<typename T> void createTimeLineItems(const Settings& settings, vector<shared_ptr<Actor>>& vec, const Json::Value &data, const string& key) {
		for (Json::ArrayIndex j = 0; j < data[key].size(); ++j) {
			shared_ptr<T> v = std::make_shared<T>();
			v->setSettings(settings); // inherit settings
			if (v->readFromScript(data[key][j])) {
				// only save if data was read in 
				vec.push_back(v);
			}
		}
	}


	class Background : public Actor {
	public:
		Background() :Actor(new RoleBackground()) {  }
		RoleBackground* getPlayer() { return ((RoleBackground*)getPlayer()); }
	};
	class Ball : public Actor {
	public:
		Ball() :Actor(new Ball2d()) {  }
		Ball2d* getPlayer() { return ((Ball2d*)getPlayer()); }
	};

	class Picture : public Actor {
	public:
		Picture() :Actor(new RoleRaster()) {  }
		Picture(const string&s) :Actor(new RoleRaster(s)) {  }
		RoleRaster* getPlayer() { return ((RoleRaster*)getPlayer()); }
	};
	
	class Text : public Actor {
	public:
		Text() :Actor(new RoleText()) {  }
		RoleText* getPlayer() { return ((RoleText*)getPlayer()); }
		bool readFromScript(const Json::Value &data);
	};

	class Paragraph : public Actor {
	public:
		Paragraph() :Actor(new RoleParagraph()) {  }
		RoleParagraph* getPlayer() { return ((RoleParagraph*)getPlayer()); }
		bool readFromScript(const Json::Value &data);
	};

	
	class Sphere : public Actor {
	public:
		Sphere() :Actor(new RoleSphere()) {  }
		RoleSphere* getPlayer() { return ((RoleSphere*)getPlayer()); }
		bool readFromScript(const Json::Value &data);
	};
	// audio gets an x,y,z which can be ignored for now but maybe surround sound will use these for depth
	class Audio : public Actor {
	public:
		Audio() :Actor(new RoleSound()) {  }
		RoleSound* getPlayer() { return ((RoleSound*)getPlayer()); }
		bool readFromScript(const Json::Value &data);
	};

	class Video : public Actor {
	public:
		Video(const string&s) :Actor(new RoleVideo(s)) {  }
		Video() :Actor(new RoleVideo()) {  }
		RoleVideo* getPlayer() { return ((RoleVideo*)getPlayer()); }
		bool readFromScript(const Json::Value &data);
	};

	// class Character 3d, 2d, talking, movment, etc.  not for this release, way to complicated in this world, lots of tools, not all is compatable 
	// item in a play list
	class PlayItem {
	public:
		PlayItem() {}
		PlayItem(const string&keynameIn) { keyname = keynameIn; }
		bool readFromScript(const Json::Value &data);
		bool operator==(const PlayItem rhs) { return rhs.keyname == keyname; }
		string &getKeyName() { return keyname; }

	private:
		string keyname;
	};
	class Playlist {
	public:
		bool readFromScript(const Json::Value &data);
		shared_ptr<vector<shared_ptr<PlayItem>>> getList() { return list; }
	private:
		shared_ptr<vector<shared_ptr<PlayItem>>> list;
	};

}