#pragma once

#include "2552software.h"
#include "draw.h"

// to do  1. go to smart pointers for lists, then add in time out code in contorller

namespace Software2552 {

	// drawing drawingTools etc, shared across objects
	class DrawingTools {
	public:
		friend class Timeline;

		template<typename T> void DrawingTools::update(GraphicID ID, T& v) {
			for (auto& t : v) {
				if (t->id() == ID) {
					t->update();
				}
			}
		}
		// a valid x and y are required to use this helper
		template<typename T> void DrawingTools::draw(GraphicID ID, T& v, int x, int y) {
			for (auto& t : v) {
				if (t->id() == ID) {
					t->draw(x, y);
				}
			}
		}
		template<typename T> void DrawingTools::removePlayers(GraphicID ID, T& v) {
			T::iterator i = v.begin();
			while (i != v.end()) {
				if (!(*i)->id() == ID) {
					i = v.erase(i);
				}
				else {
					++i;
				}
			}
		}

		// add (and setup) a video player
		void DrawingTools::setup(shared_ptr<Wrapper<ofVideoPlayer>> player) {
			if (player->load(player->getLocation())) {
				player->play();
				videoPlayers.push_back(player);
			}
			else {
				logErrorString("add video Player");
			}
		}
		void setup(shared_ptr< Wrapper<ofxParagraph>> player) {
			paragraphPlayers.push_back(player);
		}
		void setup(shared_ptr< Wrapper<ofSoundPlayer>> player) {
			if (player->load(player->getLocation())) {
				player->play();
				audioPlayers.push_back(player);
			}
			else {
				logErrorString("add sound Player");
			}
		}
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
