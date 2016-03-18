#pragma once
#include "2552software.h"
#include "model.h"
#include "scenes.h"

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


		bool readScript(const string& path);

	private:
		// stat all objects in vector to playing
		template<typename T> void startAnimation(T& v) {
			for (auto& t : v) {
				t.play();
			}
		}
		MoreMesh mesh; // just for now bugbug, move into class hiearchey when ready
		Stage scene;
		vector<Act> acts; // timeline is a series of acts
						  // objects in model, timing code in model, colors from json
	};


}