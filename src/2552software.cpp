#include "2552software.h"
#include <istream>
#pragma comment( lib, "sapi.lib" )

// helpers, base classes etc

namespace Software2552 {

	bool Trace::checkPointer2(IUnknown *p, const string&  message, char*file, int line) {
		logVerbose2(message, file, line); // should give some good trace
		if (p == nullptr) {
			logError2("invalid pointer " + message, file, line);
			return false;
		}
		return true;
	}
	bool Trace::checkPointer2(BaseClass *p, const string&  message, char*file, int line) {
		logVerbose2(message, file, line); // should give some good trace
		if (p == nullptr) {
			logError2("invalid pointer " + message, file, line);
			return false;
		}
		return true;
	}
	string Trace::buildString(const string& text, char* file, int line) {
		return text + " " + file + ": " + ofToString(line);
	}
	void Trace::logError2(const string& errorIn, char* file, int line ) {
		ofLog(OF_LOG_FATAL_ERROR, buildString(errorIn, file,  line));
	}
	void Trace::logError2(HRESULT hResult, const string&  message, char*file, int line) {

		logError2(message + ":  " + ofToHex(hResult), file, line);

	}
	// use when source location is not needed
	void Trace::logTraceBasic(const string& message, char *name){
		string text = name;
		text += " " + message + ";"; // dirty dump bugbug maybe clean up some day
		ofLog(OF_LOG_NOTICE, text);
	}
	void Trace::logTraceBasic(const string& message) {
#if _DEBUG
		ofLog(OF_LOG_VERBOSE, message); //OF_LOG_VERBOSE could dump a lot but in debug thats what we want?
#else
		ofLog(OF_LOG_NOTICE, message);
#endif
	}
	void Trace::logTrace2(const string& message,  char*file, int line) {
		if (ofGetLogLevel() >= OF_LOG_NOTICE) {
			ofLog(OF_LOG_NOTICE, buildString(message, file, line));
		}
	}
	bool Trace::CheckHresult2(HRESULT hResult, const string& message, char*file, int line) {
		if (FAILED(hResult)) {
			if (hResult != E_PENDING) {
				logError2(hResult, message, file, line);
			}
			return true; // error found
		}
		logVerbose2(message, file, line);
		return false; // no error
	}
	// allow wide chars this way, bugbug do we need to make wstring seamless?
	std::string Trace::wstrtostr(const std::wstring &wstr)	{
		std::string strTo;
		char *szTo = new char[wstr.length() + 1];
		szTo[wstr.size()] = '\0';
		WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
		strTo = szTo;
		delete[] szTo;
		return strTo;
	}


	Animator::Animator() {
		setup();
	}
	void Animator::update() {
		if (ofGetElapsedTimeMillis() - startTime > duration) {
			expired = true;
		}
	}
	void Animator::play() {
		paused = false;
		startReadHead();
	}
	// not coded yet
	void Animator::pause() {
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
		if (paused || isExpired()) {
			return false;
		}
		float elapsed = ofGetElapsedTimef();
		// example: ElapsedTime = 100, start = 50, wait = 100, duration 10
		if (elapsed - (startTime + wait) > 0) {
			if (duration == 0) {
				return true; // draw away
			}
			// ok to start but only if we are less than duration
			return (elapsed < startTime + wait + duration);
		}
		return false;
	}



	void Animator::setup() {
		startTime = 0;
		expired = false;
		paused = false;
		duration = 0; // infinite bugbug duraiton and wait not full baked in yet
		wait = 0;
		rate = 2000;// 2 seconds while developing, but much longer later bugbug set in json
	}
	// return true if a refresh was done
	bool Animator::refresh() {
		uint64_t t = ofGetElapsedTimeMillis() - startTime;//test

		if (t < getWait()) {
			return false;// waiting to start
		}
		if (t > duration) {
			expired = true;
			return false;
		}
		if (t > refreshRate()) {
			startTime = ofGetElapsedTimeMillis();
			return true;
		}
		return false;
	}

}