#include "t.h"

namespace Software2552 {

	// helpers


	//bugbug todo weave into errors, even on release mode as anyone can break a json file
	void echoValue(const Json::Value &data, bool isError) {
		//Json::Value::Members m = data.getMemberNames();
		try {
			if (data.isString()) {
				tracer("type=string, value=\""+ data.asString() + "\">", isError);
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
			else  {
				tracer("type=unsupported, type is "+ ofToString(data.type()) + ">", isError);
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
				tracer("<subvalue name=\""+ member +"\">", isError);
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
	template<typename T> void drawVector(T& vec) {
		for (auto& a : vec) {
			a.draw();
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
				default:
					// ignore?
					logTrace("value not found");
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

	// end of helpers

	void Scene::setup() {
		setupVector(audios);
		setupVector(videos);
		setupVector(texts);
		setupVector(images);
		setupVector(graphics); //bugbug create built in graphics like those in the book ("Smoke")
		setupVector(characters);
	}
#if _DEBUG
	// echo object (debug only) bugbug make debug only
	void Story::trace() {
		basicTrace(STRINGIFY(Story));
		traceVector(story);
	}
#endif
	void Scenes::setup() {
		setupVector(scenes);
	}
	void Scenes::update() {
		updateVector(scenes); // give all abjects a change at update
		removeExpiredScenes();
	}
	bool Scenes::dataAvailable() {
		// see if any scenes have any data
		for (auto& scene : scenes) {
			if (scene.dataAvailable()) {
				return true;
			}
		}
		return false;
	}
	void Scenes::removeExpiredScenes() {
		vector<Scene>::iterator it = scenes.begin();
		while (it != scenes.end()) {
			if (!it->dataAvailable()) {
				PlayItem play(it->getKey());
				playlist.plays().erase(std::remove(playlist.plays().begin(), playlist.plays().end(), play), playlist.plays().end());
				it = scenes.erase(it);
			}
			else {
				++it;
			}
		}
	}
	// see if any data in need of drawing is present
	bool Scene::dataAvailable() {
		return audios.size() > 0 ||
			videos.size() > 0 ||
			texts.size() > 0 ||
			images.size() > 0 ||
			graphics.size() > 0 ||
			characters.size() > 0;
	}
	void Scenes::draw() {
		// always play the first key onwards
		for (auto& play : playlist.plays()) {
			Scene lookupscene(play.getKeyName());
			std::vector<Scene>::iterator scene = find(getScenes().begin(), getScenes().end(), lookupscene);
			if (scene != getScenes().end()) {
				scene->draw();
				if (scene->waitOnScene()) {
					break; // only draw one time if blocking, do  not go on to the next one yet
				}
			}
		}
	}
	void Story::setup() {
		read();
		setupVector(story);
	};
	void Story::update() {
		updateVector(story);
	}
	void Story::draw() {
		drawVector(story);
	}
	void Story::read() {
		echo("read a story");

		Scenes scenes(getSettings(), getSharedTools(), "main deck");
		// code in the list of items to make into the story here. 
		scenes.read("json.json");
		story.push_back(scenes);
	}

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
		ECHOAll(data);

		if (Settings::read(data[Settings::JsonName])) {

			parse<Reference>(references, data["references"]);
			return true;
		}
		return false;
	}
	bool Image::read(const Json::Value &data) {
		ECHOAll(data);

		if (Graphic::read(data)) {
			READSTRING(url, data);
			return true;
		}
		return false;
	}
	bool Timeline::read(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		settings.read(data[settings.JsonName]);
		READSTRING(title, data);
		return true;
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
	bool Settings::read(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READSTRING(name, data);
			READSTRING(notes, data);
			READFLOAT(duration, data);
			timelineDate.read(data["timelineDate"]); // date item existed
			lastUpdateDate.read(data["lastUpdateDate"]); // last time object was updated
			date.read(data["date"]);
			startingPoint.read(data["startingPoint"]);
			foregroundColor.read(data["foreground"]);
			backgroundColor.read(data["background"]);
			font.read(data["font"]);
			return true;
		}

		return false;
	}
	
	bool Reference::read(const Json::Value &data) {
		ECHOAll(data);

		if (Settings::read(data[Settings::JsonName])) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READSTRING(url, data[STRINGIFY(Reference)]);
			READSTRING(location, data[STRINGIFY(Reference)]);
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
		string date; // scratch varible

		READINT(bc, data);

		if (READSTRING(date, data)) {
			if (!Poco::DateTimeParser::tryParse(date, datetime, timeZoneDifferential)) {
				logErrorString("invalid date");
				return false;
			}
			datetime.makeUTC(timeZoneDifferential);
		}

		return true;
	}
	bool Graphic::read(const Json::Value &data) {
		ECHOAll(data);

		if (ReferencedItem::read(data)) {
			READSTRING(type, data);
			READFLOAT(duration, data);
			READFLOAT(delay, data);
			return true;
		}
		return false;
	}

	bool Text::read(const Json::Value &data) {
		ECHOAll(data);

		if (Graphic::read(data)) {
			string paragraph; // read in text
			READSTRING(paragraph, data);
			if (paragraph.size() > 0){
				text.setText(paragraph);
				text.setFont(getFont()); // use current font
				text.setColor(foregroundColor);
				int indent=-1;
				READINT(indent, data);
				if (indent > -1) {
					text.setIndent(indent);
				}
				int leading = -1;
				READINT(leading, data);
				if (leading > -1) {
					text.setLeading(leading);
				}
				int spacing = -1;
				READINT(spacing, data);
				if (spacing > -1) {
					text.setSpacing(spacing);
				}
				string alignment;
				READSTRING(alignment, data);
				if (alignment == "left") { //bugbug ignore case
					text.setAlignment(ofxParagraph::ALIGN_LEFT);
				}
				else if (alignment == "center") { //bugbug ignore case
					text.setAlignment(ofxParagraph::ALIGN_CENTER);
				}
				else if (alignment == "right") { //bugbug ignore case
					text.setAlignment(ofxParagraph::ALIGN_RIGHT);
				}
				int width = -1;
				READINT(width, data);
				if (width > -1) {
					text.setWidth(width);
				}
			}
			// add more here like indent as we learn more
			return true;
		}
		return false;
	}
	template<typename T, typename T2> void Scene::createTimeLineItems(T2& vec, const Json::Value &data)
	{
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			T item;
			item.setSettings(getSettings());
			item.read(data[j]);
			vec.push_back(item);
		}
	}
	void Settings::setSettings(const Settings& rhs) {
		font = rhs.font;
		timelineDate = rhs.timelineDate; // date item existed
		lastUpdateDate = rhs.lastUpdateDate; // last time object was updated
		name = rhs.name; // any object can have a name, note, date, reference, duration
		notes = rhs.notes;
		date = rhs.date; // bugbug make this a date data type
		foregroundColor = rhs.foregroundColor;
		backgroundColor = rhs.backgroundColor;
		duration = rhs.duration;
	}

	bool Scene::read(const Json::Value &data) {
		ECHOAll(data);

		if (Timeline::read(data)) {
			READSTRING(keyname, data);
			READBOOL(wait, data);
			createTimeLineItems<Audio>(audios, data["audio"]);
			createTimeLineItems<Video>(videos, data["video"]);
			createTimeLineItems<Text>(texts, data["text"]);
			createTimeLineItems<Image>(images, data["images"]);
			createTimeLineItems<Graphic>(graphics, data["graphics"]);
			return true;
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
	bool Audio::read(const Json::Value &data) {
		ECHOAll(data);

		if (Image::read(data)) {
			READINT(volume, data);
			return true;
		}
		return false;
	
	}
	// read as many jason files as needed, each becomes a deck
	bool Scenes::read(const string& fileName) {
		
		ofxJSON json;

		if (!json.open(fileName)) {
			logErrorString("Failed to parse JSON " + fileName);
			return false;
		}

		// parser uses exepections but openFrameworks does not so exceptions end here
		try {
			Timeline::read(json); // read base class
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