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
	class Paragraph2552 : public ofxParagraph {
	public:
		
		//align right blows up on at least short strings
		Paragraph2552(string titleIn, string text, int maxWidth, int y = 0, Alignment align = ALIGN_LEFT) : ofxParagraph(text, 0, align) {
			// fonts are shared via static bugbug verify this
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
		};

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
		void setTitle(const ofxSmartText & titleIn) {
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
		ofxSmartText title; // font for the title bugbug not sure where to delete this yet
	};

	// common to all classes
	class CommonData {
	public:
		CommonData() {
		}
		CommonData(const string&nameIn) {
			name = nameIn;
		}

		bool operator==(const CommonData& rhs) { return rhs.name == name; }
		string &getName() { return name; }
		bool read(const Json::Value &data);

		string timelineDate; // date item existed
		string lastUpdateDate; // last time object was updated
		string name; // any object can have a name, note, date, reference, duration
		string notes;
		string date; // bugbug make this a date data type
	};

	// reference to a cited item
	class Reference : public CommonData {
	public:
		void read(const Json::Value &data);

		string url; // can be local too
		string location;
		string source;
	};

	// true for all time based items
	class TimeBaseClass : public CommonData {
	public:
		TimeBaseClass() :CommonData(){
			duration = 0; // infinite by default
		}
		TimeBaseClass(const string&nameIn):CommonData(nameIn) {
			duration = 0; // infinite by default
		}
		
		bool read(const Json::Value &data);
		vector <Reference> references;
		float duration; 
	};

	class Text : public TimeBaseClass {
	public:
		void read(const Json::Value &data);
		
		string paragraph; //bugbug convert to ofxParagraph for here, font and size (not sure about size)
		string font;
		int size;
	};

	// 3d, 2d, talking, movment, etc will get complicated but put in basics for now
	class Character : public TimeBaseClass {
	public:
		Character() {
			x = y = z = 0;
			type = "2d";
		}
		void read(const Json::Value &data);
		string type; // 2d, 3d
		string path;
		int    x,y,z;
		
	};
	class Audio : public TimeBaseClass {
	public:
		Audio() {
			duration = 10.0;  // 10 seconds
			volume = 5; // 1/2 way
		}
		void read(const Json::Value &data);
		string url;// can be local too
		int    volume;
		
	};
	class Video : public Audio {
	};
	// default settings
	class Defaults : public TimeBaseClass {
	public:
		Defaults() {
			font = "data/Raleway - Thin.ttf";
			italicfont = "data/Raleway-Italic.ttf";
			boldfont = "data/Raleway-Bold.ttf";
			fontsize = 18;
		}
		void read(const Json::Value &data);
		string font;
		string italicfont;
		string boldfont;
		int    fontsize;
	};

	// state of system
	class State {
	public:
		State() {}
		void setup() {}
		void update() {}
		void draw(const Defaults& defaults) {}; // draw all items in State

	private:
		vector<Paragraph2552> paragraphs;
		vector<Reference> sources;
		vector <ofImage> images;
		vector <Model3D> models;
		vector <ofVideoPlayer> videoPlayers;
		ofSoundPlayer soundPlayer;
	};

	// state that can age out
	class TimedState : public State {
	public:
		//infinite is 0
		TimedState(float durationIn = 0) {
			reset();
			duration = durationIn;
		}
		void reset() {
			startTime = ofGetElapsedTimef();
		}
		void setup() {}
		void update() {}
		void draw(const Defaults& defaults) {
			if (!stop()) {
				State::draw(defaults);
			}
		}
		bool operator==(const TimedState& rhs) { return true;/* do actual comparison bugbug */ }
		const bool stop() {
			if (!duration) {
				return false; // no time out if no duration
			}
			return (ofGetElapsedTimef() - startTime) > duration;
		}
		float duration;
		float startTime;
	private:
	};

	// cannot have a this, crazy stuff
	static bool okToRemove(const TimedState& s) {
		if (!s.duration) {
			return false; // no time out if no duration
		}
		return (ofGetElapsedTimef() - s.startTime) > s.duration;
	};
	// time and elements of time
	class TimeLine  {
	public:
		void setup(const Defaults& defaultsIn) {
			defaults = defaultsIn;
		}
		void update() {
			// remove all timed out items bugbug does this reall work ? 
			t.erase(std::remove_if(t.begin(), t.end(), okToRemove), t.end());
		}
		void draw() {
			for (TimedState& state : t) {
				state.draw(defaults);
			}
		}
		void push(const TimedState &state) {
			t.push_back(state); // saves a copy of state
		}
		void pop() {
			t.pop_back();
		}
		void remove(const TimedState &state) {
			t.erase(std::remove(t.begin(), t.end(), state), t.end());
		}
		void reset() {
			t.clear();
		}

	private:
		
		vector<TimedState> t; // in order list
		Defaults defaults;
	};

	class Slide : public TimeBaseClass {
	public:
		Slide(const string&name):TimeBaseClass(name){}

		void read(const Json::Value &data);

		string title;
		string timeline;
		string lastupdate;
		vector <Audio> audios;
		vector <Video> videos;
		vector <Text> texts;
	};

	class Deck : public TimeBaseClass {
	public:
		Deck(const string&name) :TimeBaseClass(name) {}

		void addSlide(const Slide &s) {
			slides.push_back(s);
		}
		
		vector <Slide>& getSlides(){ return slides; }

	private:
		vector <Slide> slides;
	};

	//heart of the system
	class kernel  {
	public:
		kernel() {}
		// open, parse json file bugbug move all these large in lines into cpp at some point
		void setup() { read(); };
		void update() {
			t.update();
		}
		void draw() {
			t.draw();
		}
		void read();
	private:
		
	
		// all key data needed to run the app goes here
		TimeLine t;
		ofxJSON json;  // source json
		Defaults defaults;
		KinectBodies bodies;
		KinectFaces faces;
		KinectAudio audio;
		Kinect2552 myKinect;
		vector <Deck> decks;
		

	};

}