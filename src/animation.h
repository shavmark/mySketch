#pragma once
#include "2552software.h"
#include "ofxAnimatableFloat.h"
#include "ofxAnimatableOfPoint.h"
#include "ofxAnimatableOfColor.h"

// supports animation

namespace Software2552 {

	class objectLifeTimeManager {
	public:
		bool isExpired() const { return expired; }
		template<typename T>void removeExpiredPtrToItems(vector<T>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), objectLifeTimeManager::staticOKToRemovePtr), v.end());
		}
		template<typename T>void removeExpiredItems(vector<T>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), objectLifeTimeManager::staticOKToRemove), v.end());
		}
		float& getOjectLifetime() { return objectlifetime; }
		static bool staticOKToRemovePtr(shared_ptr<objectLifeTimeManager> me);
		static bool staticOKToRemove(const objectLifeTimeManager& me);
		bool  objectIsExpired() { return expired; }
		void operator++ () { ++usageCount; }
		bool operator> (const objectLifeTimeManager& rhs) { return usageCount > rhs.usageCount; }
		int getAnimationUsageCount() const { return usageCount; }

	private:
		int	     usageCount=0;   // number of times this animation was used
		float    objectlifetime=0; // 0=forever, how long object lives after it starts drawing
		bool	 expired=false;    // object is expired

	};
	class AniPointWrapper : public ofxAnimatableOfPoint, public objectLifeTimeManager {
	public:
		float& getAnimationWait() { return waitTime_; }
		float& getAnimationDelay() { return delay_; }
	};
	// animates colors
	class ColorAnimation : public ofxAnimatableOfColor, public objectLifeTimeManager {
	public:
		void draw();
	};

	class MyAnimation   {
	public:
		MyAnimation() {  }

		// avoid name clashes and wrap the most used items, else access the contained object for more
		AniPointWrapper& getAnimationHelper() { return ani; }
		float& getAnimationWait() { return getAnimationHelper().getAnimationWait(); }
		float& getAnimationDelay() { return getAnimationHelper().getAnimationDelay(); }
		ofPoint& getCurrentPosition() { return getAnimationHelper().getCurrentPosition(); }
		void setPositionX(float f) { getAnimationHelper().setPositionX(f); }
		void setPositionY(float f) { getAnimationHelper().setPositionY(f); }
		void setPositionZ(float f) { getAnimationHelper().setPositionZ(f); }
		void setAnimationPosition(const ofPoint& p) { getAnimationHelper().setPosition(p); }
		void setDuration(float seconds) { getAnimationHelper().setDuration(seconds); }
		void setCurve(AnimCurve curveStyle) { getAnimationHelper().setCurve(curveStyle);	}
		void setRepeatType(AnimRepeat repeat) {	getAnimationHelper().setRepeatType(repeat);	}

		void pause() { getAnimationHelper().pause(); };
		void resume() { getAnimationHelper().resume(); };
		void reset() {	getAnimationHelper().reset();	}
		void updateAnimation(float dt) { getAnimationHelper().update(dt); }//must be called
		void animateTo(const ofPoint& where) { getAnimationHelper().animateTo(where); }
		void animateToAfterDelay(const ofPoint& where, float delay) { getAnimationHelper().animateToAfterDelay(where, delay); }
		void animateToIfFinished(const ofPoint& where) { getAnimationHelper().animateToIfFinished(where); }

		bool okToDraw() { return true; }//bugbug move this to the right place
		void setRefreshRate(float) {}//bugbug make part of animation
		bool refreshAnimation() { return true; } ///bugbug find the real one

		AniPointWrapper ani; // call all the other items via here

	private:
		string   locationPath;   // location of item to draw
	};
	// basic drawing info, bugbug maybe color set goes here too, not sure yet
	class DrawingBasics : public MyAnimation {
	public:
		DrawingBasics() {  }
		DrawingBasics(const string&path) { setLocationPath(path);  }
		bool okToDraw();

		// helpers to wrap basic functions
		void setupForDrawing() { mySetup(); };
		void updateForDrawing() { 
			float dt = 1.0f / 60.0f;//bugbug does this time to frame count? I think so
			if (colorAnimation != nullptr) {
				colorAnimation->update(dt);
			}
			getAnimationHelper().update(dt);
			myUpdate(); // call derived classes
		};
		void drawIt() { myDraw(); };
		bool loadForDrawing() { return myObjectLoad(); };

		void setColorAnimation(shared_ptr<ColorAnimation>p) {
			colorAnimation = p;
		}
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
	};



}