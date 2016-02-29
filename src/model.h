#pragma once

#include "2552software.h"
#include "draw.h"

// timeline software, json based

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
	bool setString(string &value, const Json::Value& data);

	// will only readJsonValue vars if there is a string to readJsonValue, ok to preserve existing values
	// always use this macro or the readJsonValue function to be sure errors are handled consitantly
#define READFLOAT(var, data) readJsonValue(var, data[#var])
#define READINT(var, data) readJsonValue(var, data[#var])
#define READBOOL(var, data) readJsonValue(var, data[#var])
#define READSTRING(var, data) setString(var, data[#var])
#define READDATE(var, data) setString(var, data[#var])

	class Point3D : public ofVec3f {
	public:
		bool read(const Json::Value &data);
#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(Point3D));
			logVerbose(ofToString(x));
			logVerbose(ofToString(y));
			logVerbose(ofToString(z));
		}
#endif // _DEBUG

	};
	class Color : public ofColor {
	public:
		bool read(const Json::Value &data);
#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(RGB));
			logVerbose(ofToString(r));
			logVerbose(ofToString(g));
			logVerbose(ofToString(b));
		}
#endif // _DEBUG

	};

	//http://pocoproject.org/slides/070-DateAndTime.pdf
	class DateAndTime : public Poco::DateTime {
	public:
		// default to no no date to avoid the current date being set for bad data
		DateAndTime() : Poco::DateTime(0,1,1){
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
		bool read(const Json::Value &data);
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

	class Font  {
	public:
		Font() {
			// always install the default foint
			if (!fontExists(defaultFontFile, defaultFontSize)) {
				font = ofxSmartFont::add(defaultFontFile, defaultFontSize, defaultFontName);
			}
			else {
				font = ofxSmartFont::get(defaultFontFile, defaultFontSize);
			}
		}
		// only add if its not already there
		void addFont(const string& file, int size = defaultFontSize, const string& name = "default") {
			if (!fontExists(file, size)) {
				font = ofxSmartFont::add(file, size, name);;
			}
		}

		bool read(const Json::Value &data);
		shared_ptr<ofxSmartFont> font; // no need to re-wrap
		bool fontExists(const string& name, int size = defaultFontSize) {
			return ofxSmartFont::get(name, size) != nullptr;
		}

#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(Font));
			if (font != nullptr) {
				logVerbose(font->name());
				logVerbose(font->file());
				logVerbose(ofToString(font->size()));
			}
			else {
				basicTrace("no font");
			}
		}
#endif // _DEBUG

	};

	//  settings
	class Settings {
	public:
		Settings() {
			init();
		}
		Settings(const string& nameIn) {
			init();
			name = nameIn;
		}
		const string JsonName = "settings";
		void operator=(const Settings& rhs) { 
			setSettings(rhs);
		}
		void setSettings(const Settings& rhs);
		bool read(const Json::Value &data);

		Point3D& getStartingPoint() {	return startingPoint;	}

		shared_ptr<ofxSmartFont> getFont() {
			return font.font; 
		}
		bool operator==(const Settings& rhs) { return rhs.name == name; }
		string &getName() { return name; }
		const ofColor& getForeground(){ return foregroundColor; }
		const ofColor& getBackground() { return backgroundColor; }
		float getDuration() { return duration; }
		float getWait() { return wait; }
		void setWait(float waitIn) { wait = waitIn; }
		void addWait(float waitIn) { wait += waitIn; }
#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(Settings));
			foregroundColor.trace();
			backgroundColor.trace();
			startingPoint.trace();
			font.trace();
			timelineDate.trace(); // date item existed
			lastUpdateDate.trace(); // last time object was updated
			itemDate.trace();

			logVerbose(name);
			logVerbose(notes);
			logVerbose(ofToString(duration));
			logVerbose(ofToString(wait));
		}
#endif

	protected:
		Font   font;
		DateAndTime timelineDate; // date item existed
		DateAndTime lastUpdateDate; // last time object was updated
		DateAndTime itemDate; // date of reference vs the date of the referenced item 
		string name; // any object can have a name, note, date, reference, duration
		string notes;// unstructured string of info, can be shown to the user
		Color  foregroundColor;
		Color  backgroundColor;
		float  duration; // life time of object, 0 means forever
		float  wait;     // time to wait before drawing
		Point3D startingPoint; // starting point of object for drawing
	private:
		void init() {
			Poco::Timespan totalTime = 1 * 1000 * 1000;
			duration = 0;
			wait = 0;
			foregroundColor.set(0, 0, 255);
			backgroundColor.set(255, 255, 0);
		}
	};

	// reference to a cited item
	class Reference : public Settings {
	public:
		bool read(const Json::Value &data);

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
	class ReferencedItem : public Settings {
	public:
		ReferencedItem() : Settings() {};
		ReferencedItem(const string &name) :Settings(name) {};
#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(ReferencedItem));
			Settings::trace();
			
			for (auto& ref : references) {
				ref.trace();
			}
		}

#endif // _DEBUG

		bool read(const Json::Value &data);

	protected:
		vector <Reference> references;
		
	};

	// basic graphic like SUN etc to add flavor
	class Graphic : public ReferencedItem {
	public:
		Graphic();
		
		bool read(const Json::Value &data);
		// for use with object
		bool okToRemove() {
			if (duration == 0) {
				return false; // no time out ever, or we have not started yet
			}
			// example: ElapsedTime = 100, start = 50, wait = 100, duration 10 is (100-(50-100) > 60 is false 
			// example: ElapsedTime = 500, start = 50, wait = 100, duration 10 is (500-(50-100) > 60 is true 
			return (ofGetElapsedTimef() - (start - wait)) > start + duration;
		}
		void pause() {
			float elapsed = ofGetElapsedTimef();
			// if beyond wait time 
			// else hold wait time even after pause
			if (elapsed - (start + wait) > 0) {
				wait = 0; // ignore wait time upon return
			}
			paused = true;
		}
		void startReadHead() {
			start = ofGetElapsedTimef(); // set a base line of time
		}

		void play() {
			paused = false;
			startReadHead();
		}
		bool okToDraw() {
			if (paused) {
				return false;
			}
			float elapsed = ofGetElapsedTimef();
			// example: ElapsedTime = 100, start = 50, wait = 100, duration 10
			if (elapsed - (start + wait) > 0) {
				if (duration == 0) {
					return true; // draw away
				}
				// ok to start but only if we are less than duration
				return (elapsed < start + wait + duration);
			}
			return false;
		}

		GraphicID id() { return myID; }
		int getWidth() { return width; }
		int getHeight() { return height; }
		string &getLocation() { return locationPath; }
		float  getVolume() { return volume; }

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
		ofParameter<float> volume;
		float start;
		bool paused;
	private:
		GraphicID myID;// every graphic item gets a unique ID for deletion and etc
	};
	// with or w/o font
	class Text : public Graphic {
	public:
		Text() : Graphic() { str = "default";  }
		Text(const string&textIn) : Graphic() { str = textIn;  }
		bool read(const Json::Value &data);
		TextToRender &getPlayer(float wait = 0) {
			addWait(wait);
			return player;
		}

		string& getText() { return str; }

	private:
		string str;
		TextToRender player;
	};
	class Paragraph : public Graphic {
	public:
		Paragraph();
		bool read(const Json::Value &data);
		ofxParagraph &getPlayer(float wait = 0) {
			addWait(wait);
			return paragraph;
		}
		string& getText() { return str; }

	private:
		string str;
		ofxParagraph paragraph;
	};


	class Image : public Graphic {
	public:
	protected:

	};

	// audio gets an x,y,z which can be ignored for now but maybe surround sound will use these for depth
	class Audio : public Graphic {
	public:
		bool read(const Json::Value &data);
		ofSoundPlayer &getPlayer(float wait = 0) {
			addWait(wait);
			return player;
		}
	private:
		ofSoundPlayer  player;
	};
	
	class Video : public Graphic {
	public:
		bool read(const Json::Value &data);
		
		ofVideoPlayer& getPlayer(float wait=0) { 
			addWait(wait); 
			return player; 
		}
	private:
		ofVideoPlayer player;
	};

	// 3d, 2d, talking, movment, etc will get complicated but put in basics for now
	class Character : public Graphic {
	public:
		Character() : Graphic(){
			type = "2d";
		}
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			logVerbose(STRINGIFY(Character));

			Graphic::trace();
		}

#endif // _DEBUG

	};

	// item in a play list
	class PlayItem {
	public:
		PlayItem() {}
		PlayItem(const string&keynameIn) { keyname = keynameIn; }
		bool read(const Json::Value &data);
		bool operator==(const PlayItem& rhs) { return rhs.keyname == keyname; }
		string &getKeyName() {return keyname;}
	private:
		string keyname;
	};
	class Playlist  {
	public:
		bool read(const Json::Value &data);
		vector<PlayItem>& plays() { return playList; }
		void remove(const PlayItem &item) {
			// remove by name
			playList.erase(std::remove(playList.begin(), playList.end(), item), playList.end());
		}
	private:
		vector<PlayItem> playList;
	};

	class SettingsAndTitle {
	public:
		SettingsAndTitle() {
		}
		SettingsAndTitle(const string& titleIn) {
			title = titleIn;
		}
		SettingsAndTitle(const Settings& defaultsIn, const string& titleIn) {
			title = titleIn;
			settings = defaultsIn;
		}
		SettingsAndTitle(const Settings& defaultsIn) {
			settings = defaultsIn;
		}
		void operator=(const Settings& rhs) {
			settings = rhs;
		}
		void setDefaults(const Settings& rhs) {
			settings = rhs;
		}
		string &getTitle() { return title; }
		bool read(const Json::Value &data) {
			// dumps too much so only enable if there is a bug: ECHOAll(data);
			settings.read(data[settings.JsonName]);
			READSTRING(title, data);
			return true;
		}
		Settings& getSettings() {
			return settings;
		}

	private:
		Settings settings; // every object can have its own  defaults that derives from this object
		string title; // title of deck, slide etc. 
	};

	class VideoEngine {
	public:
		VideoEngine() {
			videos = nullptr;
		}
		VideoEngine(shared_ptr<vector<Video>> vidoesIn) {
			set(vidoesIn);
		}
		void set(shared_ptr<vector<Video>> videosIn=nullptr) {
			if (videosIn == nullptr) {
				videos = std::make_shared<vector<Video>>();
			}
			else {
				videos = videosIn;
			}
		}
		shared_ptr<vector<Video>> get() {
			if (videos == nullptr) {
				set();
			}
			return videos; 
		}
		void add(Video& player, float wait=0);
		void setup(float wait = 0);
	private:
		shared_ptr<vector<Video>> videos; // allow for sharing of this data
	};
	class GraphicEngines {
	public:
		VideoEngine videos;
	};
	class Scene : public SettingsAndTitle {
	public:
		Scene(const string&keynameIn) : SettingsAndTitle() {
			keyname = keynameIn;
			engines = nullptr;
		}
		Scene() : SettingsAndTitle() {
			engines = nullptr;
		}
		Scene(const Settings& defaults) : SettingsAndTitle(defaults) {
			engines = nullptr;
		}
		void setEngine(shared_ptr<GraphicEngines> enginesIn = nullptr) {
			if (enginesIn == nullptr) {
				engines = std::make_shared<GraphicEngines>();
			}
			else {
				engines = enginesIn;
			}
		}
		shared_ptr<GraphicEngines> getEngine() {
			if (engines == nullptr) {
				setEngine();
			}
			return engines;
		}

		bool operator==(const Scene& rhs) { return rhs.keyname == keyname; }
		bool read(const Json::Value &data);
		template<typename T, typename T2> void createTimeLineItems(T2& vec, const Json::Value &data, const string& key);
		string &getKey() { return keyname; }
		void setVideos(shared_ptr<vector<Video>> v) {
			getEngine()->videos.set(v);
		}
		vector <Paragraph>& getParagraphs() {	return paragraphs;}
		vector <Text>& getTexts() { return texts; }
		vector <Audio>& getAudio() { return audios; }
		shared_ptr<vector<Video>> getVideo() { return getEngine()->videos.get(); }
		vector <Character>& getCharacters() { return characters; }
		vector <Image>& getImages() { return images; }
		vector <Graphic>& getGraphics() { return graphics; }

		bool dataAvailable();

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Scene));

			traceVector(texts);
			traceVector(audios);
			traceVector(*getEngine()->videos.get());
			traceVector(paragraphs);
			traceVector(images);
			traceVector(graphics);
			traceVector(characters);
		}

#endif // _DEBUG

	protected:
		shared_ptr<GraphicEngines> engines;
		vector <Audio> audios; // join with ofaudio
		vector <Character> characters; // join with vector <Model3D> models;
		vector <Image> images; //bugbug join with ofImage vector <ofImage> images;
		vector <Graphic> graphics; // tie to ofX
		vector <Paragraph>  paragraphs; 
		vector <Text>  texts;
		string keyname;
	private:
	};

	// an Act is one json file that contains 1 or more scenes
	class Act : public SettingsAndTitle {
	public:
		Act(const Settings& defaults, const string& title) : SettingsAndTitle(defaults, title) {}

		// read a deck from json (you can also just build one in code)
		bool read(const string& fileName = "json.json");

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Act));
			traceVector(scenes);
		}
#endif
		bool dataAvailable();
		void add(const Scene &scene) {
			scenes.push_back(scene);
		}
		void remove(const Scene &scene) {
			// remove by name
			scenes.erase(std::remove(scenes.begin(), scenes.end(), scene), scenes.end());
		}
		vector <Scene>& getScenes() { return scenes; }
		Playlist &getPlayList() {
			return playlist;
		};
		void setEngine(shared_ptr<GraphicEngines> e) {
			engines = e;
		}
	private:
		vector <Scene> scenes;
		Playlist playlist;
		shared_ptr<GraphicEngines> engines;

	};
	
	// an app can run many Stories
	// a Story is a collection of acts
	class Story :public SettingsAndTitle {
	public:
		// readJsonValue our own defaults
		Story(const Settings& defaults, const string& title) : SettingsAndTitle(defaults, title) {
			engines = nullptr;
		}

		// get defaults later
		Story() : SettingsAndTitle() {
			engines = nullptr;
		}

		vector<Act>& getActs() {
			return acts;
		}
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void Story::trace();
#endif
		void setEngines(shared_ptr<GraphicEngines> v) {
			engines = v;
		}

		void read(const string& path, const string& title="<title>");

	private:
		vector<Act> acts;
		shared_ptr<GraphicEngines> engines;
	};
}