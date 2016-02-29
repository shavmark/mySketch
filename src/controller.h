#pragma once
#include "2552software.h"
#include "model.h"
#include "view.h"

// join data and view, this class knows about both the data and how its shown

namespace Software2552 {
	class Timeline {
	public:
		Timeline();
		void setup();
		void update();
		void draw();
		void stop() {}
		void pause() {}
		void play() {}
		void fastForward() {}
		void skip() {}
		void readStory(const string& path, const string& title);
	private:
		template<typename T> void setupTools(T& v);
		void enumerateSetup(Scene &scene);
		DrawingTools drawingTools;
		float longestWaitTime;
		
	};


}