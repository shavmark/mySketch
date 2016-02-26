#include "view.h"

// this is the drawing model module, it should not contain data objects


namespace Software2552 {
	//https://sites.google.com/site/ofauckland/examples/curly-moving-particles code this in soon to learn the cool stuff we can add bugbug
	//https://github.com/tesseract-ocr/tesseract

	void DrawingTools::removeVideoPlayers(GraphicID ID) {
		vector<Wrapper<ofVideoPlayer>>::iterator it = videoPlayers.begin();
		while (it != videoPlayers.end()) {
			if (!it->id() == ID) {
				it = videoPlayers.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void DrawingTools::drawVideo(GraphicID ID, int x, int y) {
		for (auto& v : videoPlayers) {
			if (v.id() == ID) {
				v.draw(x, y);
			}
		}
	}
	void DrawingTools::updateVideo(GraphicID ID) {
		for (auto& v : videoPlayers) {
			if (v.id() == ID) {
				v.update();
			}
		}
	}
	void DrawingTools::setupVideoPlayer(GraphicID ID, float vol, const string&location) {
		Wrapper<ofVideoPlayer> player(ID);
		player.setVolume(vol);
		player.load(location);
		player.play();
		videoPlayers.push_back(player);

	}

	void DrawingTools::drawParagraph(GraphicID ID) {
		for (auto& t : paragraphPlayers) {
			if (t.id() == ID) {
				t.draw();
			}
		}
	}
	void DrawingTools::drawParagraph(GraphicID ID, const ofVec2f& point) {
		for (auto& t : paragraphPlayers) {
			if (t.id() == ID) {
				t.draw(point.x, point.y);
			}
		}
	}
		
	void DrawingTools::removeParagraphPlayers(GraphicID ID) {
		vector<Wrapper<ofxParagraph>>::iterator it = paragraphPlayers.begin();
		while (it != paragraphPlayers.end()) {
			if (!it->id() == ID) {
				it = paragraphPlayers.erase(it);
			}
			else {
				++it;
			}
		}
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
		for (auto& t : textPlayers) {
			if (t.id() == ID) {
				ofPushStyle();
				ofSetColor(t.color);
				t.font->draw(t.text, t.x, t.y);
				ofPopStyle();
			}
		}
	}
	void DrawingTools::drawText(GraphicID ID, const ofVec2f& point) {
		for (auto& t : textPlayers) {
			if (t.id() == ID) {
				if (t.font != nullptr) {
					ofPushStyle();
					ofSetColor(t.color);
					t.font->draw(t.text, t.x, t.y);
					ofPopStyle();
				}
			}
		}
	}

	void DrawingTools::removeTextPlayers(GraphicID ID) {
		vector<Wrapper<TextToRender>>::iterator it = textPlayers.begin();
		while (it != textPlayers.end()) {
			if (!it->id() == ID) {
				it = textPlayers.erase(it);
			}
			else {
				++it;
			}
		}
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