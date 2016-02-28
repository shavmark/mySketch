#include "draw.h"

// this is the drawing  module


namespace Software2552 {
	void TextToRender::draw(int x, int y) {
		if (font != nullptr) {
			ofPushStyle();
			ofSetColor(color);
			font->draw(text, x, y);
			ofPopStyle();
		}
	}
}
