#pragma once

#include "2552software.h"
#include "draw.h"

// to do  1.  add in time out code in contorller

namespace Software2552 {

	// drawing drawingTools etc, shared across objects
	class DrawingTools {
	public:

		template<typename T> void update(T& v) {
			for (auto& t : v) {
				if (t->okToDraw()) { //bugbug thinking here is only update active ones? or are they deleted?
					t->update();
				}
			}
		}
		template<typename T> void startReadHead(T& v) {
			for (auto& t : v) {
				t->startReadHead();
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
		void setIfGreater(float& f1, float f2) {
			if (f2 > f1) {
				f1 = f2;
			}
		}
		// start if ready
		template<typename T> void start(T& v) {
			for (auto& player : v) {
				if (player->okToDraw() && !player->isPlaying()) {
					player->play();
				}
			}

		}

		template<typename T> float findMaxDelay(T& v) {
			float f = 0;
			for (auto& t : v) {
				setIfGreater(f, t->getWrapperDuration());
			}
			return f;
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
		float getLongestDelay()	{
			float f = 0;
			for (auto& v : videoPlayers) {
				setIfGreater(f, v->getDuration());
			}
			setIfGreater(f, findMaxDelay(paragraphPlayers));
			setIfGreater(f, findMaxDelay(textPlayers));
			setIfGreater(f, findMaxDelay(audioPlayers));
			return f;
		}
		void stop() {}
		void pause() {}
		void play();
		void skip() {}
		void fastForward() {}

		// update all drawing tools that require an update
		void update();
		// draw all items in need of drawing
		void draw();
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
		void cleanup();

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
