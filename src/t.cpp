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

	template<typename T, typename T2> void parse(T2& vec, const Json::Value &data)
	{
		for (Json::ArrayIndex j = 0; j < data.size(); ++j) {
			T item;
			item.read(data[j]);
			vec.push_back(item);
		}
	}
	void setString(string &value, const Json::Value& data) {
		if (set(value, data)) {
			value = data.asString();
		}
	}
	template<typename T> bool set(T &value, const Json::Value& data) {
		try {
			if (!data.empty()) {
				switch (data.type()) {
				case Json::booleanValue:
					value = data.asBool();
					break;
				case Json::stringValue:
					return true; // templates get confused to set the string else where, use this to validate
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
	void Scenes::setup() {
		setupVector(scenes);
	}
	void Scenes::update() {
		updateVector(scenes);
	}
	void Scenes::draw() {
		for (auto& scene : scenes) {
			scene.draw();
			if (scene.wait()) {
				break; // only draw one time if blocking, do  not go on to the next one yet
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

		if (Settings::read(data)) {

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
		settings.read(data);
		READSTRING(title, data);
		return true;
	}

	bool Settings::read(const Json::Value &data) {
		// dumps too much so only enable if there is a bug: ECHOAll(data);
		if (!data.empty()) { // ignore reference as an array or w/o data at this point
			READDATE(timelineDate, data);
			READDATE(date, data);
			READDATE(lastUpdateDate, data);
			READSTRING(name, data);
			READSTRING(notes, data);
			READFLOAT(duration, data);

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

			setString(name, data["font"]["name"]);
			setString(name, data["font"]["file"]);
			set(size, data["font"]["size"]);

			if (filename.size() != 0) {
				if (!fontExists(filename, size)) {
					// name is not unqiue, just a helper of some kind I guess
					font = ofxSmartFont::add(filename, size, name);
				}
			}
			return true;
		}

		return false;
	}
	
	bool Reference::read(const Json::Value &data) {
		ECHOAll(data);

		if (Settings::read(data)) { // ignore reference as an array or w/o data at this point
			// no base class so it repeats some data in base class ReferencedItem
			READSTRING(url, data);
			READSTRING(location, data);
			READSTRING(source, data);
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

	bool Graphic::read(const Json::Value &data) {
		ECHOAll(data);

		if (ReferencedItem::read(data)) {
			READSTRING(type, data);
			READFLOAT(duration, data);
			READFLOAT(delay, data);
			READINT(x, data);
			READINT(y, data);
			READINT(z, data);

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
				text.setFont(font); // use current font
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
				// read in base class, read in as percent  (Assumed updated in update())
				if (x > -1) {
					text.x = x; 
				}
				if (y > -1) {
					text.x = y;
				}
			}
			// add more here like indent as we learn more
			return true;
		}
		return false;
	}
	bool Scene::read(const Json::Value &data) {
		ECHOAll(data);

		if (Timeline::read(data)) {
			READSTRING(keyname, data);
			READBOOL(skip, data);
			READBOOL(block, data);
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
			Timeline::read(json);

			// build order and list of slide scenes
			for (Json::ArrayIndex i = 0; i < json["playList"].size(); ++i) {
				// scenes stored in order they should be run
				string keyname;
				setString(keyname, json["playList"][i]["keyname"]);
				Scene scene(getSettings(), getSharedTools(), keyname);
				add(scene);
			}
			for (Json::ArrayIndex i = 0; i < json["scenes"].size(); ++i) {
				logTrace("create look upjson[scenes][" + ofToString(i) + "][keyname]");
				Scene lookupscene(json["scenes"][i]["keyname"].asString());
				// read into matching slide
				std::vector<Scene>::iterator it = find(getScenes().begin(), getScenes().end(), lookupscene);
				if (it != getScenes().end()) {
					// Settings can appear here too
					logTrace("parse settings json[scenes][" + ofToString(i) + "][keyname]");
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