#pragma once

#include "2552software.h"
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
		bool read(const Json::Value &data);
	};
	template <class T> class ColorBase {
	public:
		ColorBase() {}
		ColorBase(const T& colorIn) {
			color = colorIn;
		}
		bool read(const Json::Value &data) {
			readJsonValue(color.r, data["r"]);
			readJsonValue(color.g, data["g"]);
			readJsonValue(color.g, data["b"]);
			return true;
		}
		operator T() {
			return color;
		}
		T& get() { return color; }
	protected:
		T color;
	};
	// matching colors, this class controls colors, no data or others selected colors
	// this way its easier to manage things
	//http://openframeworks.cc/documentation/types/ofColor/#show_fromHsb
	// there is a lot we can do
	class ColorSet {
	public:
		enum ColorType {
			Warm, Cool,Stark, Pastel, LightValue, DarkValue, MediumValue, Random
		};

		ColorSet() {
			set(Stark, ofColor(0, 0, 0), ofColor(255, 255, 255), ofColor(0, 0, 0));
		}
		ColorSet(const ColorType type, const ofColor&foreground, const ofColor&background, const ofColor&text) {
			set(type, foreground, background, text);
		}
		void set(const ColorType type, const ofColor&foreground, const ofColor&background, const ofColor&text) {
			state = std::make_tuple(type, 0, foreground, background, text);
		}
		ColorType getType() const {
			return get<0>(state);
		}
		int getCount() const {
			return get<1>(state);
		}
		const ofColor& getForeground() const {
			return get<2>(state);
		}
		const ofColor& getBackground() const {
			return get<3>(state);
		}
		const ofColor& getFontColor() const {
			return get<4>(state);
		}
		void operator++ () {
			std::get<1>(state) = std::get<1>(state) + 1;
		}
		bool operator> (const ColorSet& rhs) {
			return getCount() > rhs.getCount();
		}
		// return true if less than, and both of the desired type or Random
		bool lessThan(const ColorSet& j, ColorType type) {
			if (type != Random && getType() != j.getType()) {
				return false;
			}
			return *this > j; 
		}
		ColorSet& operator=(const ColorSet& rhs) {
			state = rhs.state;
			return *this;
		}
	private:
		// never want to seperate or we will not match
		// where does value come in? maybe convert from color to hue, sat/val?
		tuple<ColorType, int, ofColor, ofColor, ofColor> state;
	};
	class Colors {
	public:
		Colors() {
			setup(); 
		}
		// call getNext at start up and when ever colors should change
		const ColorSet& get() {
			if (smallest < 0){
				getNext();
			}
			++data[smallest];
			return data[smallest];
		}
		// do not break colors up or thins will not match
		const ofColor& getForeground() { return get().getForeground(); }
		const ofColor& getFontColor() { return get().getFontColor(); }
		const ofColor& getBackground() { return get().getBackground(); }
		// bugbug enable this in json?
		void useWarms() {

		}
		void useCools() {

		}
		void useLights() {

		}
		void useDarks() {

		}

		// get next color based on type and usage count
		// example: type==cool gets the next cool type, type=Random gets any next color
		void getNext(ColorSet::ColorType type= ColorSet::Warm) {
			// find smallest of type
			smallest = 0; // code assume some data loaded
			for (int i = 0; i < data.size(); ++i) {
				if (data[smallest].lessThan(data[i], type)) {
					smallest = i;
				}
			}
			//std::vector<ColorSet>::iterator result = std::min_element(std::begin(data), std::end(data));
			//ColorSet cs = *std::min_element(data.begin(), data.end() - 1, ColorSet::searchfn);
			//if (result != data.end()) {
				//return *result;
			//}
		}
	private:
		// foreground, background, font
		vector<ColorSet> data;
		int smallest;//index of smallest value
		//vector <ColorSet> warm;
		// make a bunch of colors that match using various techniques
		void setup() {
			smallest = -1;//index of smallest value
			ColorSet cs = ColorSet(ColorSet::Warm,
					ofColor(255, 0, 0), ofColor(0, 255, 0),ofColor(0, 0, 255));
			data.push_back(cs);

			ofColor fore, back, text;
			fore.fromHsb(200, 100, 40); // just made this up for now
			back.fromHsb(100, 100, 50);
			text.fromHsb(200, 100, 100);
			cs.set(ColorSet::Warm, fore, back, text);

//			ColorSet cs2 = ColorSet(ColorSet::Warm,
	//			ofColor::aliceBlue, ofColor::crimson, ofColor::antiqueWhite);
			ColorSet cs2 = ColorSet(ColorSet::Warm,
				ofColor(0, 255, 0), ofColor(100, 255, 0), ofColor(255, 255, 255));
			data.push_back(cs2);

		}
	};
	Colors& getSharedColors();
	const ColorSet& getCurrentColors();
	void nextColor(ColorSet::ColorType type = ColorSet::Warm);
	// color support of explict color is needed
	class Color : public ColorBase<ofColor> {
	public:
		Color() :ColorBase() {
		}
		Color(const ofColor& color) :ColorBase(color) {
		}
	};
	class floatColor : public ColorBase<ofFloatColor> {
	public:
		floatColor() :ColorBase() {
		}
		floatColor(const ofFloatColor& color):ColorBase(color){
		}

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

	class Font  { 
	public:
		ofTrueTypeFont& get() { 
			if (font == nullptr) {
				getPointer();
			}
			if (font != nullptr) {
				return font->ttf;
			}
			ofTrueTypeFont font2;
			return font2; // default
		}
		shared_ptr<ofxSmartFont> getPointer() {
			if (font == nullptr) {
				font = ofxSmartFont::get(defaultFontFile, defaultFontSize);
				if (font == nullptr) {
					// name is not unqiue, just a helper of some kind I guess
					font = ofxSmartFont::add(defaultFontFile, defaultFontSize, defaultFontName);
				}
			}
			if (font == nullptr) {
				logErrorString("font is null");
			}
			return font;
		}
		bool read(const Json::Value &data);
	private:
		shared_ptr<ofxSmartFont> font;
	};

	class TheSet {
	public:
		bool read(const Json::Value &data) {
			floatColor colorAmbient(light.getAmbientColor());
			colorAmbient.read(data["ambientColor"]);
			light.setAmbientColor(colorAmbient.get());

			floatColor colorDiffuse(light.getDiffuseColor());
			colorAmbient.read(data["diffuseColor"]);
			light.setDiffuseColor(colorDiffuse.get());
			return true;
		}

		/// use pointers if data set gets too large

		ofLight	light;
		ofEasyCam camera;
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
		bool read(const Json::Value &data);

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
		bool read(const Json::Value &data);
	protected:
		DateAndTime timelineDate; // date item existed
		DateAndTime lastUpdateDate; // last time object was updated
		DateAndTime itemDate; // date of reference vs the date of the referenced item 

	};
	// reference to a cited item
	class Reference : public Dates {
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
	class ReferencedItem : public Dates {
	public:
		bool read(const Json::Value &data);
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
	class Graphic : public ReferencedItem {
	public:
		Graphic();

		bool read(const Json::Value &data);

		// for use in remove_if
		static bool staticOKToRemove(shared_ptr<Graphic> me);
		virtual void pause();
		void startReadHead();
		virtual void play();
		virtual void stop() {	}
		bool okToDraw();
		virtual void setup() {}
		virtual void draw() {}
		virtual void update() {}
		
		GraphicID id() { return myID; }
		int getWidth() { return width; }
		int getHeight() { return height; }
		string &getLocation() { return locationPath; }
		float  getVolume() { return volume; }
		virtual void getTimeBeforeStart(float& f) {
			setIfGreater(f, getDuration() + getWait());
		}
		float getDuration() { return duration; }
		float getWait() { return wait; }
		void setWait(float waitIn) { wait = waitIn; }
		void addWait(float waitIn) { wait += waitIn; }
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
		float start;
		bool paused;
		float  duration; // life time of object, 0 means forever
		float  wait;     // time to wait before drawing
		Point3D startingPoint; // starting point of object for drawing

	private:
		GraphicID myID;// every graphic item gets a unique ID for deletion and etc
	};

	// an actor if you will
	template <class T> class ThePlayer : public Graphic
	{
	public:
		virtual bool read(const Json::Value &data) = 0;
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

	// some objects just use the OpenFrameworks objects if things are basic enough
	class Image : public ThePlayer<ofImage> {
	public:
		bool read(const Json::Value &data);
		void draw() {
			player.draw(getStartingPoint().x, getStartingPoint().y);
		}
		void update() {
			player.update();
		};
	};
	
	class Particles : public ThePlayer<ParticlesEngine> {
	public:

		bool read(const Json::Value &data);
		void draw() {
			player.draw(this);
		}
		void setup() {
			player.setup(this);
		};
		void update() {
			player.update(this);
		};
	private:
	};

	class Text : public ThePlayer<TextEngine> {
	public:
		bool read(const Json::Value &data);
		void draw() {
			player.draw(this);
		}
		void update() {
			player.update(this);
		};
		string& getText() { return text; }
	private:
		string text;
	};

	class Paragraph : public ThePlayer<ofxParagraph> {
	public:

		bool read(const Json::Value &data);
		void draw() {
			player.draw(getStartingPoint().x, getStartingPoint().y);
		}
	};


	// audio gets an x,y,z which can be ignored for now but maybe surround sound will use these for depth
	class Audio : public ThePlayer<ofSoundPlayer> {
	public:
		bool read(const Json::Value &data);
		void setup();
	};

	class Video : public ThePlayer<VideoEngine> {
	public:

		bool read(const Json::Value &data);
		void setup();
		void update() {
			player.update();
		};
		void draw() {
			player.draw(this);
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
		virtual float getTimeBeforeStart(float f = 0);
		string test;
	};

	// 3d, 2d, talking, movment, etc will get complicated but put in basics for now
	class Character : public ThePlayer<CharacterEngine> {
	public:

		bool read(const Json::Value &data);
	private:
		// player is just a string object
	};

	class Scene;
	class GraphicEngines {
	public:

		void bumpWait(float wait) {
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
		float findMaxWait() {
			float f = 0;
			for (auto& t : get()) {
				setIfGreater(f, t->getDuration() + t->getWait());
			}
			return f;
		}
		// remove items that are timed out
		void removeExpiredItems() {
			get().erase(std::remove_if(get().begin(), get().end(),
				Graphic::staticOKToRemove), get().end());
		}

		void cleanup() {
			removeExpiredItems();
		}
		bool dataAvailable();
		void bumpWaits(float wait);
		float getLongestWaitTime();
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