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
		const int frameRate = 60;

		bool readScript(const string& path);
		Playlist playlist;

	private:

		MoreMesh mesh; // just for now bugbug, move into class hiearchey when ready
	};


}