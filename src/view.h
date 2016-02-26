#pragma once

#include "2552software.h"
#include "view.h"

namespace Software2552 {

	template <class T>
	class Wrapper : public  T {
	public:
		// wrap an Openframeworks or other such object so we can easily delete, track, test, debug  etc
		Wrapper(GraphicID id) : T() { ID = id; }
		bool operator==(const Wrapper &rhs) {
			return ID == rhs.ID;
		}
		GraphicID id() { return ID; }
	private:
		GraphicID ID;
	};

	class TextToRender {
	public:
		TextToRender() {}
		TextToRender(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; x = 0; y = 0; }
		shared_ptr<ofxSmartFont> font;
		int x;
		int y;
		ofColor color;
		string text;
	};

	// drawing drawingTools etc, shared across objects
	class DrawingTools {
	public:
		// remove item from a list
		void removeVideoPlayers(GraphicID ID);
		void setupVideoPlayer(GraphicID ID, float vol, const string&location);
		// draw all videos with this ID
		void drawVideo(GraphicID ID, int x, int y);
		void updateVideo(GraphicID ID);

		// Paragraph is a special case is its a 3rd party too
		void removeParagraphPlayers(GraphicID ID);
		void updateParagraph(GraphicID ID) {		}
		void setupParagraph(GraphicID ID, const string& text, shared_ptr<ofxSmartFont> font, int x, int y, int width=620, const ofColor& color=ofColor(0,0,0), ofxParagraph::Alignment align= ofxParagraph::ALIGN_LEFT, int indent= 40, int leading= 16, int spacing=6);
		void drawParagraph(GraphicID ID);
		void drawParagraph(GraphicID ID, const ofVec2f& point);

		void removeTextPlayers(GraphicID ID);
		void updateText(GraphicID ID) {		}
		void setupText(GraphicID ID, const string& text, shared_ptr<ofxSmartFont> font, int x, int y, const ofColor& color = ofColor(0, 0, 0));
		void drawText(GraphicID ID);
		void drawText(GraphicID ID, const ofVec2f& point);

		void start() { ofPushStyle(); } // start draw
		void end() { ofPopStyle(); } // end draw
	private:
		ofLight	 light;
		ofCamera camera;
		vector<Wrapper<ofVideoPlayer>> videoPlayers;
		vector<Wrapper<ofxParagraph>> paragraphPlayers;
		vector<Wrapper<TextToRender>> textPlayers;
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
