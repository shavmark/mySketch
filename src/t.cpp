#include "t.h"

namespace Software2552 {
	void kernel::set(vector <string> &value, const Json::ArrayIndex &data) {
		for (Json::ArrayIndex i = 0; i < data; ++i) {
			//value.push_back(data[i].asString());
		}
	}

	void kernel::setup() {
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
			set(defaults.font, json["defaults"]["font"]);
			set(defaults.duration, json["defaults"]["duration"]);
			
			// build order and list of slide decks
			for (Json::ArrayIndex i = 0; i < json["order"].size(); ++i) {
				for (Json::ArrayIndex j = 0; j < json["order"][i]["names"].size(); ++j) {
					deck nextdeck(json["order"][i]["names"][j]["n"].asString());
					for (Json::ArrayIndex k = 0; k < json["order"][i][nextdeck.getName()].size(); ++k) {
						slide s(json["order"][i][nextdeck.getName()][k]["s"].asString());
						nextdeck.addSlide(s);
					}
					decks.push_back(nextdeck);
				}
			}
			// build decks
			for (auto& currentDeck : decks) {
				for (Json::ArrayIndex i = 0; i < json[currentDeck.getName()].size(); ++i) {
						std::string name = json[currentDeck.getName()][i]["name"].asString();
						slide newslide(json[currentDeck.getName()][i]["name"].asString());
						std::vector<slide>::iterator it = find(currentDeck.getSlides().begin(), currentDeck.getSlides().end(), newslide);
						if (it != currentDeck.getSlides().end()) {
							it->title = json[currentDeck.getName()][i]["title"].asString();
							it->timeline = json[currentDeck.getName()][i]["timeline"].asString();
							it->lastupdate = json[currentDeck.getName()][i]["lastupdate"].asString();
							readReference(it->ref, json[currentDeck.getName()][i]);
							//for (Json::ArrayIndex j = 0; j < json[currentDeck.getName()][i]["reference"].size(); ++j) {
							//}
						}
				}
			}
#if 0
			for (auto& d : decks) {
				for (Json::ArrayIndex i = 0; i < json[d.getName()].size(); ++i) {
					std::string name = json[d.getName()][i]["name"].asString();
					deck d2(name);
					for (std::vector<deck>::iterator it = decks.begin(); it != decks.end(); ++it) {
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