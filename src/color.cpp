#include "2552software.h"
#include "model.h"
#include "color.h"
#include <map>

namespace Software2552 {
	shared_ptr<Colors::colordata> Colors::privateData=nullptr; // declare static data
	
	ColorSet::ColorSet(const ColorGroup& groupIn) :objectLifeTimeManager() {
		setGroup(groupIn);
		// always set a color to avoid bad errors
		setSetcolors(4, ofColor(ofColor::blue).getHex(), ofColor(ofColor::orangeRed).getHex(), ofColor(ofColor::azure).getHex(), ofColor(ofColor::green).getHex());
	}
	ColorSet::ColorSet(const ColorGroup& groupIn, const ofColor& color1, const ofColor& color2, const ofColor& color3, const ofColor& color4) : objectLifeTimeManager() {
		// always store as hex
		setGroup(groupIn);
		setSetcolors(4, color1.getHex(), color2.getHex(), color3.getHex(), color4.getHex());
	}
	ColorSet::ColorSet(const ColorGroup&groupIn, int color1, int color2, int color3, int color4) : objectLifeTimeManager() {
		setGroup(groupIn);
		setSetcolors(4, color1, color2, color3, color4);
	}
	void ColorSet::setSetcolors(int c, ...) {
		colors.clear();

		va_list args;
		va_start(args, c);
		for (int i = 0; i < c; ++i) {
			colors.push_back(va_arg(args, int));
		}
		va_end(args);
	}

	shared_ptr<ColorSet> Colors::getDefaultColors() {
		if (getprivateData()->defaultColorset == nullptr) {
			getprivateData()->defaultColorset = std::make_shared<ColorSet>(ColorSet::Default);
		}
		++(*getprivateData()->defaultColorset);
		return getprivateData()->defaultColorset;
	}

	// get a color set if the current one is not set or if its expired
	shared_ptr<ColorSet> Colors::get() {
		if (getListItem(getCurrent()) == nullptr) {
			return getDefaultColors();
		}
		// if first time in set things up
		if (getSmallest() < 0 || getListItem(getCurrent())->refreshAnimation()) {
			getNextColors(); 
		}
		// no data or no match found in data
		if (getSmallest() < 0) {
			return getDefaultColors();
		}
		if (getListItem(getCurrent()) != nullptr) {
			++(*getListItem(getCurrent()));
		}
		return getListItem(getCurrent());
	}
	shared_ptr<ColorSet> Colors::getFirstColors(ColorSet::ColorGroup group) {
		for (const auto& a : getList()) {
			if (a->getGroup() == group) {
				return a;
			}
		}
		return getDefaultColors();
	}
	shared_ptr<ColorSet> Colors::getLastColors(ColorSet::ColorGroup group) {
		std::vector<shared_ptr<ColorSet>>::reverse_iterator rit = getList().rbegin();
		for (; rit != getList().rend(); ++rit) {
			if ((*rit)->getGroup() == group) {
				return *rit;
			}
		}
		return getDefaultColors();
	}

	// get next color based on type and usage count
	// example: type==cool gets the next cool type, type=Random gets any next color
	shared_ptr<ColorSet> Colors::getNextColors(ColorSet::ColorGroup group) {
		if (getSmallest() < 0) {
			// first call 
			setSmallest(0);
			setCurrent(0);
		}
		// find smallest of type
		//setSmallest(0); // code assume some data loaded
		for (int i = 0; i < getList().size(); ++i) {
			if (getListItem(getSmallest())->lessThan(*getListItem(i), group)) {
				setSmallest(i);
				setCurrent(i);
			}
		}
		return getListItem(getCurrent());
	}
	shared_ptr<ColorSet> Colors::getListItem(int i) { 
		if (i < 0 || i >= getprivateData()->colorlist.size())	{
			return nullptr;
		}
		return getprivateData()->colorlist[i]; 
	}

	// make a bunch of colors that match using various techniques
	// return hex color
	int Colors::find(ColorSet::ColorGroup group, ColorName name)	{
		pair <ColorSet::ColorGroup, ColorName> p (group, name);
		auto it = getTable().find(p);
		if (it != getTable().end()) {
			return it->second;
		}
		return 0; // none found
	}
	shared_ptr<Colors::colordata> Colors::getprivateData() {
		if (privateData == nullptr) {
			privateData = std::make_shared<colordata>();
		}
		return privateData;
	}

	void Colors::AddColorRow(ColorSet::ColorGroup group, ColorName name, int val) {
		setColorTableItem(pair<ColorSet::ColorGroup, ColorName>(group, name), val);
		int i = find(group, name); // for testing

	}
	// use the build the sets 
	void Colors::setupBasicColors(ColorSet::ColorGroup group, std::array<int, COLORNAME_COUNT>a) {
		// fixed list 
		//bugbug allow any number of colors per group, these 14 are special sets from the book
		AddColorRow(group, A, a[0]);
		AddColorRow(group, B, a[1]);
		AddColorRow(group, C, a[2]);
		AddColorRow(group, D, a[3]);
		AddColorRow(group, E, a[4]);
		AddColorRow(group, F, a[5]);
		AddColorRow(group, G, a[6]);
		AddColorRow(group, H, a[7]);
		AddColorRow(group, I, a[8]);
		AddColorRow(group, J, a[9]);
		AddColorRow(group, K, a[10]);
		AddColorRow(group, L, a[11]);
		AddColorRow(group, M, a[12]);
		AddColorRow(group, N, a[13]);
		AddColorRow(group, O, a[14]);
	}
	void Colors::add(ColorSet::ColorGroup group, ColorName fore, ColorName back, ColorName text, ColorName light) {

		shared_ptr<ColorSet> s = std::make_shared<ColorSet>(group,
			find(group, fore),
			find(group, back),
			find(group, text),
			find(group, light)
			);
		getList().push_back(s);
	}
	void Colors::add(ColorSet::ColorGroup group, ColorName fore, ColorName back, const ofColor& text, const ofColor& light) {

		int h = light.getHex();
		shared_ptr<ColorSet> s = std::make_shared<ColorSet>(group,
			find(group, fore),
			find(group, back),
			text.getHex(),
			light.getHex()
			);

		getList().push_back(s);
	}
	void Colors::add(ColorSet::ColorGroup group, const ofColor& fore, const ofColor& back, const ofColor& text, const ofColor& light) {

		shared_ptr<ColorSet> s = std::make_shared<ColorSet>(group,
			fore.getHex(),
			back.getHex(),
			text.getHex(),
			light.getHex()
			);

		getList().push_back(s);
	}
	ColorSet::ColorGroup ColorSet::setGroup(const string&name) {
		if (name == "Modern") {
			return Modern;
		}
		else if (name == "Smart") {
			return Smart;
		}
		else if (name == "Extreme") {
			return Extreme;
		}
		else if (name == "EarthTone") {
			return EarthTone;
		}
		else if (name == "Black") {
			return Black;
		}
		else if (name == "White") {
			return White;
		}
		else if (name == "Blue") {
			return Blue;
		}
		else if (name == "Random") {
			return Random;
		}
		else {
			return Default;
		}
	}

	bool ColorSet::lessThan(const ColorSet& j, ColorGroup group) {
		if (isExpired() || (group != Random && getGroup() != j.getGroup())) {
			return false;
		}
		return *this > j;
	}
	ColorSet& ColorSet::operator=(const ColorSet& rhs) {
		*this = rhs;
		return *this;
	}

	//http://www.creativecolorschemes.com/resources/free-color-schemes/art-deco-color-scheme.shtml
	void Colors::setup() {

		// only needs to be setup one time since its static data
		if (getList().size() == 0) {
			std::array<int, COLORNAME_COUNT> modern =
			{ 0x003F53, 0x3994B3, 0x64B1D1, 0x00626D, 0x079CBA, 0x60CDD9, 0x003E6B,
				0x0073A0, 0xBAECE9, 0xD0FC0E, 0xFDB075, 0xFFD76E, 0x4D5E6C, 0x858E9C, 0xCCD1D5 };
			std::array<int, COLORNAME_COUNT> smart =
			{ 0x312659, 0x373B81, 0x425096, 0x0D60A5, 0x297BC6, 0x3EA0ED, 0x169FAD,
				0x30C1D8, 0x7FE3F7, 0xB01116, 0xD71920, 0xFEAB07, 0xFED341, 0xFFDA7A, 0xFFEEBC };
			std::array<int, COLORNAME_COUNT> extreme =
			{ 0x023D7B, 0x1B4C8C, 0x448BE3, 0x025B8F, 0x088BB3, 0x02CAE6, 0xC61630,
				0xFE243E, 0xFE3951, 0xF03624, 0xF3541B, 0xFE872B, 0x8FD173, 0xB7B96B, 0xAD985F };
			std::array<int, COLORNAME_COUNT> earthtone =
			{ 0x493829, 0x816C5B, 0xA9A18C, 0x613318, 0x855723, 0xB99C6B, 0x8F3B1B,
				0xD57500, 0xDBCA69, 0x404F24, 0x668D3C, 0xBDD09F, 0x4E6172, 0x83929F, 0xA3ADB8 };

			setupBasicColors(ColorSet::Modern, modern);
			add(ColorSet::Modern, A, B, ofColor::black, ofColor::white);
			add(ColorSet::Modern, E, D, ofColor::black, ofColor::white);
			add(ColorSet::Modern, N, M, ofColor::white, ofColor::white);
			add(ColorSet::Modern, G, H, ofColor::black, ofColor::white);
			add(ColorSet::Modern, D, M, ofColor::white, ofColor::white);
			add(ColorSet::Modern, D, B, ofColor::black, ofColor::white);
			add(ColorSet::Modern, J, A, ofColor::white, ofColor::white);
			add(ColorSet::Modern, M, A, ofColor::white, ofColor::white);
			add(ColorSet::Modern, H, N, L, ofColor::white);
			add(ColorSet::Modern, H, N, ofColor::black, ofColor::white);
			add(ColorSet::Modern, O, C, ofColor::black, ofColor::white);
			add(ColorSet::Modern, I, F, ofColor::black, ofColor::white);
			add(ColorSet::Modern, K, N, ofColor::black, ofColor::white);

			setupBasicColors(ColorSet::Smart, smart);
			add(ColorSet::Smart, B, E, ofColor::black, ofColor::white);
			add(ColorSet::Smart, A, G, ofColor::black, ofColor::white);
			add(ColorSet::Smart, F, M, ofColor::black, ofColor::white);
			add(ColorSet::Smart, J, N, ofColor::black, ofColor::white);
			add(ColorSet::Smart, N, D, ofColor::black, ofColor::white);
			add(ColorSet::Smart, H, K, ofColor::black, ofColor::white);
			add(ColorSet::Smart, B, L, ofColor::black, ofColor::white);
			add(ColorSet::Smart, M, A, ofColor::black, ofColor::white);
			add(ColorSet::Smart, B, E, ofColor::blue, ofColor::white);
			add(ColorSet::Smart, O, M, ofColor::black, ofColor::white);
			add(ColorSet::Smart, D, J, ofColor::blue, ofColor::white);
			add(ColorSet::Smart, E, H, ofColor::black, ofColor::white);

			setupBasicColors(ColorSet::Extreme, extreme);
			add(ColorSet::Extreme, B, K, ofColor::white, ofColor::white);
			add(ColorSet::Extreme, E, M, ofColor::black, ofColor::white);
			add(ColorSet::Extreme, G, D, ofColor::black, ofColor::white);
			add(ColorSet::Extreme, D, O, ofColor::black, ofColor::white);
			add(ColorSet::Extreme, F, I, ofColor::black, ofColor::white);
			add(ColorSet::Extreme, H, K, ofColor::black, ofColor::white);
			add(ColorSet::Extreme, L, C, ofColor::black, ofColor::white);

			setupBasicColors(ColorSet::EarthTone, earthtone);
			add(ColorSet::EarthTone, D, B, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, E, A, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, J, I, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, F, N, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, D, H, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, H, J, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, N, J, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, A, H, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, I, K, ofColor::black, ofColor::white);
			add(ColorSet::EarthTone, ofColor::green, C, ofColor::black, ofColor::white); // just a demo of using the built in colors

			// setup the built in colors
			add(ColorSet::Blue, ofColor::lightBlue, ofColor::white, ofColor::blue, ofColor::white);
			add(ColorSet::Black, ofColor::black, ofColor::white, ofColor::blue, ofColor::white); 
			add(ColorSet::White, ofColor::white, ofColor::black, ofColor::blue, ofColor::white); 
			add(ColorSet::RedBlue, ofColor::red, ofColor::lightCoral, ofColor::blue, ofColor::indianRed);
			add(ColorSet::Default, ofColor::red, ofColor::blue, ofColor::white, ofColor::green);

		}

#if 0
			//http://www.creativecolorschemes.com/resources/free-color-schemes/art-deco-color-scheme.shtml
//bugbug at some point maybe read from json
			ColorSet cs = ColorSet(ColorSet::ArtDeco,
				ofColor(255, 0, 0), ofColor(0, 255, 0), ofColor(0, 0, 255));
			data.push_back(cs);

			ofColor fore, back, text;
			fore.setHsb(200, 100, 40); // just made this up for now
			back.setHsb(100, 100, 50);
			text.setHsb(200, 100, 100);
			cs.set(ColorSet::ArtDeco, fore, back, text);
			data.push_back(cs);
			//			ColorSet cs2 = ColorSet(ColorSet::Warm,
			//			ofColor::aliceBlue, ofColor::crimson, ofColor::antiqueWhite);
			ColorSet cs2 = ColorSet(ColorSet::ArtDeco,
				ofColor(0, 255, 0), ofColor(0, 0, 255), ofColor(255, 255, 255));
			data.push_back(cs2);
	}

#endif // 0
	}


}