#pragma once
#include "2552software.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxAnimatableOfColor.h"

// supports animation

namespace Software2552 {

	class objectLifeTimeManager {
	public:
		objectLifeTimeManager();
		void start() { startTime = ofGetElapsedTimeMillis(); };
		void setRefreshRate(uint64_t rateIn) { refreshRate = rateIn; }
		float getRefreshRate() {return refreshRate;	}
		void setWait(float w) { waitTime = w; }
		float getWait() { return waitTime; }
		float getStart() { return startTime; }
		bool isExpired() const { return expired; }
		void setExpired(bool b = true) { expired = true; }
		float getObjectLifetime() { return objectlifetime; }
		void setObjectLifetime(float t) { objectlifetime=t; }
		void operator++ () { ++usageCount; }
		bool operator> (const objectLifeTimeManager& rhs) { return usageCount > rhs.usageCount; }
		int getAnimationUsageCount() const { return usageCount; }
		bool refreshAnimation();
		// how long to wait
		virtual void getTimeBeforeStart(float& t) {	setIfGreater(t, getObjectLifetime() + getWait());		}
		static bool OKToRemove(shared_ptr<objectLifeTimeManager> me);
		void removeExpiredItems(vector<shared_ptr<objectLifeTimeManager>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), OKToRemove), v.end());
		}

	private:
		int	    usageCount=0;     // number of times this animation was used
		float   objectlifetime=0; // 0=forever, how long object lives after it starts drawing
		bool	expired=false;    // object is expired
		float	startTime = 0;
		float   waitTime = 0;
		float	refreshRate = 0;
	};
	// animates colors
	class ColorAnimation : public ofxAnimatableOfColor, public objectLifeTimeManager {
	public:
		void draw();
		bool paused() { return paused_; }

	private:
	};
	class PointAnimation : public ofxAnimatableOfPoint, public objectLifeTimeManager {
	public:
		void startAnimationAfterDelay(float delay) { ofxAnimatableOfPoint::startAnimationAfterDelay(delay); }
		bool paused() {	return paused_;	}
	};
	// basic drawing info, bugbug maybe color set goes here too, not sure yet
	class DrawingBasics  {
	public:
		DrawingBasics() {  }
		DrawingBasics(const string&path) { 	setLocationPath(path); 		}
		bool okToDraw();
		// avoid name clashes and wrap the most used items, else access the contained object for more
		shared_ptr<PointAnimation> getAnimationHelper();
		virtual float getTimeBeforeStart(float t) { return getAnimationHelper()->getWait(); }
		static bool OKToRemove(shared_ptr<DrawingBasics> me) {
			return me->getAnimationHelper()->OKToRemove(me->getAnimationHelper());
		}
		static bool OKToRemoveNormalPointer(DrawingBasics* me) {
			return me->getAnimationHelper()->OKToRemove(me->getAnimationHelper());
		}
		void removeExpiredItems(vector<shared_ptr<DrawingBasics>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), OKToRemove), v.end());
		}

		// helpers to wrap basic functions
		void setupForDrawing() { mySetup(); };
		void updateForDrawing();
		void drawIt() { myDraw(); };
		bool loadForDrawing() { return myObjectLoad(); };

		void setColorAnimation(shared_ptr<ColorAnimation>p) {			colorAnimation = p;		}
		int w = 0;
		int h = 0;

		string &getLocationPath() { return locationPath; }
		void setLocationPath(const string&s) { locationPath = s; }

	private:
		// derived classes supply these if they need them to be called
		virtual void mySetup() {};
		virtual void myUpdate() {};
		virtual void myDraw() {};
		virtual bool myObjectLoad() { return true; };
		string   locationPath;   // location of item to draw
		shared_ptr<ColorAnimation> colorAnimation = nullptr; // optional color
		shared_ptr<PointAnimation> ani; // call all the other items via here
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



}