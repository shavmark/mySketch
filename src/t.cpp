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
	bool TimeLineBaseClass::read(const Json::Value &data) {
		if (CommonData::read(data)) {
			
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

		string name;
		int size = defaultFontSize;
		string filename;

		set(name, data["font"]["name"]);
		set(name, data["font"]["file"]);
		set(size, data["font"]["size"]);

		if (filename.size() != 0) {
			// save read in data
			if (font == nullptr) {
				font = make_shared<ofxSmartFont>();
			}
			font->add(filename, size, name);
		}

		return true;
	}
	// true of any object
	bool CommonData::read(const Json::Value &data) {
		// this should never be a list, lists are processed in other areas
		if (data.size() > 0 && data.type() != Json::objectValue)	{
			PrintJSONValue(data);
			string error = "invalid data ";
			if (data.isConvertibleTo(Json::stringValue)) {
				error += data.asString();
				logErrorString(error); //bugbug put in the name here to help debug json files
			}
			else {
				PrintJSONTree(data);
			}
			return false;
		}
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

	bool Reference::read(const Json::Value &data) {
		if (CommonData::read(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class TimeLineBaseClass
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
		if (TimeLineBaseClass::read(data)) {
			READ(type, data);
			READ(duration, data);
			READ(delay, data);
			READ(x, data);
			READ(y, data);
			READ(z, data);
			READ(foreground, data); // for now just use color  names (see ofColor) bugbug see how color pans out
			READ(background, data);

			return true;
		}
		return false;
	}

	bool Text::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			READ(paragraph, data);
			READ(size, data);
			READ(font, data);
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