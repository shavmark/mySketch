#include "model.h"
#include "animation.h"
#include "scenes.h"
#include <algorithm>

// maps json to drawing and animation tools

namespace Software2552 {
	vector<shared_ptr<Channel>>& ChannelList::getList() {
		return list;
	}
	// since list is maintained 0 is always current
	shared_ptr<Channel> ChannelList::getCurrent() {
		// find first non skipped channel
		for (const auto& channel : list) {
			if (!channel->getSkip()){
				return channel;
			}
		}
		return nullptr;
	}
	shared_ptr<Channel> ChannelList::getbyNumber(int i) {
		if (i < list.size() && i > 0) {
			return list[i];
		}
	}

	// first channel of type 
	shared_ptr<Channel> ChannelList::getbyType(Channel::ChannelType type, int number) {
		// find first non skipped channel
		int count = 0;
		for (const auto& channel : list) {
			if (!channel->getSkip() && channel->getType() == type) {
				if (count == number) {
					return channel;
				}
				++count;
			}
		}
		return nullptr;
	}
	shared_ptr<Channel> ChannelList::getbyName(const string&name) {
		// find first non skipped channel
		for (const auto& channel : list) {
			if (!channel->getSkip() && channel->getKeyName() == name) {
				return channel;
			}
		}
		return nullptr;
	}

	bool ChannelList::readFromScript(const Json::Value &data) {

		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			shared_ptr<Channel> channel = std::make_shared<Channel>();
			channel->readFromScript(data[j]);
			if (!channel->getSkip()) {
				list.push_back(channel);
			}
		}
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
	AnimiatedColor::AnimiatedColor(ColorChoice&colorIn) :ofxAnimatableOfColor() {
		color = colorIn;
	}
	void AnimiatedColor::draw() {
		applyCurrentColor();
	}
	bool AnimiatedColor::readFromScript(const Json::Value &data) {
		// set defaults or read from data
		setColor(Colors::getFirstColors(color.getGroup())->getOfColor(Colors::foreColor));
		animateTo(Colors::getLastColors(color.getGroup())->getOfColor(Colors::foreColor));
		setDuration(0.5f);
		setRepeatType(LOOP_BACK_AND_FORTH);
		setCurve(LINEAR);
		return true;
	}
	bool Ball::myReadFromScript(const Json::Value &data) {
		// can read any of these items from json here
		getDefaultPlayer()->getAnimationHelper()->setPositionY(getPlayer()->floorLine - 100);
		getDefaultPlayer()->getAnimationHelper()->setCurve(EASE_IN);
		getDefaultPlayer()->getAnimationHelper()->setRepeatType(LOOP_BACK_AND_FORTH);
		getDefaultPlayer()->getAnimationHelper()->setDuration(0.55);
		readJsonValue(getPlayer()->radius, data["radius"]);
		ofPoint p;
		p.y = getPlayer()->floorLine;
		getPlayer()->getAnimationHelper()->animateTo(p);
		return true;
	}
	 bool ActorBasics::readFromScript(const Json::Value &data) {
		 if (player == nullptr) {
			 logErrorString("missing player");
			 return false;
		 }

		// any actor can have settings set, or defaults used
		Settings::readFromScript(data["settings"]);

		shared_ptr<AnimiatedColor> ac = std::make_shared<AnimiatedColor>(getColor());
		ac->readFromScript(data["coloranimation"]);
		player->setColorAnimation(ac);

		// any actor can have a reference
		references = parse<Reference>(data["references"]);
		 
		// all actors can have a location
		readStringFromJson(player->getLocationPath(), data["location"]);
		if (player->getLocationPath().size() > 0) {
			getDefaultPlayer()->loadForDrawing();//bugbug if things get too slow etc do not load here
		}

		getDefaultPlayer()->loadForDrawing();//bugbug if things get too slow etc do not load here

		// optional sizes, locations, durations for animation etc
		readJsonValue(player->w, data["width"]);
		readJsonValue(player->h, data["height"]);
		float t=0;
		readJsonValue(t, data["duration"]);
		player->getAnimationHelper()->setObjectLifetime(t);
		float w=0;
		readJsonValue(w, data["wait"]);
		player->getAnimationHelper()->setWait(w);
		Point3D point;
		point.readFromScript(data["startingPoint"]);
		player->getAnimationHelper()->setPosition(point);

		// read derived class data
		myReadFromScript(data);

		return true;
	}


	bool Channel::readFromScript(const Json::Value &data) {
		READSTRING(keyname, data);
		float lifetime=0;
		READFLOAT(lifetime, data);
		setObjectLifetime(lifetime);
		READBOOL(skip, data);
		string type= "any";
		readStringFromJson(type, data["channelType"]);
		if (type == "history") {
			setType(History);
		}
		else if (type == "sports") {
			setType(Sports);
		}
		else  {
			setType(Any);
		}
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
		group = ColorSet::setGroup(colorgroup);
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
	
	bool Text::myReadFromScript(const Json::Value &data) {
		readStringFromJson(getRole<Role>()->getText(), data["text"]["str"]);
		return true;
	}

	// return true if text read in
	bool Paragraph::myReadFromScript(const Json::Value &data) {

		string str;
		readStringFromJson(str, data["text"]["str"]);
		getPlayer().setText(str);

		int indent;
		int leading;
		int spacing;
		string alignment; // paragraph is a data type in this usage

		if (READINT(indent, data)) {
			getPlayer().setIndent(indent);
		}
		if (READINT(leading, data)) {
			getPlayer().setLeading(leading);
		}
		if (READINT(spacing, data)) {
			getPlayer().setSpacing(leading);
		}
		READSTRING(alignment, data);
		if (alignment == "center") { //bugbug ignore case
			getPlayer().setAlignment(ofxParagraph::ALIGN_CENTER);
		}
		else if (alignment == "right") { //bugbug ignore case
			getPlayer().setAlignment(ofxParagraph::ALIGN_RIGHT);
		}

		getPlayer().setFont(getFontPointer());
		getPlayer().setColor(Colors::getFontColor());

		return true;
	}

	void Settings::setSettings(const Settings& rhs) {
		// only copy items that change as a default
		font = rhs.font;
		colors = rhs.colors;
	}

	bool Video::myReadFromScript(const Json::Value &data) {
		float volume=1;//default
		READFLOAT(volume, data);
		getPlayer().setVolume(volume);
		return true;
	}
	bool Audio::myReadFromScript(const Json::Value &data) {
		float volume = 1;//default
		READFLOAT(volume, data);
		getPlayer().setVolume(volume);
		getPlayer().setMultiPlay(true);
		getPlayer().setSpeed(ofRandom(0.8, 1.2));// get from data

		return true;
	}
	void Camera::orbit() {
		if (useOrbit) {
			float time = ofGetElapsedTimef();
			float longitude = 10 * time;
			float latitude = 10 * sin(time*0.8);
			float radius = 600 + 50 * sin(time*0.4);
			player.orbit(longitude, latitude, radius, ofPoint(0, 0, 0));
		}
	}
	
	bool Camera::myReadFromScript(const Json::Value &data) {
		//bugbug fill in
		player.setScale(-1, -1, 1); // showing video
		//getAnimationHelper()->setPositionZ(videoSphere.getPlayer().getRadius() * 2 + 100);
		player.setFov(60);
		return true;
	}
	bool Light::myReadFromScript(const Json::Value &data) {
		//bugbug fill in as an option, use Settings for color, or the defaults
		//get from json player.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		//get from json player.setSpecularColor(ofColor(255.f, 0, 0));
		//could get from json? not sure yet getAnimationHelper()->setPositionX(ofGetWidth()*.2);
		//could get from json? not sure yet getAnimationHelper()->setPositionY(ofGetHeight()*.2);
		return true;
	}
	bool PointLight::myReadFromScript(const Json::Value &data) {
		//bugbug fill in as an option, use Settings for color, or the defaults
		//get from json player.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		//get from json player.setSpecularColor(ofColor(255.f, 0, 0));
		//could get from json? not sure yet getAnimationHelper()->setPositionX(ofGetWidth()*.2);
		//could get from json? not sure yet getAnimationHelper()->setPositionY(ofGetHeight()*.2);
		return true;
	}
	bool DirectionalLight::myReadFromScript(const Json::Value &data) {
		//bugbug fill in as an option, use Settings for color, or the defaults
		//get from json player.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		//get from json player.setSpecularColor(ofColor(255.f, 0, 0));
		//could get from json? not sure yet getAnimationHelper()->setPositionX(ofGetWidth()*.2);
		//could get from json? not sure yet getAnimationHelper()->setPositionY(ofGetHeight()*.2);
		//directionalLight->player.setOrientation(ofVec3f(0, 90, 0));
		return true;
	}
	bool SpotLight::myReadFromScript(const Json::Value &data) {
		//bugbug fill in as an option, use Settings for color, or the defaults
		//get from json player.setDiffuseColor(ofColor(0.f, 255.f, 0.f));
		// specular color, the highlight/shininess color //
		//get from json player.setSpecularColor(ofColor(255.f, 0, 0));
		//could get from json? not sure yet getAnimationHelper()->setPositionX(ofGetWidth()*.2);
		//could get from json? not sure yet getAnimationHelper()->setPositionY(ofGetHeight()*.2);
		//directionalLight->player.setOrientation(ofVec3f(0, 90, 0));
		//spotLight->getPlayer().setSpotlightCutOff(50);
		//		spotLight->player.setSpotConcentration(2);
		return true;
	}
	void Ball::Role::myDraw() {
		ofFill();
		ofCircle((2 * ofGetFrameNum()) % ofGetWidth(), getAnimationHelper()->getCurrentPosition().y, radius);
		//glColor4ub(255, 255, 255, 255);
		ofRect(0, floorLine + radius, ofGetWidth(), 1);

		//vertical lines
		ofRect(xMargin, 0, 1, floorLine + radius);
		ofRect(xMargin + widthCol + radius, 0, 1, floorLine + radius);

	}

	void Text::Role::myDraw() {
		//bugbug add in some animation
		drawText(text, getAnimationHelper()->getCurrentPosition().x, getAnimationHelper()->getCurrentPosition().y);
	}

	void Text::Role::drawText(const string &s, int x, int y) {
		ofPushStyle();
		Colors::setFontColor();
		Font font;
		font.get().drawString(s, x, y);
		ofPopStyle();
	}

	bool Plane::myReadFromScript(const Json::Value &data) {
		//bugbug get this data when ready
		getRole<DrawingPrimitive3d>()->setWireframe(false);
		return true;
	}
	void DrawingPrimitive3d::basedraw() {
		if (useWireframe()) {
			basicdrawWire();
		}
		else {
			basicdraw();
		}
	}
	
	bool Cube::myReadFromScript(const Json::Value &data) {
		float size = 100;//default
		READFLOAT(size, data);
		getRole<DrawingPrimitive3d>()->setWireframe(false);
		getPlayer()->set(size);
		getPlayer()->roll(20.0f);// just as an example
		return true;
	}
	bool Sphere::myReadFromScript(const Json::Value &data) {
		float radius = 100;//default
		READFLOAT(radius, data);
		getPlayer()->setRadius(radius);

		float resolution = 100;//default
		READFLOAT(resolution, data);
		getPlayer()->setResolution(resolution);

		return true;
	}
	void Background::Role::myDraw() {
		//ofBackgroundHex this is an option too bugbug enable background type

		ofBackgroundGradient(ofColor::fromHex(Colors::getForeground()),
			ofColor::fromHex(Colors::getBackground()), mode);
		Text text;
		text.drawText("print", 100, 200);
	}

	void Paragraph::Role::myDraw() {
		player.setPosition(getAnimationHelper()->getCurrentPosition().x, getAnimationHelper()->getCurrentPosition().y);
		player.draw();
	}
	void Background::Role::mySetup() {
		mode = OF_GRADIENT_LINEAR;
		getAnimationHelper()->setRefreshRate(60000);// just set something different while in dev
	}
	// colors and background change over time but not at the same time
	void Background::Role::myUpdate() {
		//bugbug can add other back grounds like a video loop, sound
		// picture, any graphic etc
		if (getAnimationHelper()->refreshAnimation()) {
			switch ((int)ofRandom(0, 3)) {
			case 0:
				mode = OF_GRADIENT_LINEAR;
				break;
			case 1:
				mode = OF_GRADIENT_CIRCULAR;
				break;
			case 2:
				mode = OF_GRADIENT_BAR;
				break;
			}
		}

	}
	bool ChannelList::skipChannel(const string&keyname) {
		for (auto& item : list) {
			if (item->getKeyName() == keyname) {
				return item->getSkip();
			}
		}
		return true;
	}
	// match the keynames 
	bool ChannelList::setStage(shared_ptr<Stage> p) {
		if (p != nullptr) {
			for (auto& item : list) {
				if (item->getKeyName() == p->getKeyName()) {
					item->setStage(p);
					return true;
				}
			}
		}
		return false;
	}
	ActorBasics::~ActorBasics() {
		if (player != nullptr) {
			delete player;
			player = nullptr;
		}
	}
	bool ChannelList::read(const string&path) {
		ofxJSON json;

		if (!json.open(path)) {
			logErrorString("Failed to parse JSON " + path);
			return false;
		}
		try {

			readFromScript(json["channelList"]);
			if (getList().size() == 0) {
				logErrorString("missing channelList");
				return false;
			}

			// read all the scenes
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				string sceneType;
				if (readStringFromJson(sceneType, json["scenes"][i]["sceneType"])) {
					shared_ptr<Stage> p = getScene(sceneType);
					// read common items here
					//p->settings.readFromScript(json["scenes"][i]["settings"]);
					readStringFromJson(p->getKeyName(), json["scenes"][i]["keyname"]);
					if (skipChannel(p->getKeyName())) {
						continue;
					}
					if (p->getKeyName() == "ClydeBellecourt") {
						int i = 1; // just for debugging
					}
					// save with right playitem
					if (p->create(json["scenes"][i])) {
						// find stage and set it
						if (!setStage(p)) {
							logTrace("scene not in playlist (ignored) " + p->getKeyName());
						}
					}
				}
			}
			// remove unattached stages, user forgot them in the input file
			std::vector<shared_ptr<Channel>>::iterator iter = list.begin();
			while (iter != list.end())	{
				if ((*iter)->getStage() == nullptr) {
					iter = list.erase(iter);
					logTrace("item in playlist not found in json (ignored) " + (*iter)->getKeyName());
				}
				else {
					++iter;
				}
			}
		}
		catch (std::exception e) {
			logErrorString(e.what());
			return false;
		}
	}

	// add this one http://clab.concordia.ca/?page_id=944
	void Video::Role::myDraw() {
		player.draw(getAnimationHelper()->getCurrentPosition().x, getAnimationHelper()->getCurrentPosition().y);
	}
	void Video::Role::mySetup() {
		if (!player.isLoaded()) {
			if (!player.load(getLocationPath())) {
				logErrorString("setup video Player");
			}
		}
		player.play();

	}
	bool Video::Role::myObjectLoad() {
		setupForDrawing();
		return true;
	}
	float Video::Role::getTimeBeforeStart(float t) {

		// if json sets a wait use it
		if (getAnimationHelper()->getWait() > 0) {
			setIfGreater(t, getAnimationHelper()->getWait());
		}
		else {
			// will need to load it now to get the true lenght
			if (!player.isLoaded()) {
				player.load(getLocationPath());
			}
			float duration = getAnimationHelper()->getObjectLifetime();
			setIfGreater(t, duration);
		}
		return t;
	}
	void Picture::Role::myDraw() {
		player.draw(getAnimationHelper()->getCurrentPosition().x, getAnimationHelper()->getCurrentPosition().y, w, h);
	}
	void Audio::Role::mySetup() {
		if (!player.load(getLocationPath())) {
			logErrorString("setup audio Player");
		}
		// some of this data could come from data in the future
		player.play();
	}
	int Grabber::find() {
		//bugbug does Kintect show up?
		ofVideoGrabber grabber;
		vector<ofVideoDevice> devices = grabber.listDevices();
		for (vector<ofVideoDevice>::iterator it = devices.begin(); it != devices.end(); ++it) {
			if (it->deviceName == name) {
				return it->id;
			}
		}
	}
	bool Grabber::loadGrabber(int wIn, int hIn) {
		id = find();
		player.setDeviceID(id);
		player.setDesiredFrameRate(30);
		return player.initGrabber(wIn, hIn);
	}

	void Grabber::draw() {
		if (player.isInitialized()) {
			player.draw(x, y);
		}
	}
	bool Grabber::myReadFromScript(const Json::Value &data) {
		//bugbug fill this in
		return true;
	}
	bool TextureVideo::Role::mybind() {
		if (player.isInitialized() && player.isUsingTexture()) {
			player.getTexture().bind();
			return true;
		}
		return false;
	}
	bool TextureVideo::Role::myunbind() {
		if (player.isInitialized() && player.isUsingTexture()) {
			player.getTexture().unbind();
			return true;
		}
		return false;
	}
	bool TextureVideo::myReadFromScript(const Json::Value &data) {
		//bugbug fill this in
		return true;
	}


}