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

	class Font {
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

	class TheSet {
	public:
		ofFloatColor readColor(ofFloatColor& color, const Json::Value &data) {
			readJsonValue(color.r, data["r"]);
			readJsonValue(color.g, data["g"]);
			readJsonValue(color.g, data["b"]);

			return color;
		}
		bool read(const Json::Value &data) {
			light.setAmbientColor(readColor(light.getAmbientColor(), data["ambientColor"]));
			light.setDiffuseColor(readColor(light.getDiffuseColor(), data["diffuseColor"]));
			return true;
		}

		/// use pointers if data set gets too large

		ofLight	light;
		ofEasyCam camera;
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
			//remove_if needs this, but we do not want to copy in that case setSettings(rhs);
		}
		bool read(const Json::Value &data);

		Point3D& getStartingPoint() { return startingPoint; }

		shared_ptr<ofxSmartFont> getFont() {
			return font.font;
		}
		bool operator==(const Settings& rhs) { return rhs.name == name; }
		string &getName() { return name; }
		const ofColor& getForeground() { return foregroundColor; }
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
		TheSet stageSet;
		string title; // title object

	private:
		void init() {
			Poco::Timespan totalTime = 1 * 1000 * 1000;
			duration = 0;
			wait = 0;
			foregroundColor.set(0, 0, 255);
			backgroundColor.set(255, 255, 0);
		}
		void setSettings(const Settings& rhs);

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

		// for use in remove_if
		static bool staticOKToRemove(Graphic& me) {
			// duration == 0 means never go away, and start == 0 means we have not started yet
			if (me.duration == 0 || me.start == 0) {
				return false; // no time out ever, or we have not started yet
			}
			float elapsed = ofGetElapsedTimef() - me.start;
			if (me.wait > elapsed || me.duration < elapsed) {
				return false;
			}
			return true; 

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
		Text() : Graphic() { str = "default"; }
		Text(const string&textIn) : Graphic() { str = textIn; }
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

		ofVideoPlayer& getPlayer() {
			//player.setPixelFormat(OF_PIXELS_NATIVE);
			return player;
		}
	private:
		ofVideoPlayer player;
	};

	// 3d, 2d, talking, movment, etc will get complicated but put in basics for now
	class Character : public Graphic {
	public:
		Character() : Graphic() {
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

	class Scene;
	class GraphicEngines {
	public:

		template<typename T> void bumpWait(T& v, float wait) {
			for (auto& t : v) {
				t.addWait(wait);
			}
		}
		template<typename T> float findMaxWait(T& v) {
			float f = 0;
			for (auto& t : v) {
				setIfGreater(f, t.getDuration() + t.getWait());
			}
			return f;
		}
		// remove items that are timed out
		template<typename T> void removeExpiredItems(T& v) {
			
			v.erase(std::remove_if(v.begin(), v.end(), Graphic::staticOKToRemove), v.end());
		}

		void setIfGreater(float& f1, float f2) {
			if (f2 > f1) {
				f1 = f2;
			}
		}
		void cleanup() {
			removeExpiredItems(videos); // make derived classes to do fancy things beyond the scope here
			removeExpiredItems(paragraphs);
			removeExpiredItems(texts);
			removeExpiredItems(audios);
			removeExpiredItems(images);
			removeExpiredItems(graphics);
			removeExpiredItems(characters);
		}
		bool dataAvailable();
		void setup(float wait = 0);
		void add(Scene&scene);
		void add(shared_ptr<GraphicEngines> e);
		void bumpWaits(float wait);
		float getLongestWaitTime();
		vector<Video> videos;
		vector <Audio> audios;
		vector<Paragraph> paragraphs;
		vector<Text> texts;
		vector<Image> images;
		vector<Graphic> graphics;
		vector<Character> characters;
	private:
	};

	class Scene : public Settings {
	public:
		Scene(const string&keynameIn) {
			keyname = keynameIn;
			engines = nullptr;
		}
		Scene() {
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

		bool operator==(const Scene& rhs) { return rhs.keyname == keyname; }
		bool read(const Json::Value &data);
		template<typename T, typename T2> void createTimeLineItems(T2& vec, const Json::Value &data, const string& key);
		string &getKey() { return keyname; }
		float getLongestWaitTime() { return getEngines()->getLongestWaitTime(); }
		void bumpWaits(float wait) { getEngines()->bumpWaits(wait); }
		bool dataAvailable() { return getEngines()->dataAvailable(); }
		void setup() { getEngines()->setup(); }
		void cleanup() { getEngines()->cleanup(); }
		vector <Video>& getVideo() { return getEngines()->videos; }
		vector <Paragraph>& getParagraphs() { return getEngines()->paragraphs; }
		vector <Text>& getTexts() { return getEngines()->texts; }
		vector <Audio>& getAudio() { return  getEngines()->audios; }
		vector <Character>& getCharacters() { return getEngines()->characters; }
		vector <Image>& getImages() { return getEngines()->images; }
		vector <Graphic>& getGraphics() { return getEngines()->graphics; }

		// we want folks to use wrappers to avoid tons of dependicies on this
		shared_ptr<GraphicEngines> getEngines() {
			if (engines == nullptr) {
				setEngine();
			}
			return engines;
		}

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Scene));

			traceVector(getEngines()->texts);
			traceVector(getEngines()->audios);
			traceVector(getEngines()->videos);
			traceVector(getEngines()->paragraphs);
			traceVector(getEngines()->images);
			traceVector(getEngines()->graphics);
			traceVector(getEngines()->characters);
		}

#endif // _DEBUG

	protected:
		shared_ptr<GraphicEngines> engines;
		string keyname;

	private:

	};
	// item in a play list
	class PlayItem {
	public:
		PlayItem() {}
		PlayItem(const string&keynameIn) { keyname = keynameIn; }
		bool read(const Json::Value &data);
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
		bool read(const Json::Value &data);
		vector<PlayItem>& getList() { return list; }
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Playlist));
			traceVector(getList());
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
		bool read(const string& fileName = "json.json");
		bool dataAvailable();
		vector<PlayItem> &getPlayList() { return playlist.getList(); };

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			logVerbose(STRINGIFY(Act));
			traceVector(getPlayList());
		}
#endif
	private:
		Playlist playlist;

	};

}