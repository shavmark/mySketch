#include "t.h"

namespace Software2552 {

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