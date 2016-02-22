#pragma once

#include "2552software.h"
#include "ofxSmartFont.h"
#include "ofxJSON.h"
#include "kinect2552.h"
#include "ofxParagraph.h"

// timeline software, json based

namespace Software2552 {
	// helpers

	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data); 

#if _DEBUG
	template<typename T> void traceVector(T& vec);
#endif // _DEBUG

	template<typename T> void setupVector(T& vec);
	template<typename T> void updateVector(T& vec);
	template<typename T> void drawVector(T& vec);

	// set only if value in json
	inline void set(string &value, const Json::Value& data) {
		if (!data.empty()) { 
			value = data.asString();
		}
	}
	inline void set(float &value, const Json::Value &data) {
		if (!data.empty()) { // not an  array and there is data
			value = data.asFloat();
		}
	}
	inline void set(int &value, const Json::Value &data) {
		if (!data.empty()) { // not an  array and there is data
			value = data.asInt();
		}
	}

// will only set vars if there is a string to set, ok to preserve existing values
#define READ(var, data) set(var, data[#var])


	// match string to font
	class  ofxSmartText
	{
	public:
		ofxSmartText() { f = nullptr; };
		ofxSmartText(const string& text, shared_ptr<ofxSmartFont> font) {
			set(text, font);
		};

		void set(const string& text, shared_ptr<ofxSmartFont> font) {
			set(text);
			set(font);
		}
		void set(const string& text) {
			s = text;
		}
		void set(shared_ptr<ofxSmartFont> font) {
			f = font;
		}
		shared_ptr<ofxSmartFont> getFont() {
			return f;
		}
		void draw(int x, int y) {
			if (getFont()) {
				getFont()->draw(getText(), x, y);
			}
		}

		string getText() {
			return s;
		}
		int getWidth() {
			if (f == nullptr)
				return 0;

			return f->width(s);
		}
		int getHeight() {
			if (f == nullptr)
				return 0; // not sure what to do here, maybe just make sure it never happens

			return f->height(s);
		}

	private:
		string s;
		shared_ptr<ofxSmartFont> f;
	};

	// set defaults bugbug get to our tracing base class
	class Paragraph : public ofxParagraph {
	public:
		
		//align right blows up on at least short strings
		Paragraph(string titleIn, string text, int maxWidth, int y = 0, Alignment align = ALIGN_LEFT) : ofxParagraph(text, 0, align) {
			// fonts are shared via static bugbug maybe ask before calling to avoid log on it?
#if 0
			setText(ofxSmartText(text, ofxSmartFont::add("data/Raleway-Thin.ttf", 14, "raleway-thin")));
			if (titleIn.length() > 0) {
				setTitle(ofxSmartText(titleIn, ofxSmartFont::add("data/Raleway-Italic.ttf", 18, "raleway-italic")));
			}
			if (y == 0) {
				set(maxWidth, title.getHeight() * 3); // default
			}
			else {
				set(maxWidth, y); // assume height is set else where
			}
#endif // 0

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
		void setTitle(const ofxParagraph & titleIn) {
			title = titleIn;
		}
		void setText(ofxSmartText & text) {
			ofxParagraph::setText(text.getText());
			ofxParagraph::setFont(text.getFont());
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
			font = nullptr;
		}
		Settings(const Settings&In) {
			font = In.font;
		}
		const int defaultFontSize = 14;

		bool read(const Json::Value &data);

		shared_ptr<ofxSmartFont> getFont() {
			if (font == nullptr) {
				font = make_shared<ofxSmartFont>();
				font->add("fonts/Raleway-Thin.ttf", defaultFontSize, "Raleway-Thin");
			}
			return font;
		}
	protected:
		shared_ptr<ofxSmartFont> font;
	private:
	};

	class Tools {
	public:
		Tools() {
		}
		ofLight & getLight() { return light; }
#if _DEBUGDefaultTools
		// echo object (debug only)
		void trace() {
		}
#endif
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
			settings = defaultsIn; // start with common defaults for example
			sharedTools = tools;
		}
		bool operator==(const Timeline& rhs) { return rhs.title == title; }
		string &getTitle() { return title; }

		// read in my defaults and title
		bool read(const Json::Value &data) {
			settings.read(data);
			READ(title, data);
			return true;
		}
		// combine my and my parents defaults
		Settings& getSettings() {
			return settings;
		}
		shared_ptr<Tools> getSharedTools() {
			return sharedTools;
		}

	private:
		Settings settings; // every object can have its own  defaults that derives from this object
		shared_ptr<Tools> sharedTools; 
		string title; // title of deck, slide etc. Must be unique
	};

	class CommonData  {
	public:
		CommonData() {
		}
		CommonData(const string&nameIn) {
			name = nameIn;
		}
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(CommonData));
			basicTrace(date);
			basicTrace(timelineDate);
			basicTrace(lastUpdateDate);
			basicTrace(name);
			basicTrace(notes);
		}
#endif
		bool operator==(const CommonData& rhs) { return rhs.name == name; }
		string &getName() { return name; }
		bool read(const Json::Value &data);

	protected:
		string timelineDate; // date item existed
		string lastUpdateDate; // last time object was updated
		string name; // any object can have a name, note, date, reference, duration
		string notes;
		string date; // bugbug make this a date data type
	};

	// reference to a cited item
	class Reference : public CommonData {
	public:
		bool read(const Json::Value &data);

		// echo object (debug only)
#if _DEBUG
		void trace() {
			basicTrace(STRINGIFY(Reference));
			CommonData::trace();
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
	class TimeLineBaseClass : public CommonData {
	public:
		TimeLineBaseClass() : CommonData() {};
		TimeLineBaseClass(const string &name) :CommonData(name) {};
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(TimeLineBaseClass));
			CommonData::trace();
			
			for (auto& ref : references) {
				ref.trace();
			}
		}

#endif // _DEBUG

		bool read(const Json::Value &data);
		vector <Reference> references;
		
	};

	// basic graphic like SUN etc to add flavor
	class Graphic : public TimeLineBaseClass {
	public:
		Graphic() {
			x = y = z = 0;
			duration = 0; // infinite by default
			start = 0; // force reset to be called to make sure timing is right
			delay = 0;
			started = false;
		}
		
		static bool okToRemove(const Graphic& s) {
			if (!s.duration || !s.started) {
				return false; // no time out ever, or we have not started yet
			}
			return (ofGetElapsedTimef() - (s.start+ s.delay)) > (s.duration + s.delay);
		};
		
		bool read(const Json::Value &data);

		void setup() {
			start = ofGetElapsedTimef();
			start += delay;
		}
		void update() {
		}
		bool okToDraw() {
			if (duration == 0) {
				return true; // always draw
			}
			if (start <= 0) {
				return false; // not started yet
			}
			if (start + delay < ofGetElapsedTimef()) {
				return false; // not started yet
			}
			// still going??
			return start+delay+duration < ofGetElapsedTimef();
		}
		void draw() {
		}
#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Graphic));
			TimeLineBaseClass::trace();
			basicTrace(type);
			basicTrace(ofToString(duration));
			basicTrace(ofToString(delay));
			basicTrace(ofToString(x));
			basicTrace(ofToString(y));
			basicTrace(ofToString(z));
			basicTrace(foreground);
			basicTrace(background);
		}
#endif // _DEBUG
	protected:
		int    x, y, z;
		string type; // 2d, 3d, other
		string foreground;
		string background;
		float duration;
		float start;
		float delay; // start+delay is the true start
		bool started;
	};

	class Text : public Graphic {
	public:
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Text));
			Graphic::trace();
			// ofxParagraph does not expose a bunch of stuff that we can echo here, not a big deal
		}
#endif // _DEBUG
	protected:
		ofxParagraph paragraph;
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

	class Scene : public Timeline {
	public:
		Scene(const string& title) : Timeline(title) {}
		Scene(const Settings& defaults, shared_ptr<Tools> tools, const string& title) : Timeline(defaults, tools, title) {}

		void setup();
		void update();
		void draw();

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

		bool read(const Json::Value &data);

	protected:
		vector <Audio> audios; // join with ofaudio
		vector <Video> videos; // join wiht ofvideo
		vector <Character> characters; // join with vector <Model3D> models;
		vector <Image> images; //bugbug join with ofImage vector <ofImage> images;
		vector <Graphic> graphics; // tie to ofX
		vector <Text>  texts; //bugbug join Text and Paragraph vector<Paragraph> paragraphs;

	};

	class Scenes : public Timeline {
	public:
		Scenes(const Settings& defaults, shared_ptr<Tools> tools, const string& title) : Timeline(defaults, tools, title) {}

		// read a deck from json (you can also just build one in code)
		bool read(const string& fileName = "json.json");

		void setup() {
			setupVector(scenes);
		}
		void update() {
			updateVector(scenes);
		}
		void draw() {
			setupVector(scenes);
		}
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			basicTrace(STRINGIFY(Scenes));
			for (auto& scene : scenes) {
				scene.trace();
			}
		}
#endif
		void add(const Scene &s) {
			scenes.push_back(s);
		}
		void remove(const Scene &slide) {
			// remove by name
			scenes.erase(std::remove(scenes.begin(), scenes.end(), slide), scenes.end());
		}
		vector <Scene>& getScenes() { return scenes; }

	private:
		vector <Scene> scenes;
		
	};
	
	// an app can run many Stories
	class Story :public Timeline {
	public:
		// set our own defaults
		Story(const Settings& defaults, shared_ptr<Tools> tools, const string& title) : Timeline(defaults, tools, title) {}

		// get defaults later
		Story() : Timeline() {}

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void Story::trace();
#endif

		void setup() {
			read();
			setupVector(story);
		};
		void update() {
			updateVector(story);
		}
		void draw() {
			drawVector(story);
		}

	private:
		vector<Scenes> story;
		void read() {
			Scenes scenes(getSettings(), getSharedTools(), "main deck");
			// code in the list of items to make into the story here. 
			scenes.read("json.json");
			story.push_back(scenes);
		}

	};
}