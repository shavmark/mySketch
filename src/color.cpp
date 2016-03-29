#include "2552software.h"
#include "animation.h"
#include "model.h"
#include "color.h"
#include <map>
#include <unordered_map>

namespace Software2552 {
	shared_ptr<Colors::colordata> Colors::privateData=nullptr; // declare static data

	ColorSet::ColorSet(const ColorGroup groupIn, int fore, int back, int text, int other, int lightest, int darkest) : objectLifeTimeManager() {
		group = groupIn;
		setSetcolors(6, fore, back, text, other, lightest, darkest);
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

	// get a color set if the current one is not set or if its expired
	shared_ptr<ColorSet> ColorList::get() {
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

	void ColorList::update() {
		// clean up deleted items every so often
		for (auto& d : getList()) {
			d->refreshAnimation();
		}
		// remove expired colors
		removeExpiredItems(getList());
	}

	shared_ptr<ColorSet> ColorList::getFirstColors(ColorSet::ColorGroup group) {
		for (const auto& a : getList()) {
			if (a->getGroup() == group) {
				return a;
			}
		}
		return getDefaultColors();
	}

	// get next color based on type and usage count
	// example: type==cool gets the next cool type, type=Random gets any next color
	shared_ptr<ColorSet> ColorList::getNextColors(ColorSet::ColorGroup group) {
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
	shared_ptr<ColorSet> ColorList::getListItem(int i) {
		if (i < 0 || i >= getprivateData()->colorlist.size())	{
			return nullptr;
		}
		return getprivateData()->colorlist[i]; 
	}

	shared_ptr<Colors::colordata> ColorList::getprivateData() {
		if (privateData == nullptr) {
			privateData = std::make_shared<colordata>();
		}
		return privateData;
	}
	void ColorList::add(const ColorSet::ColorGroup group, int fore, int back, int text, int other, int lightest, int darkest) { 
		// colors stored as hex
		shared_ptr<ColorSet> s = std::make_shared<ColorSet>(group,
			fore,
			back,
			text,
			other,
			lightest,
			darkest
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


	//http://www.creativecolorschemes.com/resources/free-color-schemes/art-deco-color-scheme.shtml
	void ColorList::setup() {

		// only needs to be setup one time since its static data
		if (getList().empty()) {
			std::unordered_map<char, int> modern =
			{ {'A', 0x003F53}, {'B', 0x3994B3}, {'C', 0x64B1D1 }, {'D', 0x00626D }, {'E', 0x079CBA }, {'F', 0x60CDD9 },
			 {'G', 0x003E6B }, {'H', 0x0073A0 }, {'I', 0xBAECE9 }, {'J', 0xD0FC0E }, {'K', 0xFDB075 }, {'L', 0xFFD76E },
			 {'M', 0x4D5E6C }, {'N', 0x858E9C }, {'O', 0xCCD1D5 }};
			std::unordered_map<char, int> smart = {
			{ 'A',0x312659},{'B', 0x373B81},{ 'C', 0x425096},{ 'D', 0x0D60A5 },{ 'E', 0x297BC6 },{ 'F', 0x3EA0ED },{ 'G', 0x169FAD },
			{ 'H', 0x30C1D8 },{ 'I', 0x7FE3F7 },{ 'J', 0xB01116 },{ 'K', 0xD71920 },{ 'L', 0xFEAB07 },{ 'M', 0xFED341 },{ 'N', 0xFFDA7A },{ 'O', 0xFFEEBC } };

			std::unordered_map<char, int> extreme = {
				{ 'A',0x023D7B },{ 'B', 0x1B4C8C },{ 'C', 0x448BE3 },{ 'D', 0x025B8F },{ 'E', 0x088BB3 },
				{ 'F',  0x02CAE6 },{ 'G',  0xC61630 },{ 'H',0xFE243E },{ 'I',  0xFE3951 },{ 'J',  0xF03624 },
				{ 'K',  0xF3541B },{ 'L',  0xFE872B },{ 'M',  0x8FD173 },{ 'N',  0xB7B96B },{ 'O',  0xAD985F } };

			std::unordered_map<char, int>  earthtone ={
			{ 'A',0x493829 },{ 'B',  0x816C5B },{ 'C',  0xA9A18C },{ 'D',  0x613318 },{ 'E',  0x855723 },{ 'F',  0xB99C6B },{ 'G',  0x8F3B1B },{ 'H', 0xD57500 },
			{ 'I',  0xDBCA69 },{ 'J',  0x404F24 },{ 'K',  0x668D3C },{ 'L',  0xBDD09F },{ 'M',  0x4E6172 },{ 'N',  0x83929F },{ 'O',  0xA3ADB8} };

			//A C B D A C
			add(ColorSet::Modern, modern['A'], modern['C'], modern['B'], modern['D'], modern['A'], modern['C']);
			/*
			add(ColorSet::Modern, E, D, ofColor::black.getHex(), ofColor::white.getHex());

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
			*/
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