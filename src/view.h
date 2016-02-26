#pragma once

#include "2552software.h"
#include "view.h"

namespace Software2552 {

	// drawing tools etc, shared across objects
	class Tools {
	public:
		Tools() {
		}
		ofLight	 light;
		ofCamera camera;
		vector<ofVideoPlayer> videoPlayers;
	};

	// drawing related items start here
	class BaseClass2552WithDrawing : public BaseClass {
	public:
		BaseClass2552WithDrawing() { valid = false; }

		bool objectValid() { return valid; } // data is in a good state
		void setValid(bool b = true) { valid = b; };

	private:
		bool valid; // true when data is valid and ready to draw
	};
	class Graphics2552  {
	public:
		static void rotateToNormal(ofVec3f normal);

	};

}
