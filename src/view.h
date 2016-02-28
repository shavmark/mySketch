#pragma once

#include "2552software.h"
#include "draw.h"

// to do  1.  add in time out code in contorller

namespace Software2552 {

	// drawing drawingTools etc, shared across objects
	class DrawingTools {
	public:
		//friend class Timeline;

		template<typename T> void update(T& v) {
			for (auto& t : v) {
				if (t->okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
					t->update();
				}
			}
		}
		// a valid x and y are required to use this helper
		template<typename T> void draw(T& v) {
			for (auto& t : v) {
				if (t->okToDraw()) {
					t->draw(t->point.x, t->point.y);
				}
			}
		}
		// remove items that are timed out
		template<typename T> void removeExpiredItems(T& v) {
			T::iterator i = v.begin();
			while (i != v.end()) {
				if ((*i)->okToRemove()) {
					i = v.erase(i);
				}
				else {
					++i;
				}
			}
		}
		// update all drawing tools that require an update
		void update();
		// draw all items in need of drawing
		void draw();
		void cleanup();
		// add (and setup) a video player
		void setup(shared_ptr<Wrapper<ofVideoPlayer>> player);
		void setup(shared_ptr< Wrapper<ofxParagraph>> player) {
			paragraphPlayers.push_back(player);
		}
		void setup(shared_ptr< Wrapper<ofSoundPlayer>> player);
		void setup(shared_ptr<Wrapper<TextToRender>> player) {
			textPlayers.push_back(player);
		}

		void start() { ofPushStyle(); } // start draw
		void end() { ofPopStyle(); } // end draw
	private:
		ofLight	 light;
		ofCamera camera;
		//bugbug at some point make these shared_ptr to avoid coping data
		vector<shared_ptr<Wrapper<ofVideoPlayer>>> videoPlayers;
		vector<shared_ptr<Wrapper<ofxParagraph>>> paragraphPlayers;
		vector<shared_ptr<Wrapper<TextToRender>>> textPlayers;
		vector<shared_ptr<Wrapper<ofSoundPlayer>>> audioPlayers;
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
