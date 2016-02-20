#include "t.h"

namespace Software2552 {
	template<typename T, typename T2>
	void parse(T2& vec, const Json::Value &data)
	{
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			T item;
			item.read(data[j]);
			vec.push_back(item);
		}
	}
	// return true if there is some data 
	bool TimeBaseClass::read(const Json::Value &data) {
		if (CommonData::read(data)) {
			READ(duration, data);
			parse<Reference>(references, data["references"]);
			return true;
		}
		return false;
	}

	void Defaults::read(const Json::Value &data) {
		if (TimeBaseClass::read(data)) {
			READ(font, data);
			READ(italicfont, data);
			READ(boldfont, data);
			READ(fontsize, data);
		}

	}
	// true of any object
	bool CommonData::read(const Json::Value &data) {
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READ(timelineDate, data);
			READ(date, data);
			READ(lastUpdateDate, data);
			READ(name, data);
			READ(notes, data);
			return true;
		}
		return false;
	}

	void Reference::read(const Json::Value &data) {
		if (CommonData::read(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class TimeBaseClass
			READ(url, data);
			READ(location, data);
			READ(source, data);
		}
	}
	void Character::read(const Json::Value &data) {
		if (TimeBaseClass::read(data)) {
			READ(path, data);
			READ(type, data);
			READ(x, data);
			READ(y, data);
			READ(z, data);
		}
	}
	void Text::read(const Json::Value &data) {
		if (TimeBaseClass::read(data)) {
			READ(paragraph, data);
			READ(size, data);
			READ(font, data);
		}
	}
	void Slide::read(const Json::Value &data) {
		if (TimeBaseClass::read(data)) {
			READ(title, data);
			parse<Audio>(audios, data["audio"]);
			parse<Video>(videos, data["video"]);
			parse<Text>(texts, data["text"]);
		}
	}
	void Audio::read(const Json::Value &data) {
		if (TimeBaseClass::read(data)) {
			READ(url, data);
			READ(volume, data);
		}
	
	}
	void kernel::read() {
		string file = "json.json";
		if (json.open(file)) {
			logTrace(json.getRawString());
		}
		else {
			logErrorString("Failed to parse JSON " + file);
			return;
		}

		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			defaults.read(json["defaults"]);

			// build order and list of slide decks
			for (Json::ArrayIndex i = 0; i < json["order"].size(); ++i) {
				for (Json::ArrayIndex j = 0; j < json["order"][i]["names"].size(); ++j) {
					Deck nextdeck(json["order"][i]["names"][j]["name"].asString());
					for (Json::ArrayIndex k = 0; k < json["order"][i][nextdeck.getName()].size(); ++k) {
						Slide s(json["order"][i][nextdeck.getName()][k]["name"].asString()); 
						// store name now, parse slide below, this allows a nice management of slides in and out, and order
						nextdeck.addSlide(s);
					}
					decks.push_back(nextdeck);
				}
			}
			// build decks, break out to make code more readable plus the slides can be read in in any order
			for (auto& currentDeck : decks) {
				for (Json::ArrayIndex i = 0; i < json[currentDeck.getName()].size(); ++i) {
					std::string name = json[currentDeck.getName()][i]["name"].asString();
					Slide newslide(json[currentDeck.getName()][i]["name"].asString());
					// read into matching slide
					std::vector<Slide>::iterator it = find(currentDeck.getSlides().begin(), currentDeck.getSlides().end(), newslide);
					if (it != currentDeck.getSlides().end()) {
						it->read(json[currentDeck.getName()][i]); 
					}
				}
			}
#if 0
			for (auto& d : decks) {
				for (Json::ArrayIndex i = 0; i < json[d.getName()].size(); ++i) {
					std::string name = json[d.getName()][i]["name"].asString();
					Deck d2(name);
					for (std::vector<Deck>::iterator it = decks.begin(); it != decks.end(); ++it) {
						*it->addSlide();
					}
					std::string title = json["treaties"][i]["title"].asString();
					for (Json::ArrayIndex j = 0; j < json[deck.getName()][i]["names"].size(); ++j) {
					}
				}
			}

#endif // 0


			//for (auto& deck : decks) {}
			//set(names, json["order"]["names"]);
			}
		catch (std::exception e) {
			logErrorString(e.what());
			return;
		}


#if 0
		int y = 0;
		for (Json::ArrayIndex i = 0; i < json["treaties"].size(); ++i) {
			std::string title = json["treaties"][i]["title"].asString();
			for (Json::ArrayIndex j = 0; j < json["treaties"][i]["text"].size(); ++j) {
				std::string text = json["treaties"][i]["text"][j]["p"].asString();
				Paragraph2552 p(title, text, screenWidth, y);
				paragraphs.push_back(p);
				y = p.nextRow();
				title.clear(); // only show title one time
			}
		}
#endif // 0
		}
	

}