#pragma once
#include <algorithm>
#include <functional> 
#include <cctype>
#include <locale>
#include "ofMain.h"
#include "ofxSmartFont.h"
#include "ofxParagraph.h"
#include "ofxJSON.h"
#include "2552software.h"

// timeline software

namespace From2552Software {
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
			setText(ofxSmartText(text, ofxSmartFont::add("raleway/Raleway-Thin.ttf", 14, "raleway-thin")));
			if (titleIn.length() > 0) {
				setTitle(ofxSmartText(titleIn, ofxSmartFont::add("raleway/Raleway-Italic.ttf", 18, "raleway-italic")));
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

	class Paragraphs {
	public:
		void build(int screenWidth, string file = "json.json") {

			// Now parse the JSON and keep it around
			bool parsingSuccessful = json.open(file);

			if (parsingSuccessful)
			{
				ofLogNotice("ofApp::setup") << json.getRawString();
			}
			else
			{
				ofLogError("ofApp::setup") << "Failed to parse JSON" << endl;
			}

			int y = 0;
			for (Json::ArrayIndex i = 0; i < json["treaties"].size(); ++i) {
				std::string title = json["treaties"][i]["title"].asString();
				for (Json::ArrayIndex j = 0; j < json["treaties"][i]["text"].size(); ++j) {
					std::string text = json["treaties"][i]["text"][j]["p"].asString();
					Paragraph2552 p(title, text, screenWidth, y);
					paragraphs.push_back(p);
					y = p.nextRow();
					title.clear(); // only show title one time
				}
			}

		}

		vector<Paragraph2552> & get() {
			return paragraphs;
		}
		Paragraph2552 & get(int i) {
			return paragraphs[i];
		}

	private:
		vector<Paragraph2552> paragraphs;
		ofxJSONElement json;
	};

	//bugbug move json logic here too

	class Reference {
	public:
		// if these strings are displayed they can use a non presentation font as they will be done in a popup or such
		string link;
		string date;
		string location;
		string source;
	};
	// state of system
	class State : public Trace2552 {
	public:
		State() {}
		void setup() {}
		void update() {}
		void draw() {}; // draw all items in State

	private:
		vector<Paragraph2552> paragraphs;
		vector<string> notes; // internal use, not directly displayed other than in a non presentation popup possibly
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
		void draw() {
			if (!stop()) {
				State::draw();
			}
		}
		bool stop() {
			if (!duration) {
				return false; // no time out if no duration
			}
			return (ofGetElapsedTimef() - startTime) > duration;
		}
	private:
		float startTime;
		float duration;
	};

	// time and elements of time
	class TimeLine : public Trace2552 {
	public:
		void setup() {}
		void update() {
			// remove all timed out items bugbug does this reall work ? 
			t.erase(std::remove_if(t.begin(), t.end(), okToRemove), t.end());
		}
		void draw() {
			for (TimedState& state : t) {
				state.draw();
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
		bool okToRemove(TimedState& s) {
			return s.stop(); 
		};
		vector<TimedState> t;
	};
}