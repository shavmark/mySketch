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
			start = 0; // force reset to be called to make sure timing is right, 0 means not started
			delay = 0;
		}
		Wrapper() : T() {
			ID = ofGetSystemTimeMicros();
			duration = 0; // 0 is infinte
			start = 0; // force reset to be called to make sure timing is right, 0 means not started
			delay = 0;
		}
		bool operator==(const Wrapper &rhs) {
			return ID == rhs.ID;
		}
		void startDrawing() {
			start = ofGetElapsedTimef();
		}
		
		// for use where static needed
		static bool okToRemove(const Wrapper& s) {
			if (!s.duration || s.start < 0) {
				return false; // no time out ever, or we have not started yet
			}
			//bugbug delay not coded in, maybe there is a better way to make things consecutive
			return (ofGetElapsedTimef() - (s.start)) >(s.duration);
		};
		// for use with object
		bool okToRemove() {
			if (!duration || start < 0) {
				return false; // no time out ever, or we have not started yet
			}
			//bugbug delay not coded in, maybe there is a better way to make things consecutive
			return (ofGetElapsedTimef() - (start)) >(duration);
		}
		bool okToDraw() {
			if (okToRemove()) {
				return false;
			}
			if (duration == 0) {
				return true; // always draw
			}
			if (start <= 0) {
				return false; // not started yet
			}
			// still going??
			float f = ofGetElapsedTimef();
			return start + delay + duration > ofGetElapsedTimef();
		}
		GraphicID id() { return ID; }
		void set(GraphicID id) { ID = id; }
		float getDuration() { return duration; }
		void setDuration(float durationIn) { duration = durationIn; }
		string &getLocation() { return location; }
		void setLocation(const string& locationIn) { location = locationIn; }
		void setStart(const ofVec3f& pointIn) { point = pointIn; }//bugbug int or float x and y ?
		ofVec3f point;
	private:
		GraphicID ID;
		float duration; // how long this should stay around
		string location; // url or local path, optional
		float start;//bugbug this moves to the controller
		float delay; // start+delay is the true start

	};


	class TextToRender {
	public:
		TextToRender() {}
		TextToRender(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; }
		void draw(int x, int y);
		void update() {};

		void setFont(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; }
		void setColor(const ofColor& colorIn) { color = colorIn; }
		void setText(const string& strIn) { text = strIn; }
	private:
		shared_ptr<ofxSmartFont> font;
		ofColor color;
		string text;
	};

}