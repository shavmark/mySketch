#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract

	template<typename T> void DrawingTools::update(GraphicID ID, T& v) {
		for (auto& t : v) {
			if (t.id() == ID) {
				t.update();
			}
		}
	}
	template<typename T> void DrawingTools::draw(GraphicID ID, T& v, int x, int y) {
		for (auto& t : v) {
			if (t.id() == ID) {
				t.draw(x, y);
			}
		}
	}
	// draw all items that map to ID
	template<typename T> void DrawingTools::draw(GraphicID ID, T& v) {
		for (auto& t : v) {
			if (t.id() == ID) {
				t.draw();
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

	void DrawingTools::removeVideoPlayers(GraphicID ID) {
		removePlayers(ID, videoPlayers);
	}

	void DrawingTools::drawVideo(GraphicID ID, int x, int y) {
		draw(ID, videoPlayers, x, y);
	}
	void DrawingTools::updateVideo(GraphicID ID) {
		update(ID, videoPlayers);
	}
	void DrawingTools::setupVideoPlayer(GraphicID ID, float vol, const string&location) {
		Wrapper<ofVideoPlayer> player(ID);
		if (player.load(location)) {
			player.setVolume(vol);
			player.play();
			videoPlayers.push_back(player);
		}
		else {
			logErrorString(location);
		}

	}
	void DrawingTools::removePlayers(GraphicID ID) {
		removeVideoPlayers(ID);
		removeParagraphPlayers(ID);
		removeTextPlayers(ID);
	}
	void DrawingTools::drawParagraph(GraphicID ID) {
		draw(ID, paragraphPlayers);
	}
	void DrawingTools::drawParagraph(GraphicID ID, int x, int y) {
		draw(ID, paragraphPlayers, x, y);
	}
		
	void DrawingTools::removeParagraphPlayers(GraphicID ID) {
		removePlayers(ID, paragraphPlayers);
	}

	// its ok also if Controller passes in an object such as a paragraph to copy in
	void DrawingTools::setupParagraph(GraphicID ID, const string& text, shared_ptr<ofxSmartFont> font, int x, int y, int width, const ofColor& color, 
									ofxParagraph::Alignment align, int indent, int leading, int spacing) {
		Wrapper<ofxParagraph> newparagraph(ID);
		newparagraph.setAlignment(align);
		newparagraph.setText(text);
		newparagraph.setFont(font);
		newparagraph.setColor(color);
		newparagraph.setIndent(indent);
		newparagraph.setLeading(leading);
		newparagraph.setSpacing(spacing);
		newparagraph.setPosition(x, y);
		paragraphPlayers.push_back(newparagraph);
	}

	void DrawingTools::drawText(GraphicID ID) {
		drawText(ID, -1, -1);
	}
	void DrawingTools::drawText(GraphicID ID, int x, int y) {
		for (auto& t : textPlayers) {
			if (t.id() == ID) {
				if (t.font != nullptr) {
					ofPushStyle();
					ofSetColor(t.color);
					if (x < 0 || y < 0) {
						t.font->draw(t.text, x, y);
					}
					else {
						t.font->draw(t.text, x, y);
					}
					ofPopStyle();
				}
			}
		}
	}

	void DrawingTools::removeTextPlayers(GraphicID ID) {
		removePlayers(ID, textPlayers);
	}

	// its ok also if Controller passes in an object such as a paragraph to copy in
	void DrawingTools::setupText(GraphicID ID, const string& text, shared_ptr<ofxSmartFont> font, int x, int y, const ofColor& color){
		Wrapper<TextToRender> newtext(ID);
		newtext.font = font;
		newtext.x = x;
		newtext.y = y;
		newtext.color = color;
		newtext.text = text;
		textPlayers.push_back(newtext);

	}
	void Graphics2552::rotateToNormal(ofVec3f normal) {
		normal.normalize();

		float rotationAmount;
		ofVec3f rotationAngle;
		ofQuaternion rotation;

		ofVec3f axis(0, 0, 1);
		rotation.makeRotate(axis, normal);
		rotation.getRotate(rotationAmount, rotationAngle);
		logVerbose("ofRotate " + ofToString(rotationAmount));
		ofRotate(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);
	}

}