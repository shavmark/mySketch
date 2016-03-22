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
	class ActorBaseClass : public Settings {
	public:
		ActorBaseClass(DrawingBasics *p) :Settings() {
			references = nullptr;
			player = p;
		}
		~ActorBaseClass();

		bool read(const Json::Value &data);

		shared_ptr<vector<shared_ptr<Reference>>>  getReferences() { return references; }

		virtual bool readFromScript(const Json::Value &data) { return true; };

		DrawingBasics* getDefaultPlayer() { return player; }
	protected:
	private:
		DrawingBasics* player=nullptr; // need a down cast
		shared_ptr<vector<shared_ptr<Reference>>> references=nullptr;
	};
	template<typename T> void createTimeLineItems(const Settings& settings, vector<shared_ptr<ActorBaseClass>>& vec, const Json::Value &data, const string& key) {
		for (Json::ArrayIndex j = 0; j < data[key].size(); ++j) {
			shared_ptr<T> v = std::make_shared<T>();
			v->setSettings(settings); // inherit settings
			if (v->readFromScript(data[key][j])) {
				// only save if data was read in 
				vec.push_back(v);
			}
		}
	}


	class Background : public ActorBaseClass {
	public:
		Background() :ActorBaseClass(new RoleBackground()) {  }
		Colors& getPlayer() { return (((RoleBackground*)getDefaultPlayer())->player); }
	};

	// wrap drawing object with references and settings data
	class Ball : public ActorBaseClass {
	public:
		// data read during scene creation bugbug move all data reads to scene creation?
		Ball() :ActorBaseClass(new Ball2d()) {  }
		Ball2d* getPlayer() { return ((Ball2d*)getDefaultPlayer()); }
	};

	class Picture : public ActorBaseClass {
	public:
		Picture() :ActorBaseClass(new RoleRaster()) {  }
		Picture(const string&s) :ActorBaseClass(new RoleRaster(s)) {  }
		ofImage& getPlayer() { return (((RoleRaster*)getDefaultPlayer())->player); }
	};
	
	class Cube : public ActorBaseClass	{
	public:
		Cube() :ActorBaseClass(new RoleCube()) {  }
		ofBoxPrimitive& getPlayer() { return (((RoleCube*)getDefaultPlayer())->player); }
		bool readFromScript(const Json::Value &data);
	};
	class Text : public ActorBaseClass {
	public:
		Text() :ActorBaseClass(new RoleText()) {  }
		RoleText* getPlayer() { return ((RoleText*)getDefaultPlayer()); }
		bool readFromScript(const Json::Value &data);
	};

	class Paragraph : public ActorBaseClass {
	public:
		Paragraph() :ActorBaseClass(new RoleParagraph()) {  }
		ofxParagraph& getPlayer() { return (((RoleParagraph*)getDefaultPlayer())->player); }
		bool readFromScript(const Json::Value &data);
	};

	
	class Sphere : public ActorBaseClass {
	public:
		Sphere() :ActorBaseClass(new RoleSphere()) {  }
		ofSpherePrimitive& getPlayer() { return (((RoleSphere*)getDefaultPlayer())->player); }
		bool readFromScript(const Json::Value &data);
	};
	class Audio : public ActorBaseClass {
	public:
		Audio() :ActorBaseClass(new RoleSound()) {  }
		ofSoundPlayer& getPlayer() { return (((RoleSound*)getDefaultPlayer())->player); }
		bool readFromScript(const Json::Value &data);
	};

	class Video : public ActorBaseClass {
	public:
		Video(const string&s) :ActorBaseClass(new RoleVideo(s)) {  }
		Video() :ActorBaseClass(new RoleVideo()) {  }
		ofVideoPlayer& getPlayer() { return (((RoleVideo*)getDefaultPlayer())->player);}
		bool readFromScript(const Json::Value &data);
	};
	// item in a play list

	class Stage;
	class Channel : public objectLifeTimeManager {
	public:
		Channel() :objectLifeTimeManager() { setObjectLifetime(30000); }
		Channel(const string&keynameIn) { keyname = keynameIn; setObjectLifetime(30000); }
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
		vector<shared_ptr<Channel>>& getList();
	private:
		vector<shared_ptr<Channel>> list;
	};


}