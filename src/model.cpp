#include "model.h"

// this is the data model module, it should not contain drawing objects

namespace Software2552 {
	// helpers

	//bugbug todo weave into errors, even on release mode as anyone can break a json file
	void echoValue(const Json::Value &data, bool isError) {
		//Json::Value::Members m = data.getMemberNames();
		try {
			if (data.isString()) {
				tracer("type=string, value=\"" + data.asString() + "\">", isError);
			}
			else if (data.isBool()) {
				tracer("type=bool, value=" + ofToString(data.asBool()) + ">", isError);
			}
			else if (data.isInt()) {
				tracer("type=int, value=" + ofToString(data.asInt()) + ">", isError);
			}
			else if (data.isDouble()) {
				tracer("type=float, value=" + ofToString(data.asDouble()) + ">", isError);
			}
			else if (data.isArray()) {
				tracer("type=array>", isError);
			}
			else if (data.isObject()) {
				tracer("type=objectvalue(name/value pair), value=" + ofToString(data.asString()) + ">", isError);
			}
			else if (data.isNull()) {
				tracer("type=null>", isError);
			}
			else {
				tracer("type=unsupported, type is " + ofToString(data.type()) + ">", isError);
			}
		}
		catch (std::exception e) {
			logErrorString(e.what());
		}


	}
	bool echoJSONTree(const string& functionname, const Json::Value &root, bool isError)
	{
		tracer("<Parse name=\"" + functionname + "\">", isError); // kick it back as xml, easier to read by a human? 

		if (root.size() > 0) {

			for (Json::ValueIterator itr = root.begin(); itr != root.end(); itr++) {
				string member = itr.memberName();
				tracer("<subvalue name=\"" + member + "\">", isError);
				echoValue(itr.key());
				tracer("</subvalue>", isError);
				echoJSONTree(functionname, *itr, isError);
			}
			return true;
		}
		else {
			echoValue(root);
		}
		tracer("</Parse>", isError);
		return true;
	}

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
	// read in list of Json values
	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data)
	{
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			T item;
			item.read(data[j]);
			vec.push_back(item);
		}
	}
	// get a string from json
	bool setString(string &value, const Json::Value& data) {
		if (readJsonValue(value, data)) {
			value = data.asString();
			return true;
		}
		return false;
	}
	template<typename T> bool readJsonValue(T &value, const Json::Value& data) {
		try {
			if (!data.empty()) {
				switch (data.type()) {
				case Json::nullValue:
					logTrace("nullValue");
					break;
				case Json::booleanValue:
					value = data.asBool();
					break;
				case Json::stringValue:
					return true; // templates get confused to readJsonValue the string else where, use this to validate
					break;
				case Json::intValue:
					value = data.asInt();
					break;
				case Json::realValue:
					value = data.asFloat();
					break;
				case Json::objectValue:
					logErrorString("objectValue called at wrong time");
					break;
				case Json::arrayValue:
					logErrorString("arrayValue called at wrong time");
					break;
				default:
					// ignore?
					logErrorString("value not found");
					break;
				}
				return true;
			}
		}
		catch (std::exception e) {
			echoJSONTree(__FUNCTION__, data, true);
			logErrorString(e.what());
		}
		return false;
	}

	void Scene::setup() {
		setupVector(audios);
		setupVector(videos);
		setupVector(paragraphs);
		setupVector(images);
		setupVector(graphics); //bugbug create built in graphics like those in the book ("Smoke")
		setupVector(characters);
		setupVector(texts);
	}
#if _DEBUG
	// echo object (debug only) bugbug make debug only
	void Story::trace() {
		logVerbose(STRINGIFY(Story));
		traceVector(acts);
	}
#endif
	void Act::setup() {
		setupVector(scenes);
	}
	void Act::update() {
		updateVector(scenes); // give all abjects a change at update
	}
	bool Act::dataAvailable() {
		// see if any scenes have any data
		for (auto& scene : scenes) {
			if (scene.dataAvailable()) {
				return true;
			}
		}
		return false;
	}
	bool PlayItem::read(const Json::Value &data) {
		READSTRING(keyname, data);
		return true;
	}
	bool Playlist::read(const Json::Value &data) {
		parse<PlayItem>(playList, data["playList"]);
		return true;
	}

	// see if any data in need of drawing is present
	bool Scene::dataAvailable() {
		return audios.size() > 0 ||
			videos.size() > 0 ||
			paragraphs.size() > 0 ||
			images.size() > 0 ||
			texts.size() > 0 ||
			graphics.size() > 0 ||
			characters.size() > 0;
	}
	void Story::setup() {
		read();
		setupVector(acts);
	};
	void Story::update() {
		updateVector(acts);
	}
	void Story::read() {
		echo("read a story");

		Act scenes(getSettings(),"main deck");
		// code in the list of items to make into the story here. 
		scenes.read("json.json");
		acts.push_back(scenes);
	}

	void Scene::update() {
		// remove all timed out items 
		audios.erase(std::remove_if(audios.begin(), audios.end(), Graphic::okToRemove), audios.end());
		videos.erase(std::remove_if(videos.begin(), videos.end(), Graphic::okToRemove), videos.end());
		characters.erase(std::remove_if(characters.begin(), characters.end(), Graphic::okToRemove), characters.end());
		images.erase(std::remove_if(images.begin(), images.end(), Graphic::okToRemove), images.end());
		graphics.erase(std::remove_if(graphics.begin(), graphics.end(), Graphic::okToRemove), graphics.end());
		paragraphs.erase(std::remove_if(paragraphs.begin(), paragraphs.end(), Graphic::okToRemove), paragraphs.end());
		texts.erase(std::remove_if(texts.begin(), texts.end(), Graphic::okToRemove), texts.end());
		updateVector(audios);
		updateVector(videos);
		updateVector(paragraphs);
		updateVector(images);
		updateVector(graphics);
		updateVector(characters);
		updateVector(texts);

	}

	// return true if there is some data 
	bool ReferencedItem::read(const Json::Value &data) {
		ECHOAll(data);

		if (Settings::read(data[Settings::JsonName])) {

			parse<Reference>(references, data["references"]);
			return true;
		}
		return false; // some thing must be really wrong to return false as these are optional items
	}
	// return true if location found
	bool Image::read(const Json::Value &data) {
		ECHOAll(data);

		if (Graphic::read(data)) {
			return READSTRING(location, data);
		}
		return false;
	}
	bool Font::read(const Json::Value &data) {
		ECHOAll(data);

		string name;
		int size = defaultFontSize;
		string filename;

		setString(name, data["font"]["name"]);
		setString(name, data["font"]["file"]);
		readJsonValue(size, data["font"]["size"]);

		if (filename.size() != 0) {
			if (!fontExists(filename, size)) {
				// name is not unqiue, just a helper of some kind I guess
				font = ofxSmartFont::add(filename, size, name);
			}
		}
		return true;
	}
	bool Graphic::okToDraw() {
		if (duration == 0) {
			return true; // always draw
		}
		if (start <= 0) {
			return false; // not started yet
		}
		// still going??
		float f = ofGetElapsedTimef();
		return start + delay + duration > ofGetElapsedTimef();
	}
	// always return true as these are optional items
	bool Settings::read(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READSTRING(name, data);
			READSTRING(notes, data);
			READFLOAT(duration, data);
			timelineDate.read(data["timelineDate"]); // date item existed
			lastUpdateDate.read(data["lastUpdateDate"]); // last time object was updated
			itemDate.read(data["itemDate"]);
			startingPoint.read(data["startingPoint"]);
			foregroundColor.read(data["foreground"]);
			backgroundColor.read(data["background"]);
			font.read(data["font"]);
		}

		return true;
	}
	
	bool Reference::read(const Json::Value &data) {
		ECHOAll(data);

		if (Settings::read(data[Settings::JsonName])) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READSTRING(location, data[STRINGIFY(Reference)]);
			READSTRING(locationPath, data[STRINGIFY(Reference)]);
			READSTRING(source, data[STRINGIFY(Reference)]);
			return true;
		}
		return false;
	}
	bool Character::read(const Json::Value &data) {
		ECHOAll(data);

		if (Graphic::read(data)) {
			return true;
		}
		return false;
	}
	bool Color::read(const Json::Value &data) {
		ECHOAll(data);
		READINT(r, data);
		READINT(g, data);
		READINT(b, data);
		return true;
	}
	bool Point3D::read(const Json::Value &data) {
		ECHOAll(data);
		READFLOAT(x, data);
		READFLOAT(y, data);
		READFLOAT(z, data);
		return true;
	}
	bool DateAndTime::read(const Json::Value &data) {
		ECHOAll(data);

		if (READINT(bc, data)) {
			return true;
		}

		string str; // scratch varible, enables either string or ints to pull a date
		if (READSTRING(str, data)) {
			if (!Poco::DateTimeParser::tryParse(str, *this, timeZoneDifferential)) {
				logErrorString("invalid AD date " + str);
				return false;
			}
			makeUTC(timeZoneDifferential);
		}

		return true;
	}
	Graphic::Graphic() : ReferencedItem() {
		//bugbug add a pause where time is suspended, add in rew, play, stop etc also
		start = 0; // force reset to be called to make sure timing is right, 0 means not started
		delay = 0;
		height = 0;
		width = 0;
		myID = ofGetSystemTimeMicros();

	}
	// always return true as these are optional items
	bool Graphic::read(const Json::Value &data) {
		ECHOAll(data);

		ReferencedItem::read(data);
		READSTRING(type, data);
		READFLOAT(duration, data);
		READFLOAT(delay, data);
		READFLOAT(width, data);
		READFLOAT(height, data);
		return true;
	}
	Paragraph::Paragraph() :Text() {
		indent = 40;
		leading = 16;
		spacing = 6;
		alignment = "left"; // json key
	}
	// return true if text read
	bool Text::read(const Json::Value &data) {
		ECHOAll(data);

		if (Graphic::read(data)) {
			// if no text string do not save the defaults
			// so return true only if a string is found at this point
			return setString(str, data["text"]["str"]);
		}
		return true;
	}
	// return true if text read in
	bool Paragraph::read(const Json::Value &data) {
		ECHOAll(data);
		bool textReadIn = Text::read(data);
		READINT(indent, data);
		READINT(leading, data);
		READINT(spacing, data);
		READSTRING(alignment, data);
		return textReadIn;
	}
	template<typename T, typename T2> void Scene::createTimeLineItems(T2& vec, const Json::Value &data, const string& key)
	{
		logTrace("createTimeLineItems for " + key);

		for (Json::ArrayIndex j = 0; j < data[key].size(); ++j) {
			T item;
			item.setSettings(getSettings()); // copy default settings into object
			if (item.read(data[key][j])) {
				// only save if data was read in 
				vec.push_back(item);
			}
		}
	}
	void Settings::setSettings(const Settings& rhs) {
		font = rhs.font;
		timelineDate = rhs.timelineDate; // date item existed
		lastUpdateDate = rhs.lastUpdateDate; // last time object was updated
		name = rhs.name; // any object can have a name, note, date, reference, duration
		notes = rhs.notes;
		itemDate = rhs.itemDate; 
		foregroundColor = rhs.foregroundColor;
		backgroundColor = rhs.backgroundColor;
		duration = rhs.duration;
	}

	bool Scene::read(const Json::Value &data) {
		ECHOAll(data);

		try {
			if (SettingsAndTitle::read(data)) {
				READSTRING(keyname, data);
				if (keyname == "ClydeBellecourt") {
					int i = 1;
				}

				READBOOL(wait, data);
				createTimeLineItems<Audio>(audios, data, "audios");
				createTimeLineItems<Video>(videos, data, "videos");
				createTimeLineItems<Paragraph>(paragraphs, data, "paragraphs");
				createTimeLineItems<Image>(images, data,"images");
				createTimeLineItems<Graphic>(graphics, data, "graphics");
				createTimeLineItems<Text>(texts, data, "texts");
				createTimeLineItems<Character>(characters, data, "characters");
				return true;
			}
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}

		return false;
	}
	bool Audio::read(const Json::Value &data) {
		ECHOAll(data);

		if (Image::read(data)) {
			READFLOAT(volume, data);
			return true;
		}
		return false;
	
	}
	// read as many jason files as needed, each becomes a deck
	bool Act::read(const string& fileName) {
		
		ofxJSON json;

		if (!json.open(fileName)) {
			logErrorString("Failed to parse JSON " + fileName);
			return false;
		}

		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			SettingsAndTitle::read(json); // read base class
			playlist.read(json);

			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				Scene scene;
				scene.read(json["scenes"][i]);
				add(scene);
			}
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}

		return true;
	}
	

}