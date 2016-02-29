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
			waitTime = 0;
			paused = false;
		}
		Wrapper() : T() {
			ID = ofGetSystemTimeMicros();
			duration = 0; // 0 is infinte
			start = 0; // force reset to be called to make sure timing is right, 0 means not started
			waitTime = 0;
			paused = false;
		}
		bool operator==(const Wrapper &rhs) {
			return ID == rhs.ID;
		}
		void startReadHead() {
			start = ofGetElapsedTimef(); // set a base line of time
		}
		// for use with object
		bool okToRemove() {
			if (duration == 0) {
				return false; // no time out ever, or we have not started yet
			}
			// example: ElapsedTime = 100, start = 50, wait = 100, duration 10 is (100-(50-100) > 60 is false 
			// example: ElapsedTime = 500, start = 50, wait = 100, duration 10 is (500-(50-100) > 60 is true 
			return (ofGetElapsedTimef() - (start - waitTime)) > start+duration;
		}
		void wrapperPause() {
			float elapsed = ofGetElapsedTimef();
			// if beyond wait time 
			// else hold wait time even after pause
			if (elapsed - (start + waitTime) > 0) {
				waitTime = 0; // ignore wait time upon return
			}
			paused = true;
		}
		void wrapperPlay() {
			paused = false;
			startReadHead();
		}
		bool okToDraw() {
			if (paused) {
				return false;
			}
			float elapsed = ofGetElapsedTimef();
			// example: ElapsedTime = 100, start = 50, wait = 100, duration 10
			if (elapsed - (start+waitTime) > 0) {
				if (duration == 0) {
					return true; // draw away
				}
				// ok to start but only if we are less than duration
				return (elapsed < start + waitTime + duration);
			}
			return false;
		}
		GraphicID id() { return ID; }
		void set(GraphicID id) { ID = id; }
		float getWrapperDuration() { return duration; }
		void setWaitTime(float waitTimeIn) {
			waitTime = waitTimeIn;
		}
		float getWrapperWait() { return waitTime; }
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
		float waitTime; // wait time before drawing starts
		bool paused;
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