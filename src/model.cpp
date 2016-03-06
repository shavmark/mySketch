#include "model.h"

namespace Software2552 {
	// helpers
	
	vector<ColorSet> Colors::data; // one set of colors to assure matching color themes
	int Colors::smallest=-1;

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
			item.read(data[j]);
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
		startReadHead();
	}
	void TheSet::draw() {
		background.draw(&colors); // bugbug object at some point may want its own colors
	}
	void TheSet::update() {
		background.update(&colors);// bugbug object at some point may want its own colors
		if (refresh()) {
			colors.getNextColors(ColorSet::ColorType::Warm);
		}
	}

	bool Image::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			if (getLocation().size() > 0) {
				player.load(getLocation());//bugbug if things get too slow etc do not load here
			}
			return true;
		}
		return false;
	}

	bool PlayItem::read(const Json::Value &data) {
		READSTRING(keyname, data);
		return true;
	}
	bool Playlist::read(const Json::Value &data) {
		parse<PlayItem>(list, data["playList"]);
		return true;
	}

	// see if any data in need of drawing is present
	bool GraphicEngines::dataAvailable() {
		return graphicsHelpers.size() > 0;
	}
	

	float GraphicEngines::getLongestWaitTime() {
		return findMaxWait();
	}

	// return true if there is some data 
	bool ReferencedItem::read(const Json::Value &data) {

		if (Dates::read(data)) {

			parse<Reference>(references, data["references"]);
			return true;
		}
		return false; // some thing must be really wrong to return false as these are optional items
	}
	bool Font::read(const Json::Value &data) {

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
	bool Settings::read(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READSTRING(name, data);
			READSTRING(title, data);
			READSTRING(notes, data);
			font.read(data["font"]);
		}

		return true;
	}
	bool Dates::read(const Json::Value &data) {
		Settings::read(data["settings"]);
		timelineDate.read(data["timelineDate"]); // date item existed
		lastUpdateDate.read(data["lastUpdateDate"]); // last time object was updated
		itemDate.read(data["itemDate"]);
		return true;
	}

	bool Reference::read(const Json::Value &data) {

		if (Dates::read(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READSTRING(location, data[STRINGIFY(Reference)]);
			READSTRING(locationPath, data[STRINGIFY(Reference)]);
			READSTRING(source, data[STRINGIFY(Reference)]);
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
	bool Point3D::read(const Json::Value &data) {
		READFLOAT(x, data);
		READFLOAT(y, data);
		READFLOAT(z, data);
		return true;
	}
	bool DateAndTime::read(const Json::Value &data) {

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
	bool Particles::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			return true;
		}
		return true;
	}
	bool Text::read(const Json::Value &data) {
		if (Graphic::read(data)) {
			readStringFromJson(text, data["text"]["str"]);
		}
		return true;
	}
	// return true if text read in
	bool Paragraph::read(const Json::Value &data) {

		Graphic::read(data);

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
		player.setFont(getFontPointer());
		player.setColor(Colors::getCurrentColors().getFontColor());
		player.setPosition(getStartingPoint().x, getStartingPoint().y);

		return true;
	}
	template<typename T> void Scene::createTimeLineItems(const Json::Value &data, const string& key)
	{
		for (Json::ArrayIndex j = 0; j < data[key].size(); ++j) {
			shared_ptr<T> v = std::make_shared<T>();
			v->setSettings(this); // inherit settings
			if (v->read(data[key][j])) {
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

	bool Video::read(const Json::Value &data) {

		Graphic::read(data);
		player.setVolume(getVolume());
		return true;
	}
	bool Audio::read(const Json::Value &data) {

		Graphic::read(data);
		player.setVolume(getVolume());
		return true;
	}
	void GraphicEngines::bumpWaits(uint64_t wait) {
		if (!wait) {
			return;
		}
		bumpWait(wait);
	}
	bool Scene::read(const Json::Value &data) {

		try {
			READSTRING(keyname, data);
			if (keyname == "ClydeBellecourt") {
				int i = 1;
			}
			Settings::read(data);
			// add in a known type if data found
			// keep add in its own vector
			createTimeLineItems<Particles>(data, "particles"); 
			createTimeLineItems<Video>(data, "videos");
			createTimeLineItems<Audio>(data, "audios");
			createTimeLineItems<Paragraph>(data, "paragraphs");
			createTimeLineItems<Image>(data,"images");
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
	Animator::Animator() {
		startTime = 0;
		expired = false;
		paused = false;
		duration = 0; // infinite bugbug duraiton and wait not full baked in yet
		wait = 0;
		rate = 2000;// 2 seconds while developing, but much longer later bugbug set in json
	}
	bool Animator::refresh() {
		uint64_t t = ofGetElapsedTimeMillis() - startTime;//test

		if (t < getWait()) {
			return true;// waiting to start
		}
		if (t > duration) {
			expired = true;
			return false;
		}
		if (t > refreshRate()) {
			startTime = ofGetElapsedTimeMillis();
			return true;
		}
		return false;
	}
	ColorSet& Colors::get() {
		// if first time in set things up
		if (smallest < 0) {
			// if there is data
			if (data.size() > 0) {
				getNextColors(); // sets smallest as needed
			}
		}
		// no data or no match found in data
		if (smallest < 0) {
			return ColorSet();// start with defaults
		}
		++data[smallest];
		return data[smallest];
	}
	// get next color based on type and usage count
	// example: type==cool gets the next cool type, type=Random gets any next color
	ColorSet& Colors::getNextColors(ColorSet::ColorType type) {
		// find smallest of type
		smallest = 0; // code assume some data loaded
		for (int i = 0; i < data.size(); ++i) {
			if (data[smallest].lessThan(data[i], type)) {
				smallest = i;
			}
		}
		return data[smallest];
		//std::vector<ColorSet>::iterator result = std::min_element(std::begin(data), std::end(data));
		//ColorSet cs = *std::min_element(data.begin(), data.end() - 1, ColorSet::searchfn);
		//if (result != data.end()) {
		//return *result;
		//}
	}
	// make a bunch of colors that match using various techniques
	void Colors::setup() {
		// there must always be at least one color
		// set up if there is no data
		if (smallest < 0) {
			//bugbug at some point maybe read from json
			ColorSet cs = ColorSet(ColorSet::Warm,
				ofColor(255, 0, 0), ofColor(0, 255, 0), ofColor(0, 0, 255));
			data.push_back(cs);

			ofColor fore, back, text;
			fore.fromHsb(200, 100, 40); // just made this up for now
			back.fromHsb(100, 100, 50);
			text.fromHsb(200, 100, 100);
			cs.set(ColorSet::Warm, fore, back, text);

			//			ColorSet cs2 = ColorSet(ColorSet::Warm,
			//			ofColor::aliceBlue, ofColor::crimson, ofColor::antiqueWhite);
			ColorSet cs2 = ColorSet(ColorSet::Warm,
				ofColor(0, 255, 0), ofColor(100, 255, 0), ofColor(255, 255, 255));
			data.push_back(cs2);
		}
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
	bool Act::read(const string& fileName) {
		
		ofxJSON json;

		if (!json.open(fileName)) {
			logErrorString("Failed to parse JSON " + fileName);
			return false;
		}

		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			playlist.read(json);
			
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				string keyname;
				if (readStringFromJson(keyname, json["scenes"][i]["keyname"])) {
					PlayItem key(keyname);
					// if a scene is not in the play list do not save it
					std::vector<PlayItem>::iterator finditem =
						find(playlist.getList().begin(), playlist.getList().end(), key);
					if (finditem != playlist.getList().end()) {
						finditem->scene.read(json["scenes"][i]);
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
	void Animator::play() {
		paused = false;
		startReadHead();
	}

	void Animator::pause() {
		float elapsed = ofGetElapsedTimef();
		// if beyond wait time 
		// else hold wait time even after pause
		if (elapsed - (startTime + wait) > 0) {
			wait = 0; // ignore wait time upon return
		}
		paused = true;
	}

	bool Animator::staticOKToRemove(shared_ptr<Animator> me) {
		// duration == 0 means never go away, and start == 0 means we have not started yet
		if (me->duration == 0 || me->startTime == 0) {
			return false; // no time out ever, or we have not started yet
		}
		if (me->expired) {
			return true;
		}
		float elapsed = ofGetElapsedTimef() - me->startTime;
		if (me->wait > elapsed) {
			return false;
		}
		if (me->duration > elapsed) {
			return false;
		}
		return true;

	}

	Graphic::Graphic() : ReferencedItem() {
		//bugbug add a pause where time is suspended, add in rew, play, stop etc also
		height = 0;
		width = 0;
		volume = 0.5;
		myID = ofGetSystemTimeMicros();
	}
	// always return true as these are optional items
	bool Graphic::read(const Json::Value &data) {

		ReferencedItem::read(data);
		READSTRING(type, data);
		READFLOAT(width, data);
		READFLOAT(height, data);
		READSTRING(locationPath, data);
		READFLOAT(volume, data);
		READFLOAT(duration, data);
		READFLOAT(wait, data);
		startingPoint.read(data["startingPoint"]);

		return true;
	}


	bool Animator::okToDraw() {
		if (paused || isExpired()) {
			return false;
		}
		float elapsed = ofGetElapsedTimef();
		// example: ElapsedTime = 100, start = 50, wait = 100, duration 10
		if (elapsed - (startTime + wait) > 0) {
			if (duration == 0) {
				return true; // draw away
			}
			// ok to start but only if we are less than duration
			return (elapsed < startTime + wait + duration);
		}
		return false;
	}


}