#include "2552software.h"
#include "color.h"
#include <map>

namespace Software2552 {
	vector<ColorSet> Colors::data; // one set of colors to assure matching color themes
	int Colors::smallest = -1;
	std::map<std::pair <ColorSet::ColorGroup, Colors::ColorName>, int> Colors::colorTable;
	ColorSet::ColorGroup Colors::defaultGroup= ColorSet::ColorGroup::Random;

	// get a color set
	ColorSet& Colors::get() {
		// if first time in set things up
		if (getSmallest() < 0) {
			// if there is data
			if (data.size() > 0) {
				getNextColors(); // sets smallest as needed
			}
		}
		// no data or no match found in data
		if (getSmallest() < 0) {
			return ColorSet();// start with defaults
		}
		++data[getSmallest()];
		//int i = getSmallest();// for testing
		//ofColor c = data[getSmallest()].getForeground();
		return data[getSmallest()];
	}
	// get next color based on type and usage count
	// example: type==cool gets the next cool type, type=Random gets any next color
	ColorSet& Colors::getNextColors(ColorSet::ColorGroup type) {
		if (smallest < 0) {
			smallest = 0; // show we started
		}
		// find smallest of type
		//setSmallest(0); // code assume some data loaded
		for (int i = 0; i < data.size(); ++i) {
			if (data[getSmallest()].lessThan(data[i], type)) {
				setSmallest(i);
			}
		}
		return data[getSmallest()];
		//std::vector<ColorSet>::iterator result = std::min_element(std::begin(data), std::end(data));
		//ColorSet cs = *std::min_element(data.begin(), data.end() - 1, ColorSet::searchfn);
		//if (result != data.end()) {
		//return *result;
		//}
	}
	// make a bunch of colors that match using various techniques
	// return hex color
	int Colors::find(ColorSet::ColorGroup group, ColorName name)	{
		pair <ColorSet::ColorGroup, ColorName> p (group, name);
		auto it = colorTable.find(p);
		if (it != colorTable.end()) {
			return it->second;
		}
		return 0; // none found
	}

	void Colors::AddColorRow(ColorSet::ColorGroup group, ColorName name, int val) {
		pair <ColorSet::ColorGroup, ColorName> p(group, name);
		colorTable[p] = val;

		int i = find(group, name);

	}
	void Colors::setupBasicColors(ColorSet::ColorGroup group, std::array<int, COLORNAME_COUNT>a) {
		// fixed list 

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
	void Colors::add(ColorSet::ColorGroup group, ColorName fore, ColorName back, ColorName text) {
		int textColor; // default

		if (text == Black || text == White || text == Blue) {
			textColor = text;
		}
		else  {
			textColor = find(group, text);
		}

		ColorSet s = ColorSet(group,
			find(group, fore),
			find(group, back),
			textColor
			);
		data.push_back(s);
		ColorSet s2 = ColorSet(group,
			find(group, back),
			find(group, fore),
			textColor
			);
		data.push_back(s2);
	}

	//http://www.creativecolorschemes.com/resources/free-color-schemes/art-deco-color-scheme.shtml
	void Colors::setup() {
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
		add(ColorSet::Modern, A, B, Black);
		add(ColorSet::Modern, E, D);
		add(ColorSet::Modern, N, M);
		add(ColorSet::Modern, G, H, Black);
		add(ColorSet::Modern, D, M); 
		add(ColorSet::Modern, D, B, Black);
		add(ColorSet::Modern, J, A);
		add(ColorSet::Modern, M, A);
		add(ColorSet::Modern, H, N, L);
		add(ColorSet::Modern, H, N, Black);
		add(ColorSet::Modern, O, C, Black);
		add(ColorSet::Modern, I, F, Black);
		add(ColorSet::Modern, K, N, Black);

		setupBasicColors(ColorSet::Smart, smart);
		add(ColorSet::Smart, B, E, Black);
		add(ColorSet::Smart, A, G, Black);
		add(ColorSet::Smart, F, M, Black);
		add(ColorSet::Smart, J, N);
		add(ColorSet::Smart, N, D, Black);
		add(ColorSet::Smart, H, K, Black);
		add(ColorSet::Smart, B, L, Black);
		add(ColorSet::Smart, M, A, Black);
		add(ColorSet::Smart, B, E, Blue);
		add(ColorSet::Smart, O, M, Black);
		add(ColorSet::Smart, D, J, Blue);
		add(ColorSet::Smart, E, H, Black);

		setupBasicColors(ColorSet::Extreme, extreme);
		add(ColorSet::Smart, B, K);
		add(ColorSet::Smart, E, M, Black);
		add(ColorSet::Smart, G, D);
		add(ColorSet::Smart, D, O, Black);
		add(ColorSet::Smart, F, I, Black);
		add(ColorSet::Smart, H, K, Black);
		add(ColorSet::Smart, L, C, Black);

		setupBasicColors(ColorSet::EarthTone, earthtone);
		add(ColorSet::Modern, D, B);
		add(ColorSet::Modern, E, A);
		add(ColorSet::Modern, J, I);
		add(ColorSet::Modern, F, N);
		add(ColorSet::Modern, D, H);
		add(ColorSet::Modern, H, J);
		add(ColorSet::Modern, N, J);
		add(ColorSet::Modern, A, H);
		add(ColorSet::Modern, I, K);

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