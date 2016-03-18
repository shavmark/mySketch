#pragma once
#include "2552software.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxAnimatableOfColor.h"
// supports animation

namespace Software2552 {

	// basic drawing info, bugbug maybe color set goes here too, not sure yet
	class DrawingBasics {
	public:
		// helpers to wrap basic functions
		virtual void setupBasic() {};
		virtual void updateBasic() {};
		virtual void drawBasic() {};
		virtual bool loadBasic() { return true; };
		void setRefreshRate(float) {}//bugbug make part of animation
		bool refreshAnimation() { return true; } ///bugbug find the real one
		ofPoint pos;
		int w = 0;
		int h = 0;
		bool okToDraw() { return true; }//bugbug move this to the right place
		string &getLocation() { return locationPath; }
		void setLocation(const string&s) { locationPath = s; }
		uint64_t &getWait() { return wait; }//bugbug put in the right place, something from animation
		uint64_t &getLEARNINGDuration() { return dura; }//bugbug put in the right place, something from animation
	private:
		uint64_t wait = 0;
		uint64_t dura = 0;
		string locationPath; // location of item to draw

	};

	// animates colors
	class ColorAnimation : public ofxAnimatableOfColor {
	public:
		void draw();
	};

	// base class used to draw, drawing done by derived classes
	class Animatable : public ofxAnimatableFloat, public DrawingBasics {
	public:
		virtual void draw();
		virtual void update(float dt);
		void set(shared_ptr<ColorAnimation>p);

	private:
		shared_ptr<ColorAnimation> colorAnimation = nullptr; // optional color
	};

	// any object that moves, has size or color starts here
	class Animator : public Animatable {
	public:
		Animator(bool setup=true);
		//bugbug code for a bit then put in read/data in a derived class
		// defined in model..
		uint64_t refreshRate() { return rate; /*ms*/ }
		void setRefreshRate(uint64_t rateIn) { rate = rateIn; };
		bool refreshAnimation();
		void startAnimation() { startTime = ofGetElapsedTimeMillis(); }
		bool isExpired() const { return expired; }
		bool okToDraw();
		void operator++ () {++usageCount;}
		bool operator> (const Animator& rhs) {	return getAnimationUsageCount() > rhs.getAnimationUsageCount();	}
		int getAnimationUsageCount() const {	return usageCount;	}
		template<typename T>void removeExpiredPtrToItems(vector<T>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), Animator::staticOKToRemovePtr), v.end());
		}
		template<typename T>void removeExpiredItems(vector<T>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), Animator::staticOKToRemove), v.end());
		}
		virtual void resetAnimation();
		virtual void pauseAnimation();
		virtual void playAnimation();
		virtual void stopAnimation() { stopped = true; }
		// how long to wait
		virtual void getTimeBeforeStart(uint64_t& t) {
			setIfGreater(t, getDuration() + getWait());
		}
		uint64_t& getDuration() { return duration; }
		uint64_t& getWait() { return wait; }
		void setWait(uint64_t waitIn) { wait = waitIn; }
		void addWait(uint64_t waitIn) { wait += waitIn; }
		int w, h; // size

	private:
		vector<ofPoint> path;
		static bool staticOKToRemovePtr(shared_ptr<Animator> me);
		static bool staticOKToRemove(const Animator& me);
		uint64_t	duration; // life time of object, 0 means forever
		uint64_t	wait;     // time to wait before drawing
		uint64_t	startTime;
		uint64_t	rate;	 // refresh rate
		bool		expired; // object is expired
		bool		stopped;
		bool		paused;
		int			usageCount; // number of times this animation was used
	};



}