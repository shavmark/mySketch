#include "model.h"
#include "animation.h"

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
		for (auto a : vec) {
			a->trace();
		}
	}
#endif // _DEBUG

	// read in list of Json values
	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data)
	{
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			T item;
			item.readFromScript(data[j]);
			vec.push_back(item);
		}
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
	void TheSet::setup() {
		ofSetFrameRate(framerate);
		startAnimation();
	}
	void TheSet::draw() {
		if (okToDraw()) {
			background.draw(&colors); // bugbug object at some point may want its own colors
		}
	}
	void TheSet::update() {
		//bugbug work on this later colors.update();
		//background.update(&colors);// bugbug object at some point may want its own colors
		//if (refresh()) {
			//colors.getNextColors();
		//}
	}

	 template <class T> bool Actor<T>::readFromScript(const Json::Value &data) {
		// all actors can have a location
		readStringFromJson(player->getLocation(), data["location"]);
		if (player->getLocation().size() > 0) {
			getPlayer()->load(player->getLocation());//bugbug if things get too slow etc do not load here
		}
		return true;
	}

	bool Picture::readFromScript(const Json::Value &data) {
		return true;
	}
	void Colors::update() {
		// clean up deleted items every so often
		for (auto& d : getList()) {
			d.refreshAnimation();
		}
		// remove expired colors
		Animator a(false);
		a.removeExpiredItems(getList());
	}

	bool PlayItem::readFromScript(const Json::Value &data) {
		READSTRING(keyname, data);
		return true;
	}
	bool Playlist::readFromScript(const Json::Value &data) {
		parse<PlayItem>(list, data["playList"]);
		return true;
	}

	// see if any data in need of drawing is present
	bool GraphicEngines::dataAvailable() {
		return graphicsHelpers.size() > 0;
	}
	void GraphicEngines::removeExpiredItems() {
		Animator a(false);
		a.removeExpiredPtrToItems(get());
	}


	uint64_t GraphicEngines::getLongestWaitTime() {
		return findMaxWait();
	}

	// return true if there is some data 
	bool ReferencedItem::readFromScript(const Json::Value &data) {
		parse<Reference>(references, data["references"]);
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
	// always return true as these are optional items
	bool Settings::readFromScript(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READSTRING(name, data);
			READSTRING(title, data);
			READSTRING(notes, data);
			font.readFromScript(data["font"]);
		}

		return true;
	}
	bool Dates::readFromScript(const Json::Value &data) {
		timelineDate.readFromScript(data["timelineDate"]); // date item existed
		lastUpdateDate.readFromScript(data["lastUpdateDate"]); // last time object was updated
		itemDate.readFromScript(data["itemDate"]);
		return true;
	}

	bool Reference::readFromScript(const Json::Value &data) {
		Settings::readFromScript(data["settings"]);
		if (Dates::readFromScript(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READSTRING(location, data[STRINGIFY(Reference)]);
			READSTRING(locationPath, data[STRINGIFY(Reference)]);
			READSTRING(source, data[STRINGIFY(Reference)]);
			return true;
		}
		return false;
	}
	bool Character::readFromScript(const Json::Value &data) {

		if (Graphic::readFromScript(data)) {
			return true;
		}
		return false;
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
		if (Graphic::readFromScript(data)) {
			readStringFromJson(text, data["text"]["str"]);
		}
		return true;
	}
	void Text::draw() {
		if (okToDraw()) {
			player.draw(this);
		}
	}

	void Paragraph::draw() {
		if (okToDraw()) {
			player.setFont(getFontPointer());
			player.setColor(Colors::getFontColor());
			player.setPosition(pos.x, pos.y);
			player.draw(pos.x, pos.y);
		}
	}

	// return true if text read in
	bool Paragraph::readFromScript(const Json::Value &data) {

		Graphic::readFromScript(data);

		string str;
		readStringFromJson(str, data["text"]["str"]);
		player.setText(str);

		int indent;
		int leading;
		int spacing;
		string alignment; // paragraph is a data type in this usage

		if (READINT(indent, data)) {
			player.setIndent(indent);
		}
		if (READINT(leading, data)) {
			player.setLeading(leading);
		}
		if (READINT(spacing, data)) {
			player.setSpacing(leading);
		}
		READSTRING(alignment, data);
		if (alignment == "center") { //bugbug ignore case
			player.setAlignment(ofxParagraph::ALIGN_CENTER);
		}
		else if (alignment == "right") { //bugbug ignore case
			player.setAlignment(ofxParagraph::ALIGN_RIGHT);
		}

		return true;
	}
	template<typename T> void Scene::createTimeLineItems(const Json::Value &data, const string& key)
	{
		for (Json::ArrayIndex j = 0; j < data[key].size(); ++j) {
			shared_ptr<T> v = std::make_shared<T>();
			v->setSettings(this); // inherit settings
			if (v->readFromScript(data[key][j])) {
				// only save if data was read in 
				getDrawingEngines()->get().push_back(v);
			}
		}
	}

	void Settings::setSettings(const Settings& rhs) {
		// only copy items that change as a default
		font = rhs.font;
	}
	void Audio::setup() {
		if (!getPlayer().load(getLocation())) {
			logErrorString("setup audio Player");
		}
		getPlayer().play();
		getPlayer().setMultiPlay(true);
		getPlayer().setSpeed(ofRandom(0.8, 1.2));//bugbug get from data
	}
	uint64_t Video::getTimeBeforeStart(uint64_t t) {

		// if json sets a wait use it
		if (getWait() > 0) {
			setIfGreater(t, getWait());
		}
		else {
			// will need to load it now to get the true lenght
			if (!getPlayer().isLoaded()) {
				getPlayer().load(getLocation());
			}
			uint64_t duration = getPlayer().getDuration();
			setIfGreater(t, duration);
		}
		return t;
	}

	void Video::setup() {
		if (!player.isLoaded()) {
			if (!player.load(getLocation())) {
				logErrorString("setup video Player");
			}
		}
	};

	bool Video::readFromScript(const Json::Value &data) {
		Actor::readFromScript(data);
		float volume=1;//default
		READFLOAT(volume, data);
		getPlayer()->setVolume(volume);
		return true;
	}
	bool Audio::readFromScript(const Json::Value &data) {
		Actor::readFromScript(data);
		float volume = 1;//default
		READFLOAT(volume, data);
		getPlayer()->setVolume(volume);
		return true;
	}
	void GraphicEngines::bumpWaits(uint64_t wait) {
		if (!wait) {
			return;
		}
		bumpWait(wait);
	}
	bool Scene::readFromScript(const Json::Value &data) {

		try {
			READSTRING(keyname, data);
			if (keyname == "ClydeBellecourt") {
				int i = 1;
			}
			Settings::readFromScript(data);
			// add in a known type if data found
			// keep add in its own vector
			createTimeLineItems<Video>(data, "videos");
			createTimeLineItems<Audio>(data, "audios");
			createTimeLineItems<Paragraph>(data, "paragraphs");
			createTimeLineItems<Picture>(data,"images");
			createTimeLineItems<Text>(data, "texts");
			createTimeLineItems<Character>(data, "characters");
			return true;
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}

		return false;
	}
	Scene::Scene(const string&keynameIn) {
		keyname = keynameIn;
		engines = nullptr;
	}
	Scene::Scene() {
		engines = nullptr;
	}
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

	void Scene::setEngine(shared_ptr<GraphicEngines> enginesIn) {
		if (enginesIn == nullptr) {
			engines = std::make_shared<GraphicEngines>();
		}
		else {
			engines = enginesIn;
		}
	}

	// read as many jason files as needed, each becomes a deck
	bool Act::readFromScript(const string& fileName) {
		
		ofxJSON json;

		if (!json.open(fileName)) {
			logErrorString("Failed to parse JSON " + fileName);
			return false;
		}

		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			playlist.readFromScript(json);
			
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				string keyname;
				if (readStringFromJson(keyname, json["scenes"][i]["keyname"])) {
					PlayItem key(keyname);
					// if a scene is not in the play list do not save it
					std::vector<PlayItem>::iterator finditem =
						find(playlist.getList().begin(), playlist.getList().end(), key);
					if (finditem != playlist.getList().end()) {
						finditem->scene.readFromScript(json["scenes"][i]);
					}
				}
			}
			// space out waits, but calc out video and other lengths (will result in loading videos)
			float wait = 0;
			for (auto& item : playlist.getList()) {
				item.scene.getDrawingEngines()->bumpWaits(wait);
				wait = item.scene.getDrawingEngines()->getLongestWaitTime();
			}

		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}

		return true;
	}
	Graphic::Graphic() : ReferencedItem() {
		//bugbug add a pause where time is suspended, add in rew, play, stop etc also
		volume = 0.5;
	}
	// always return true as these are optional items
	bool Graphic::readFromScript(const Json::Value &data) {

		ReferencedItem::readFromScript(data);
		readJsonValue(w, data["width"]);
		readJsonValue(h, data["height"]);
		readJsonValue(getDuration(), data["duration"]);
		readJsonValue(getWait(), data["wait"]);
		Point3D point;
		point.readFromScript(data["startingPoint"]);
		pos.x = point.x;
		pos.y = point.y;
		READSTRING(locationPath, data);
		READFLOAT(volume, data);
		return true;
	}


}