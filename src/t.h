#pragma once

#include "2552software.h"
#include "ofxSmartFont.h"
#include "ofxJSON.h"
#include "kinect2552.h"
#include "ofxParagraph.h"
#include "Poco/Foundation.h"
#include "Poco/DateTime.h"
#include "Poco/LocalDateTime.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormatter.h"

// timeline software, json based

namespace Software2552 {
	// helpers
	void echoValue(const Json::Value &data, bool isError = false);
	bool echoJSONTree(const string& functionname, const Json::Value &root, bool isError=false);
#define ECHOAll(data) echoJSONTree(__FUNCTION__, data);
#define ERROR_ECHOAll(data) echoJSONTree(__FUNCTION__, data, true);

	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data); 

#if _DEBUG
	template<typename T> void traceVector(T& vec);
#endif // _DEBUG

	template<typename T> void setupVector(T& vec);
	template<typename T> void updateVector(T& vec);
	template<typename T> void drawVector(T& vec);

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
			basicTrace(STRINGIFY(Point3D));
			basicTrace(ofToString(x));
			basicTrace(ofToString(y));
			basicTrace(ofToString(z));
		}
#endif // _DEBUG

	};
	class Color : public ofColor {
	public:
		bool read(const Json::Value &data);
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(RGB));
			basicTrace(ofToString(r));
			basicTrace(ofToString(g));
			basicTrace(ofToString(b));
		}
#endif // _DEBUG

	};

	//http://pocoproject.org/slides/070-DateAndTime.pdf
	class DateAndTime {
	public:
		DateAndTime() {
			timeZoneDifferential = 0;
			bc = 0;
		}
		void operator=(const DateAndTime& rhs) {
			timeZoneDifferential = rhs.timeZoneDifferential;
			datetime = rhs.datetime;
			bc = rhs.bc;
		}
		string getDate() {

		}
		bool read(const Json::Value &data);
		const string format = "%dd %H:%M:%S.%i";
		Poco::DateTime datetime;
		int timeZoneDifferential; 
		int bc; // non zero if its a bc date
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(DateAndTime));
			if (bc) {
				basicTrace("BC");
				basicTrace(ofToString(bc));
			}
			else {
				basicTrace(Poco::DateTimeFormatter::format(datetime, format));
				basicTrace(ofToString(timeZoneDifferential));
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
			basicTrace(STRINGIFY(Font));
			if (font != nullptr) {
				basicTrace(font->name());
				basicTrace(font->file());
				basicTrace(ofToString(font->size()));
			}
			else {
				basicTrace("no font");
			}
		}
#endif // _DEBUG

	};
	// readJsonValue defaults bugbug get to our tracing base class
	class Paragraph : public ofxParagraph {
	public:
		
		//align right blows up on at least short strings
		Paragraph(string titleIn, string text, int maxWidth, int y = 0, Alignment align = ALIGN_LEFT) : ofxParagraph(text, 0, align) {
			// fonts are shared via static bugbug maybe ask before calling to avoid log on it?
		};

		// enables easy lay out of paragraphs
		int nextRow() {
			return y + title.getHeight() + getHeight();
		}

		void draw() {
			ofxParagraph::draw();
			int titleX = x + (getWidth() / 2 - title.getWidth() / 2);
			if (titleX < 0)
				titleX = title.getWidth() / 2;//this is wrong bugbug
			int titleY = title.getHeight(); // create some sort of offset
			if (titleY < 0)
				titleY = title.getHeight();//this is wrong bugbug

			title.draw(titleX, titleY);
		}

	private:
		void set(int maxWidth, int yIn) {
			setWidth((2 * maxWidth) / 3);
			x = maxWidth / 2 - getWidth() / 2;
			y = yIn;
		}
		ofxParagraph title; // font for the title bugbug not sure where to delete this yet
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
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Settings));
			foregroundColor.trace();
			backgroundColor.trace();
			startingPoint.trace();
			font.trace();
			timelineDate.trace(); // date item existed
			lastUpdateDate.trace(); // last time object was updated
			itemDate.trace();

			basicTrace(name);
			basicTrace(notes);
			basicTrace(ofToString(duration));
		}
#endif

	protected:
		Font   font;
		// when read build date class bugbug
		DateAndTime timelineDate; // date item existed
		DateAndTime lastUpdateDate; // last time object was updated
		DateAndTime itemDate; // date of reference vs the date of the referenced item 
		string name; // any object can have a name, note, date, reference, duration
		string notes;
		Color  foregroundColor;
		Color  backgroundColor;
		float  duration; 
		Point3D startingPoint;
	private:
		void init() {
			Poco::Timespan totalTime = 1 * 1000 * 1000;
			duration = 0;
			foregroundColor.set(0, 0, 255);
			backgroundColor.set(255, 255, 0);
		}
	};

	// drawing tools etc, shared across objects
	class Tools {
	public:
		Tools() {
		}
		ofLight & getLight() { return light; }
	protected:
		ofLight	 light;
		ofCamera camera;
	};

	

	class Timeline {
	public:
		Timeline() {
		}
		Timeline(const string& titleIn) {
			title = titleIn;
		}
		Timeline(const Settings& defaultsIn, shared_ptr<Tools> tools, const string& titleIn) {
			title = titleIn;
			sharedTools = tools;
			settings = defaultsIn;
		}
		Timeline(const Settings& defaultsIn, shared_ptr<Tools> tools) {
			sharedTools = tools;
			settings = defaultsIn;
		}
		bool operator==(const Timeline& rhs) { return rhs.title == title; }
		void operator=(const Settings& rhs) {
			settings = rhs;
		}
		void setDefaults(const Settings& rhs) {
			settings = rhs;
		}
		string &getTitle() { return title; }

		// read in my defaults and title
		bool read(const Json::Value &data);

		Settings& getSettings() {
			return settings;
		}
		shared_ptr<Tools> getSharedTools() {
			return sharedTools;
		}

	private:
		Settings settings; // every object can have its own  defaults that derives from this object
		shared_ptr<Tools> sharedTools; 
		string title; // title of deck, slide etc. 
	};

	// reference to a cited item
	class Reference : public Settings {
	public:
		bool read(const Json::Value &data);

		// echo object (debug only)
#if _DEBUG
		void trace() {
			basicTrace(STRINGIFY(Reference));
			Settings::trace();
			basicTrace(url);
			basicTrace(location);
			basicTrace(source);
		}
#endif
	protected:
		string url; // can be local too
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
			basicTrace(STRINGIFY(ReferencedItem));
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
		Graphic() : ReferencedItem(){
			//bugbug add a pause where time is suspended, add in rew, play, stop etc also
			start = 0; // force reset to be called to make sure timing is right, 0 means not started
			delay = 0;
		}
		static bool okToRemove(const Graphic& s) {
			if (!s.duration || s.start < 0) {
				return false; // no time out ever, or we have not started yet
			}
			//bugbug delay not coded in
			return (ofGetElapsedTimef() - (s.start)) > (s.duration);
		};
		
		bool read(const Json::Value &data);

		void setup() {
			start = ofGetElapsedTimef();
		}
		void update() {
		}
		bool okToDraw();
		void draw() {
		}
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Graphic));
			ReferencedItem::trace();
			basicTrace(type);
			basicTrace(ofToString(delay));
		}
#endif // _DEBUG
	protected:
		string type; // 2d, 3d, other
		float start;
		float delay; // start+delay is the true start
		int width; //bugbug todo
		int height;//bugbug todo
	};

	class Text : public Graphic {
	public:
		Text() :Graphic() {}

		bool read(const Json::Value &data);

		void draw() {
			if (okToDraw()) {
				text.draw(getStartingPoint().x, getStartingPoint().y);
			}
		};

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Text));
			Graphic::trace();
			// ofxParagraph does not expose a bunch of stuff that we can echo here, not a big deal
		}
#endif // _DEBUG
	protected:
		ofxParagraph text;
	};


	// 3d, 2d, talking, movment, etc will get complicated but put in basics for now
	class Character : public Graphic {
	public:
		Character() {
			type = "2d";
		}
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Character));

			Graphic::trace();
		}

#endif // _DEBUG

	};
	class Image : public Graphic {
	public:
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Image));

			Graphic::trace();
			basicTrace(url);
		}

#endif // _DEBUG
	protected:
		string url; // remote or local bugbug maybe make a better name?  not sure

	};

	// audio gets an x,y,z which can be ignored for now but maybe surround sound will use these for depth
	class Audio : public Image {
	public:
		Audio() {
			volume = 5; // 1/2 way
		}
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Audio));

			Image::trace();
			basicTrace(ofToString(volume));
		}

#endif // _DEBUG

		int    volume;
		
	};
	
	class Video : public Audio {
	public:
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Video));
			Audio::trace();
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
	private:
		vector<PlayItem> playList;
	};
	class Scene : public Timeline {
	public:
		Scene(const string&keynameIn) : Timeline() {
			keyname = keynameIn;
			wait = false;
		}
		Scene() : Timeline() {
			wait = false;
		}
		Scene(const Settings& defaults, shared_ptr<Tools> tools) : Timeline(defaults, tools) {
			wait = false;
		}
		bool operator==(const Scene& rhs) { return rhs.keyname == keyname; }
		bool read(const Json::Value &data);
		template<typename T, typename T2> void createTimeLineItems(T2& vec, const Json::Value &data);
		string &getKey() { return keyname; }
		void setup();
		void update();
		void draw();

		// should we wait on this sceen?
		bool waitOnScene() {
			return dataAvailable() && wait;
		}

		bool dataAvailable();

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			basicTrace(STRINGIFY(Scene));

			traceVector(audios);
			traceVector(videos);
			traceVector(texts);
			traceVector(images);
			traceVector(graphics);
			traceVector(characters);
		}

#endif // _DEBUG

	protected:
		vector <Audio> audios; // join with ofaudio
		vector <Video> videos; // join wiht ofvideo
		vector <Character> characters; // join with vector <Model3D> models;
		vector <Image> images; //bugbug join with ofImage vector <ofImage> images;
		vector <Graphic> graphics; // tie to ofX
		vector <Text>  texts; 
		string keyname;
		bool   wait;
	private:
	};

	class Scenes : public Timeline {
	public:
		Scenes(const Settings& defaults, shared_ptr<Tools> tools, const string& title) : Timeline(defaults, tools, title) {}

		// read a deck from json (you can also just build one in code)
		bool read(const string& fileName = "json.json");

		void setup();
		void update();
		void draw();
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			basicTrace(STRINGIFY(Scenes));
			traceVector(scenes);
		}
#endif
		bool dataAvailable();
		void add(const Scene &scene) {
			scenes.push_back(scene);
		}
		void removeExpiredScenes();
		void remove(const Scene &scene) {
			// remove by name
			scenes.erase(std::remove(scenes.begin(), scenes.end(), scene), scenes.end());
		}
		vector <Scene>& getScenes() { return scenes; }

	private:
		vector <Scene> scenes;
		Playlist playlist;
	};
	
	// an app can run many Stories
	class Story :public Timeline {
	public:
		// readJsonValue our own defaults
		Story(const Settings& defaults, shared_ptr<Tools> tools, const string& title) : Timeline(defaults, tools, title) {}

		// get defaults later
		Story() : Timeline() {}

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void Story::trace();
#endif

		void setup();
		void update();
		void draw();

	private:
		vector<Scenes> story;
		void read();

	};
}