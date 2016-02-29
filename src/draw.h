#pragma once
#include "2552software.h"

namespace Software2552 {

	class TextToRender {
	public:
		TextToRender() {}
		TextToRender(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; }
		void draw(int x, int y);
		void update() {};

		void setFont(shared_ptr<ofxSmartFont> fontIn) { font = fontIn; }
		void setColor(const ofColor& colorIn) { color = colorIn; }
		void setText(const string& strIn) { text = strIn; }
	private:
		shared_ptr<ofxSmartFont> font;
		ofColor color;
		string text;
	};

}