#include "model.h"
#include "animation.h"

namespace Software2552 {
	shared_ptr<vector<shared_ptr<PlayItem>>> Playlist::getList() {
		return list;
	}
	// since list is maintained 0 is always current
	shared_ptr<PlayItem> Playlist::getCurrent() {
		if (list == nullptr || list->size() == 0) {
			return nullptr;
		}
		return (*list)[0];
	}

	bool Playlist::readFromScript(const Json::Value &data) {
		list = parse<PlayItem>(data["playList"]);
		return true;
	}


	// helpers
	ofTrueTypeFont& Font::get() {
		if (font == nullptr) {
			getPointer();
		}
		if (font != nullptr) {
			return font->ttf;
		}
		ofTrueTypeFont font2;
		return font2; // default
	}

	shared_ptr<ofxSmartFont> Font::getPointer() {
		if (font == nullptr) {
			font = ofxSmartFont::get(defaultFontFile, defaultFontSize);
			if (font == nullptr) {
				// name is not unqiue, just a helper of some kind I guess
				font = ofxSmartFont::add(defaultFontFile, defaultFontSize, defaultFontName);
			}
		}
		if (font == nullptr) {
			logErrorString("font is null");
		}
		return font;
	}

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
		for (auto a : vec) {
			a->trace();
		}
	}
#endif // _DEBUG

	// read in list of Json values
	template<typename T> shared_ptr<vector<shared_ptr<T>>> parse(const Json::Value &data)
	{
		shared_ptr<vector<shared_ptr<T>>>  results = nullptr;
		if (data.size() > 0) {
			results = std::make_shared<vector<shared_ptr<T>>>();
		}
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			shared_ptr<T> item = std::make_shared<T>();
			item->readFromScript(data[j]);
			results->push_back(item);
		}
		return results;
	}
	// get a string from json
	bool readStringFromJson(string &value, const Json::Value& data) {
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
	DateAndTime::DateAndTime() : Poco::DateTime(0, 1, 1) {
		timeZoneDifferential = 0;
		bc = 0;
	}
	void DateAndTime::operator=(const DateAndTime& rhs) {
		timeZoneDifferential = rhs.timeZoneDifferential;
		bc = rhs.bc;
		assign(rhs.year(), rhs.month(), rhs.day(), rhs.hour(), rhs.minute(), rhs.second(), rhs.microsecond(), rhs.microsecond());
	}
	void Settings::setSettings(Settings* rhs) {
		if (rhs != nullptr) {
			setSettings(*rhs);
		}
	}


	 bool Actor::read(const Json::Value &data) {
		// any actor can have settings set, or defaults used
		Settings::readFromScript(data["settings"]);

		// any actor can have a reference
		references = parse<Reference>(data["references"]);
		 
		// all actors can have a location
		readStringFromJson(player->getLocationPath(), data["location"]);
		if (player->getLocationPath().size() > 0) {
			getPlayer()->loadForDrawing();//bugbug if things get too slow etc do not load here
		}

		// optional sizes, locations, durations for animation etc
		readJsonValue(player->w, data["width"]);
		readJsonValue(player->h, data["height"]);
		float t;
		readJsonValue(t, data["duration"]);
		player->getAnimationHelper()->setObjectLifetime(t);
		float w;
		readJsonValue(w, data["wait"]);
		player->getAnimationHelper()->setWait(w);
		Point3D point;
		point.readFromScript(data["startingPoint"]);
		player->getAnimationHelper()->setPosition(point);

		// read derived class data
		readFromScript(data);

		return true;
	}

	 
	void Colors::update() {
		// clean up deleted items every so often
		for (auto& d : getList()) {
			d->refreshAnimation();
		}
		// remove expired colors
		removeExpiredItems(getList());
	}

	bool PlayItem::readFromScript(const Json::Value &data) {
		READSTRING(keyname, data);
		float lifetime=getObjectLifetime();
		READFLOAT(lifetime, data);
		setObjectLifetime(lifetime);
		return true;
	}
	bool Font::readFromScript(const Json::Value &data) {

		string name;
		int size = defaultFontSize;
		string filename;

		readStringFromJson(name, data["font"]["name"]);
		readStringFromJson(filename, data["font"]["file"]);
		readJsonValue(size, data["font"]["size"]);

		// filename required to create a font, else default font is used
		if (filename.size() != 0) {
			font = ofxSmartFont::get(filename, size);
			if (font == nullptr) {
				// name is not unqiue, just a helper of some kind I guess
				font = ofxSmartFont::add(filename, size, name);
			}
			if (font == nullptr) {
				logErrorString("font file issue");
				return false;
			}
		}
		return true;
	}
	ColorChoice& Settings::getColor() {
		return colors;
	}

	// always return true as these are optional items
	bool Settings::readFromScript(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READSTRING(name, data);
			READSTRING(title, data);
			READSTRING(notes, data);
			font.readFromScript(data["font"]);
			getColor().readFromScript(data);
		}

		return true;
	}
	bool Dates::readFromScript(const Json::Value &data) {
		Settings::readFromScript(data["settings"]);
		timelineDate.readFromScript(data["timelineDate"]); // date item existed
		lastUpdateDate.readFromScript(data["lastUpdateDate"]); // last time object was updated
		itemDate.readFromScript(data["itemDate"]);
		return true;
	}

	bool Reference::readFromScript(const Json::Value &data) {
		Dates::readFromScript(data["dates"]);
		if (Dates::readFromScript(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READSTRING(location, data[STRINGIFY(Reference)]);
			READSTRING(locationPath, data[STRINGIFY(Reference)]);
			READSTRING(source, data[STRINGIFY(Reference)]);
			return true;
		}
		return false;
	}

	bool ColorChoice::readFromScript(const Json::Value &data) {
		string colorgroup;
		READSTRING(colorgroup, data);
		Colors
		setGroup(colorgroup);
		return true;
	}
	bool Point3D::readFromScript(const Json::Value &data) {
		READFLOAT(x, data);
		READFLOAT(y, data);
		READFLOAT(z, data);
		return true;
	}
	bool DateAndTime::readFromScript(const Json::Value &data) {

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
	bool Text::readFromScript(const Json::Value &data) {
		readStringFromJson(((RoleText*)getPlayer())->getText(), data["text"]["str"]);
		return true;
	}

	// return true if text read in
	bool Paragraph::readFromScript(const Json::Value &data) {

		string str;
		readStringFromJson(str, data["text"]["str"]);
		getPlayer()->setText(str);

		int indent;
		int leading;
		int spacing;
		string alignment; // paragraph is a data type in this usage

		if (READINT(indent, data)) {
			getPlayer()->setIndent(indent);
		}
		if (READINT(leading, data)) {
			getPlayer()->setLeading(leading);
		}
		if (READINT(spacing, data)) {
			getPlayer()->setSpacing(leading);
		}
		READSTRING(alignment, data);
		if (alignment == "center") { //bugbug ignore case
			getPlayer()->setAlignment(ofxParagraph::ALIGN_CENTER);
		}
		else if (alignment == "right") { //bugbug ignore case
			getPlayer()->setAlignment(ofxParagraph::ALIGN_RIGHT);
		}

		getPlayer()->setFont(getFontPointer());
		getPlayer()->setColor(Colors::getFontColor());

		return true;
	}

	void Settings::setSettings(const Settings& rhs) {
		// only copy items that change as a default
		font = rhs.font;
		colors = rhs.colors;
	}

	bool Video::readFromScript(const Json::Value &data) {
		float volume=1;//default
		READFLOAT(volume, data);
		getPlayer()->setVolume(volume);
		return true;
	}
	bool Audio::readFromScript(const Json::Value &data) {
		float volume = 1;//default
		READFLOAT(volume, data);
		getPlayer()->setVolume(volume);
		getPlayer()->setMultiPlay(true);
		getPlayer()->setSpeed(ofRandom(0.8, 1.2));// get from data

		return true;
	}

	bool Sphere::readFromScript(const Json::Value &data) {
		float radius = 100;//default
		READFLOAT(radius, data);
		getPlayer()->setRadius(radius);

		float resolution = 100;//default
		READFLOAT(resolution, data);
		getPlayer()->setResolution(resolution);

		return true;
	}

	
}