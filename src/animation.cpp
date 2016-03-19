#include "2552software.h"
#include "animation.h"

namespace Software2552 {


	Animator::Animator(bool setupIn) {
		if (setupIn) {
			resetAnimation();
		}
	}
	//ColorSet Animator::getFirstColor(ColorSet::ColorGroup group) {
	//	std::vector<ColorSet>::iterator itr = std::find(data.begin(), data.end(), ColorSet(group));
	//	if (itr != data.end()) {
	//		++(*itr); // reflect usage
	//		return *itr;
	//	}
	//	return ColorSet(group); // always do something
	//}

	void Animator::playAnimation() {
		stopped = paused = false;
		startAnimation();
	}
	// not coded yet
	void Animator::pauseAnimation() {
		uint64_t elapsed = ofGetElapsedTimeMillis();
		// if beyond wait time 
		// else hold wait time even after pause
		if (elapsed - (startTime + wait) > 0) {
			wait = 0; // ignore wait time upon return
		}
		paused = true;
	}

	bool Animator::staticOKToRemovePtr(shared_ptr<Animator> me) {
		if (me == nullptr) {
			return false;
		}
		return staticOKToRemove(*me);
	}
	
	bool Animator::staticOKToRemove(const Animator& me) {
		if (me.isExpired()) {
			return true;
		}
		// duration == 0 means never go away, and start == 0 means we have not started yet
		if (me.duration == 0 || me.startTime == 0) {
			return false; // no time out ever, or we have not started yet
		}
		uint64_t elapsed = ofGetElapsedTimeMillis() - me.startTime;
		if (me.wait > elapsed) {
			return false;
		}
		if (me.duration > elapsed) {
			return false;
		}
		return true;

	}

	bool Animator::okToDraw() {
		if (paused || stopped || isExpired()) {
			return false;
		}
		// if still in wait threshold
		uint64_t t = ofGetElapsedTimeMillis() - startTime;
		if (t < wait) {
			return false; // in wait mode, nothing else to do
		}
		wait = 0; // skip all future usage of wait once we start
				  // duration 0 means always draw
		if (duration == 0) {
			return true;
		}
		if (t < duration) {
			return true;
		}
		else {
			expired = true;
			return false;
		}
	}



	void Animator::resetAnimation() {
//		usageCount = 0;
		startTime = 0;
		expired = false;
		paused = false;
		stopped = false;
		duration = 0; // infinite bugbug duraiton and wait not full baked in yet
		wait = 0;
		rate = 20000;// 20 seconds while developing, but much longer later bugbug set in json
		w = ofGetWidth();
		h = ofGetHeight();

	}
	// return true if a refresh was done
	bool Animator::refreshAnimation() {
		if (startTime == 0) {
			startAnimation();
			return false;
		}
		uint64_t t = ofGetElapsedTimeMillis() - startTime;

		if (t < getWait()) {
			return false;// waiting to start
		}
		wait = 0; // skip all future usage of wait once we start
				  // check for expired flag 
		if (duration > 0 && t > duration) {
			expired = true; // duration of 0 means no expiration
			return false;
		}
		// at this point we can start the time over w/o a wait
		if (t > refreshRate()) {
			startTime = ofGetElapsedTimeMillis();
			return true;
		}
		return false;
	}


}