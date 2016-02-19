#pragma once
#include <algorithm>
#include "ofMain.h"
#include "ofxSmartFont.h"
#include "ofxJSON.h"
#include "2552software.h"

// timeline software

namespace From2552Software {
	//bugbug move json logic here too

	// state of system
	class State : public Trace2552 {
	public:
		State() {}
		// copy constructor
		State(const State& state) {
			font = state.font;
		}
		void setup() {}
		void update() {}
		void draw() {}; // draw all items in State
	private:
		shared_ptr<ofxSmartFont> font;
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
		void update() {}
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
		vector<TimedState> t;
	};
}