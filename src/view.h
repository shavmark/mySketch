#pragma once

#include "2552software.h"
#include "view.h"

namespace Software2552 {

	

	class TextToRender {
	public:
		TextToRender() {}
		TextToRender(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; x = 0; y = 0; }
		shared_ptr<ofxSmartFont> font;
		int x;
		int y;
		ofColor color;
		string text;
		void update() {};
	};

	// drawing drawingTools etc, shared across objects
	class DrawingTools {
	public:
		friend class Timeline;

		template<typename T> void DrawingTools::update(GraphicID ID, T& v) {
			for (auto& t : v) {
				if (t.id() == ID) {
					t.update();
				}
			}
		}
		// a valid x and y are required to use this helper
		template<typename T> void DrawingTools::draw(GraphicID ID, T& v, int x, int y) {
			for (auto& t : v) {
				if (t.id() == ID) {
					t.draw(x, y);
				}
			}
		}
		template<typename T> void DrawingTools::removePlayers(GraphicID ID, T& v) {
			T::iterator i = v.begin();
			while (i != v.end()) {
				if (!i->id() == ID) {
					i = v.erase(i);
				}
				else {
					++i;
				}
			}
		}

		// add (and setup) a video player
		void DrawingTools::setup(Wrapper<ofVideoPlayer> &player) {
			if (player.load(player.getLocation())) {
				player.play();
				videoPlayers.push_back(player);
			}
			else {
				logErrorString("add Player");
			}
		}
		void setup(const Wrapper<ofxParagraph> &player) {
			paragraphPlayers.push_back(player);
		}
		void setup(const Wrapper<ofSoundPlayer> &player) {
			audioPlayers.push_back(player);
		}
		void setupText(GraphicID ID, const string& text, shared_ptr<ofxSmartFont> font, int x, int y, const ofColor& color = ofColor(0, 0, 0));
		void drawText(GraphicID ID, int x=0, int y = 0);

		void start() { ofPushStyle(); } // start draw
		void end() { ofPopStyle(); } // end draw
	private:
		ofLight	 light;
		ofCamera camera;
		//bugbug at some point make these shared_ptr to avoid coping data
		vector<Wrapper<ofVideoPlayer>> videoPlayers;
		vector<Wrapper<ofxParagraph>> paragraphPlayers;
		vector<Wrapper<TextToRender>> textPlayers;
		vector<Wrapper<ofSoundPlayer>> audioPlayers;
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
