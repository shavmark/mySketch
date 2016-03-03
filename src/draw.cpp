#include "draw.h"
#include "model.h"
// this is the drawing  module


namespace Software2552 {
	void TextEngine::draw(Text* t) {
		ofPushStyle();
		ofSetColor(t->getForeground());
		t->getFont().drawString(t->getText(), t->getStartingPoint().x, t->getStartingPoint().y);
		ofPopStyle();
	}

}
