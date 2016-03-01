#pragma once

#include "2552software.h"
#include "draw.h"
#include "model.h"

// to do  1.  add in time out code in contorller

namespace Software2552 {

	// drawing drawingTools etc, shared across objects
	class DrawingTools {
	public:
		DrawingTools() {
			engines = std::make_shared<GraphicEngines>();
		}
		template<typename T> void update(T& v) {
			for (auto& t : v) {
				if (t.okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
					t.getPlayer().update();
				}
			}
		}
		template<typename T> void startReadHead(T& v) {
			for (auto& t : v) {
				t.play();
			}
		}
		// a valid x and y are required to use this helper
		template<typename T> void draw(T& v) {
			for (auto& t : v) {
				if (t.okToDraw()) {
					t.getPlayer().draw(t.getStartingPoint().x, t.getStartingPoint().y);
				}
			}
		}
		// start if ready
		template<typename T> void start(T& v) {
			for (auto& a : v) {
				if (a.okToDraw() && !a.getPlayer().isPlaying()) {
					a.getPlayer().play();
				}
			}

		}
		template<typename T> void pause(T& v) {
			for (auto& player : v) {
				player.pause();
			}
		}

		void pause();
		void play();

		// update all drawing tools that require an update
		void update();
		// draw all items in need of drawing
		void draw();
		void start() { ofPushStyle(); } // start draw
		void end() { ofPopStyle(); } // end draw
		void addEngines(shared_ptr<GraphicEngines> e) {
			engines->add(e);
		}
		void setupEngines() {
			engines->setup();
		}
	private:
		shared_ptr<GraphicEngines> engines;

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
