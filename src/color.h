#pragma once
#include "2552software.h"
namespace Software2552 {
	// matching colors, this class controls colors, no data or others selected colors
	// this way its easier to manage things
	//http://openframeworks.cc/documentation/types/ofColor/#show_fromHsb
	// there is a lot we can do 
	//http://www.color-hex.com/ http://www.colorhexa.com/web-safe-colors
	// http://cloford.com/resources/colours/500col.htm
	class ColorSet : public Animator {
	public:
		enum ColorGroup {
			Modern, Smart, Extreme, EarthTone, BuiltIn, Default, Black, White, Blue, Random//only modern so far, ArtDeco, Warm, Cool, Stark, Pastel, LightValue, DarkValue, MediumValue, Random
		};
		//bugbug color set may need 4 or more colors once we do more with graphics
		// something like fore/back/text/other[n], not sure, or maybe we
		// just use multiple ColorSets, find out more as we continue on
		ColorSet() {
			set(Modern, 0xffff, 0x0000, 0xffff);
		}
		ColorSet(const ColorGroup type) {
			set(type, 0x0000, 0x0000, 0xffff);
		}
		ColorSet(const ColorGroup type, int foreground, int background, int text) {
			set(type, foreground, background, text);
		}
		void set(const ColorGroup type, int foreground, int background, int text) {
			state = std::make_tuple(type, 0, foreground, background, text);
		}
		ColorGroup getType() const {
			return get<0>(state);
		}
		int getCount() const {
			return get<1>(state);
		}
		void operator++ () {
			std::get<1>(state) = std::get<1>(state) + 1;
		}
		const int getForeground() const {
			return get<2>(state);
		}
		const int getBackground() const {
			return get<3>(state);
		}
		const int getFontColor() const {
			return get<4>(state);
		}
		bool operator> (const ColorSet& rhs) {
			return getCount() > rhs.getCount();
		}
		bool operator== (const ColorSet& rhs) {
			return getType() == rhs.getType();
		}
		// return true if less than, and both of the desired type or Random
		bool lessThan(const ColorSet& j, ColorGroup type) {
			if (isExpired() || (type != Random && getType() != j.getType())) {
				return false;
			}
			return *this > j;
		}
		ColorSet& operator=(const ColorSet& rhs) {
			state = rhs.state;
			return *this;
		}

	private:
		// never want to seperate or we will not match
		// where does value come in? maybe convert from color to hue, sat/val?
		tuple<ColorGroup, int, int, int, int> state;
	};
	// colors animate in that they change with time
	class Colors : public Animator {
	public:
		Colors() : Animator() {
			// there must always be at least one color
			setup();
		}
		//modeled after http://www.creativecolorschemes.com/products/ccs1/rgbColorGuide.shtml
		// A-O are just names of customer colors, other values double as hex values
		enum ColorName {
			A= 0xfffffff, B, C, D, E, F, G, H, I, J, K, L, M, N, O, White= 0xffff, Black = 0x0, Blue = 0x0000FF
		};
#define COLORNAME_COUNT 15
		void update();
		// get the default color for example, the find could return more than one but this function
		// is used to get the one offs like Default and Black etc
		static ColorSet getFirstColor(ColorSet::ColorGroup group = ColorSet::Default) {
			std::vector<ColorSet>::iterator itr = std::find(data.begin(), data.end(), ColorSet(group));
			if (itr != data.end()) {
				++(*itr); // reflect usage
				return *itr;
			}
			return ColorSet(); // always do something
		}
		// call getNext at start up and when ever colors should change
		// do not break colors up or thins will not match
		// get next color based on type and usage count
		// example: type==cool gets the next cool type, type=Random gets any next color
		static ColorSet& getNextColors(ColorSet::ColorGroup type = defaultGroup);
		// there must always be at least one color
		static ColorSet& getCurrentColors() { return get(); }
		static void setSmallest(int i) { smallest = i; }
		static int  getSmallest() { return smallest; }
		static ColorSet::ColorGroup defaultGroup;
	private:
		// foreground, background, font
		static ColorSet& get();
		static std::map<std::pair <ColorSet::ColorGroup, ColorName>, int> colorTable; // key,hex pair
		static vector<ColorSet> data;
		static int smallest;//index of smallest value
		void setup();
		int find(ColorSet::ColorGroup group, ColorName name);
		void setupBasicColors(ColorSet::ColorGroup type, std::array<int, COLORNAME_COUNT>);
		void Colors::add(ColorSet::ColorGroup group, ColorName fore, ColorName back, ColorName text= White);
		void Colors::AddColorRow(ColorSet::ColorGroup group, ColorName name, int val);
	};


	template <class T> class ColorBase {
	public:
		ColorBase() {}
		ColorBase(const T& colorIn) {
			color = colorIn;
		}
		bool read(const Json::Value &data) {
			readJsonValue(color.r, data["r"]);
			readJsonValue(color.g, data["g"]);
			readJsonValue(color.g, data["b"]);
			return true;
		}
		operator T() {
			return color;
		}
		T& get() { return color; }
	protected:
		T color;
	};

	// color support of explict color is needed
	class Color : public ColorBase<ofColor> {
	public:
		Color() :ColorBase() {
		}
		Color(const ofColor& color) :ColorBase(color) {
		}
	};
	class floatColor : public ColorBase<ofFloatColor> {
	public:
		floatColor() :ColorBase() {
		}
		floatColor(const ofFloatColor& color) :ColorBase(color) {
		}

	};


}