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
		}
		bool read(const Json::Value &data);
		void setup() {
			start = ofGetElapsedTimef();
			start += delay;
		}
		void update() {
		}
		void draw() {
			if (start <= 0 || ofGetElapsedTimef() > 0) {
				return; // not started yet
			}
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

		int    x, y, z;
		string type; // 2d, 3d, other
		string foreground;
		string background;
		float duration;
		float start;
		float delay; // start+delay is the true start
	};

	class Text : public Graphic {
	public:
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Text));
			Graphic::trace();
			basicTrace(paragraph);
			basicTrace(font);
			basicTrace(ofToString(size));
		}
#endif // _DEBUG

		string paragraph; //bugbug convert to ofxParagraph for here, font and size (not sure about size)
		string font;
		int size;
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


	// default settings
	class Defaults : public TimeLineBaseClass, public Graphic {
	public:
		Defaults() {
			font = "data/Raleway - Thin.ttf";
			italicfont = "data/Raleway-Italic.ttf";
			boldfont = "data/Raleway-Bold.ttf";
			fontsize = 18;
		}
		bool read(const Json::Value &data);

#if _DEBUG
		// echo object (debug only)
		void trace() {
			basicTrace(STRINGIFY(Defaults));

			TimeLineBaseClass::trace();
			Graphic::trace();

			basicTrace(font);
			basicTrace(italicfont);
			basicTrace(boldfont);
			basicTrace(ofToString(fontsize));
		}
#endif
		string font;
		string italicfont;
		string boldfont;
		int    fontsize;
	};

	class Slide : public TimeLineBaseClass {
	public:
		Slide(const string&name) :TimeLineBaseClass(name) {}

		void setup()	{
			for (auto& a : audios) {
				a.setup();
			}
			for (auto& v : videos) {
				v.setup();
			}
			for (auto& t : texts) {
				t.setup();
			}
			for (auto& i : images) {
				i.setup();
			}
			for (auto& g : graphics) {
				g.setup();
			}
			for (auto& c : characters) {
				c.setup();
			}
		}

		void update() {
			// remove all timed out items 
			audios.erase(std::remove_if(audios.begin(), audios.end(), okToRemove), audios.end());
			videos.erase(std::remove_if(videos.begin(), videos.end(), okToRemove), videos.end());
			characters.erase(std::remove_if(characters.begin(), characters.end(), okToRemove), characters.end());
			images.erase(std::remove_if(images.begin(), images.end(), okToRemove), images.end());
			graphics.erase(std::remove_if(graphics.begin(), graphics.end(), okToRemove), graphics.end());
			texts.erase(std::remove_if(texts.begin(), texts.end(), okToRemove), texts.end());
			for (auto& a : audios) {
				a.update();
			}
			for (auto& v : videos) {
				v.update();
			}
			for (auto& t : texts) {
				t.update();
			}
			for (auto& i : images) {
				i.update();
			}
			for (auto& g : graphics) {
				g.update();
			}
			for (auto& c : characters) {
				c.update();
			}

		}
		void draw() {
			for (auto& a : audios) {
				a.draw();
			}
			for (auto& v : videos) {
				v.draw();
			}
			for (auto& t : texts) {
				t.draw();
			}
			for (auto& i : images) {
				i.draw();
			}
			for (auto& g : graphics) {
				g.draw();
			}
			for (auto& c : characters) {
				c.draw();
			}
		}

#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			basicTrace(STRINGIFY(Slide));

			TimeLineBaseClass::trace();
			basicTrace(title);
			for (auto& a : audios) {
				a.trace();
			}
			for (auto& v : videos) {
				v.trace();
			}
			for (auto& t : texts) {
				t.trace();
			}
			for (auto& i : images) {
				i.trace();
			}
			for (auto& g : graphics) {
				g.trace();
			}
			for (auto& c : characters) {
				c.trace();
			}
		}

#endif // _DEBUG

		bool read(const Json::Value &data);
		// cannot have a this, crazy stuff
		static bool okToRemove(const Graphic& s) {
			if (!s.duration) {
				return false; // no time out if no duration
			}
			return (ofGetElapsedTimef() - s.start) > s.duration;
		};

		string title;
		vector <Audio> audios; // join with ofaudio
		vector <Video> videos; // join wiht ofvideo
		vector <Character> characters; // join with vector <Model3D> models;
		vector <Image> images; //bugbug join with ofImage vector <ofImage> images;
		vector <Graphic> graphics; // tie to ofX

		vector <Text>  texts; //bugbug join Text and Paragraph2552 vector<Paragraph2552> paragraphs;

		
		

	};

	class Deck : public TimeLineBaseClass {
	public:
		Deck(const string&name) :TimeLineBaseClass(name) {}
		void setup() {
			for (auto& slide : slides) {
				slide.setup();
			}
		}
		void update() {
			for (auto& slide : slides) {
				slide.update();
			}
		}
		void draw() {
			for (auto& slide : slides) {
				slide.draw();
			}
		}
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			basicTrace(STRINGIFY(Deck));
			for (auto& slide : slides) {
				slide.trace();
			}
		}
#endif
		void addSlide(const Slide &s) {
			slides.push_back(s);
		}
		vector <Slide>& getSlides() { return slides; }
	private:
		vector <Slide> slides;
	};
	// state that can age out BUGBUG just merge with timeline, redundiant -- next step
	class TimedState {
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
				// draw here as needed bugbug big todo, this is the app
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

#if 0
		// time and elements of time
	class TimeLine {
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
#endif // 0


	

	//heart of the system
	class kernel  {
	public:
		kernel() {}
		// open, parse json file bugbug move all these large in lines into cpp at some point
		void setup() { 
			read();  
			for (auto& deck : decks) {
				deck.setup(); // bugbug reset time of start when the graphic is drawn
			}
		};
		void update() {
			for (auto& deck : decks) {
				deck.update();
			}
		}
		void draw() {
			for (auto& deck : decks) {
				deck.draw();
			}
		}
		bool read();
#if _DEBUG
		// echo object (debug only) bugbug make debug only
		void trace() {
			basicTrace(STRINGIFY(kernel));
			for (auto& deck : decks) {
				deck.trace();
			}
		}
#endif

	private:
		
	
		// all key data needed to run the app goes here
		ofxJSON json;  // source json
		Defaults defaults;
		KinectBodies bodies;
		KinectFaces faces;
		KinectAudio audio;
		Kinect2552 myKinect;
		vector <Deck> decks;
		vector <ofVideoPlayer> videoPlayers;
		ofSoundPlayer soundPlayer;


	};

}