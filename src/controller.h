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
		void pause();
		void play();

		bool readAct(const string& path);

	private:
		// stat all objects in vector to playing
		template<typename T> void startReadHead(T& v) {
			for (auto& t : v) {
				t.play();
			}
		}
		DrawingTools drawit;
		int framerate;
		shared_ptr<GraphicEngines> engines;
		vector<Act> acts; // timeline is a series of acts
	};


}