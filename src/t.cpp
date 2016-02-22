#include "t.h"

namespace Software2552 {

	// helpers
#if _DEBUG
	template<typename T> void traceVector(T& vec) {
		for (auto& a : vec) {
			a.trace();
		}
	}
#endif // _DEBUG

	template<typename T> void setupVector(T& vec) {
		for (auto& a : vec) {
			a.setup();
		}
	}
	template<typename T> void updateVector(T& vec) {
		for (auto& a : vec) {
			a.update();
		}
	}
	template<typename T> void drawVector(T& vec) {
		for (auto& a : vec) {
			a.draw();
		}
	}

	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data)
	{
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			T item;
			item.read(data[j]);
			vec.push_back(item);
		}
	}
#if _DEBUG
	void PrintJSONValue(const Json::Value &val)
	{
		if (val.isString()) {
			basicTrace("asString " + val.asString());
		}
		else if (val.isBool()) {
			basicTrace("asBool " + ofToString(val.asBool()));
		}
		else if (val.isInt()) {
			basicTrace("int " + ofToString(val.asInt()));
		}
		else if (val.isUInt()) {
			basicTrace("uint "+ofToString(val.asUInt()));
		}
		else if (val.isDouble()) {
			basicTrace("double "+ofToString(val.asDouble()));
		}
		else
		{
			basicTrace("unknown type="+ofToString(val.type()));
		}
	}

	bool PrintJSONTree(const Json::Value &root)
	{
		if (root.size() > 0) {
			
			for (Json::ValueIterator itr = root.begin(); itr != root.end(); itr++) {
				basicTrace("subvalue(");
				PrintJSONValue(itr.key());
				basicTrace(")");
				PrintJSONTree(*itr);
			}
			return true;
		}
		else {
			basicTrace("root");
			PrintJSONValue(root);
		}
		return true;
	}
#else
	bool PrintJSONTree(const Json::Value &root) {}
	void PrintJSONValue(const Json::Value &val) {}
#endif
	void Scene::setup() {
		setupVector(audios);
		setupVector(videos);
		setupVector(texts);
		setupVector(images);
		setupVector(graphics);
		setupVector(characters);
	}
#if _DEBUG
	// echo object (debug only) bugbug make debug only
	void Story::trace() {
		basicTrace(STRINGIFY(Story));
		traceVector(story);
	}
#endif
	void Scene::update() {
		// remove all timed out items 
		audios.erase(std::remove_if(audios.begin(), audios.end(), Graphic::okToRemove), audios.end());
		videos.erase(std::remove_if(videos.begin(), videos.end(), Graphic::okToRemove), videos.end());
		characters.erase(std::remove_if(characters.begin(), characters.end(), Graphic::okToRemove), characters.end());
		images.erase(std::remove_if(images.begin(), images.end(), Graphic::okToRemove), images.end());
		graphics.erase(std::remove_if(graphics.begin(), graphics.end(), Graphic::okToRemove), graphics.end());
		texts.erase(std::remove_if(texts.begin(), texts.end(), Graphic::okToRemove), texts.end());
		updateVector(audios);
		updateVector(videos);
		updateVector(texts);
		updateVector(images);
		updateVector(graphics);
		updateVector(characters);
	}
	void Scene::draw() {
		drawVector(audios);
		drawVector(videos);
		drawVector(texts);
		drawVector(images);
		drawVector(graphics);
		drawVector(characters);
	}

	// return true if there is some data 
	bool ReferencedItem::read(const Json::Value &data) {
		if (Settings::read(data)) {
			
			parse<Reference>(references, data["references"]);
			return true;
		}
		return false;
	}
	bool Image::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			READ(url, data);
			return true;
		}
		return false;
	}
	bool Settings::read(const Json::Value &data) {
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READ(timelineDate, data);
			READ(date, data);
			READ(lastUpdateDate, data);
			READ(name, data);
			READ(notes, data);
			READ(duration, data);

			int r=0, g = 0, b = 0;
			set(r, data["foreground"]["r"]);
			set(g, data["foreground"]["g"]);
			set(b, data["foreground"]["b"]);
			foregroundColor.set(r, g, b);

			r = 0, g = 0, b = 0;
			set(r, data["background"]["r"]);
			set(g, data["background"]["g"]);
			set(b, data["background"]["b"]);
			backgroundColor.set(r, g, b);

			string name;
			int size = defaultFontSize;
			string filename;

			set(name, data["font"]["name"]);
			set(name, data["font"]["file"]);
			set(size, data["font"]["size"]);

			if (filename.size() != 0) {
				//ofxSmartFont handles memory for us
				font = ofxSmartFont::add(filename, size, name);
			}
			return true;
		}

		return false;
	}
	
	bool Reference::read(const Json::Value &data) {
		if (Settings::read(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READ(url, data);
			READ(location, data);
			READ(source, data);
			return true;
		}
		return false;
	}
	bool Character::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			return true;
		}
		return false;
	}

	bool Graphic::read(const Json::Value &data) {
		if (ReferencedItem::read(data)) {
			READ(type, data);
			READ(duration, data);
			READ(delay, data);
			READ(x, data);
			READ(y, data);
			READ(z, data);

			return true;
		}
		return false;
	}

	bool Text::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			string paragraph; // read in text
			READ(paragraph, data);
			if (paragraph.size() > 0){
				ofParagraph.setText(paragraph);
				ofParagraph.setFont(font); // use current font
				ofParagraph.setColor(foregroundColor);
				int indent=-1;
				READ(indent, data);
				if (indent > -1) {
					ofParagraph.setIndent(indent);
				}
				int leading = -1;
				READ(leading, data);
				if (leading > -1) {
					ofParagraph.setLeading(leading);
				}
				int spacing = -1;
				READ(spacing, data);
				if (spacing > -1) {
					ofParagraph.setSpacing(spacing);
				}
				string alignment;
				READ(alignment, data);
				if (alignment == "left") { //bugbug ignore case
					ofParagraph.setAlignment(ofxParagraph::ALIGN_LEFT);
				}
				else if (alignment == "center") { //bugbug ignore case
					ofParagraph.setAlignment(ofxParagraph::ALIGN_CENTER);
				}
				else if (alignment == "right") { //bugbug ignore case
					ofParagraph.setAlignment(ofxParagraph::ALIGN_RIGHT);
				}
				int width = -1;
				READ(width, data);
				if (width > -1) {
					ofParagraph.setWidth(width);
				}
				// read in base class
				if (x > -1) {
					ofParagraph.x = x;
				}
				if (y > -1) {
					ofParagraph.x = y;
				}
			}
			// add more here like indent as we learn more
			return true;
		}
		return false;
	}
	bool Scene::read(const Json::Value &data) {
		if (Timeline::read(data)) {
			parse<Audio>(audios, data["audio"]);
			parse<Video>(videos, data["video"]);
			parse<Text>(texts, data["text"]);
			parse<Image>(images, data["images"]);
			parse<Graphic>(graphics, data["graphics"]);
			return true;
		}
		return false;
	}
	bool Audio::read(const Json::Value &data) {
		if (Image::read(data)) {
			READ(volume, data);
			return true;
		}
		return false;
	
	}
	// read as many jason files as needed, each becomes a deck
	bool Scenes::read(const string& fileName) {
		ofxJSON json;

		if (json.open(fileName)) {
			logTrace(json.getRawString());
		}
		else {
			logErrorString("Failed to parse JSON " + fileName);
			return false;
		}

		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			Timeline::read(json);

			// build order and list of slide scenes
			for (Json::ArrayIndex i = 0; i < json["order"].size(); ++i) {
				Scene s(getSettings(), getSharedTools(), json["order"][i].asString());
				// scenes stored in order they should be run
				add(Scene(getSettings(), getSharedTools(), json["order"][i].asString()));
			}
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				Scene lookupslide(json["scenes"][i].asString());
				// read into matching slide
				std::vector<Scene>::iterator it = find(getScenes().begin(), getScenes().end(), lookupslide);
				if (it != getScenes().end()) {
					// Settings can appear here too
					it->read(json["scenes"][i]); // update slide in place
				}
			}
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}

		return true;
	}
	

}