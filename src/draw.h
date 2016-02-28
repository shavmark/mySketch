#pragma once
#include "2552software.h"
namespace Software2552 {
	// wrap any class to help as needed
	template <class T>
	class Wrapper : public  T {
	public:
		// wrap an Openframeworks or other such object so we can easily delete, track, test, debug  etc
		Wrapper(GraphicID id) : T() {
			ID = id;
			duration = 0; // 0 is infinte
		}
		Wrapper() : T() {
			ID = ofGetSystemTimeMicros();
			duration = 0; // 0 is infinte
		}
		bool operator==(const Wrapper &rhs) {
			return ID == rhs.ID;
		}
		GraphicID id() { return ID; }
		void set(GraphicID id) { ID = id; }
		float getDuration() { return duration; }
		void setDuration(float durationIn) { duration = durationIn; }
		string &getLocation() { return location; }
		void setLocation(const string& locationIn) { location = locationIn; }
	private:
		GraphicID ID;
		float duration; // how long this should stay around
		string location; // url or local path, optional
	};


	class TextToRender {
	public:
		TextToRender() {}
		TextToRender(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; }
		void draw(int x, int y) {
			if (font != nullptr) {
				ofPushStyle();
				ofSetColor(color);
				font->draw(text, x, y);
				ofPopStyle();
			}
		}
		void draw() {
			draw(point.x, point.y);
		}
		void update() {};
		void setFont(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; }
		void setStart(const ofVec3f& pointIn) { point = pointIn;}//bugbug int or float x and y ?
		void setColor(const ofColor& colorIn) { color = colorIn; }
		void setText(const string& strIn) { text = strIn; }
	private:
		shared_ptr<ofxSmartFont> font;
		ofVec3f point;
		ofColor color;
		string text;
	};

}