#pragma once
#include "2552software.h"
#include "animation.h"
#include <cstdarg>

namespace Software2552 {
	// matching colors, this class controls colors, no data or others selected colors
	// this way its easier to manage things
	//http://openframeworks.cc/documentation/types/ofColor/#show_fromHsb
	// there is a lot we can do 
	//http://www.color-hex.com/ http://www.colorhexa.com/web-safe-colors
	// http://cloford.com/resources/colours/500col.htm

	// colors can have a time out or a wait count via Animator
	class ColorSet : public objectLifeTimeManager {
	public:
		enum ColorGroup {
			Modern, Smart, Extreme, EarthTone, BuiltIn, Default, Black, White, Blue, RedBlue, Random//only modern so far, ArtDeco, Warm, Cool, Stark, Pastel, LightValue, DarkValue, MediumValue, Random
		};
		//bugbug color set may need 4 or more colors once we do more with graphics
		// something like fore/back/text/other[n], not sure, or maybe we
		// just use multiple ColorSets, find out more as we continue on
		ColorSet(const ColorGroup& groupIn);
		ColorSet(const ColorGroup& groupIn, const ofColor& color1, const ofColor& color2, const ofColor& color3, const ofColor& color4);
		ColorSet(const ColorGroup&groupIn, int color1, int color2, int color3, int color4);
		void setSetcolors(int c, ...);
		void setGroup(const ColorGroup&groupin) {group = groupin;}
		static ColorGroup setGroup(const string&name);
		ColorGroup getGroup() const {return group;}
		int getHex(int index) const  {return colors[index];	}
		bool operator== (const ColorSet& rhs) {	return getGroup() == rhs.getGroup();}
		// return true if less than, and both of the desired type or Random
		bool lessThan(const ColorSet& j, ColorGroup type);
		ColorSet& operator=(const ColorSet& rhs);
		int size() { return colors.size(); }
		ofColor getOfColor(int index) {
			return ofColor().fromHex(getHex(index));
		}
	private:
		ColorGroup group;
		vector<int> colors; //hex values of all matching colors
	};

	// colors animate in that they change with time
	class Colors  {
	public:
		Colors() {
			// there must always be at least one color
			setup();
		}
		//modeled after http://www.creativecolorschemes.com/products/ccs1/rgbColorGuide.shtml
		// just names of customer colors, when paired they are a color set
		enum ColorName {
			// make sure A-O are just names, the other names are hex values
			A, B, C, D, E, F, G, H, I, J, K, L, M, N, O
		};
		// known colors, these are indexs into the color data
		enum ColorUse {
			foreColor=0,
			backColor = 1,
			fontColor = 2,
			lightColor = 3
		};
		// hue helpers, example getHue(getBackground()) bugbug maybe cache some data if needed
		static float getSaturation(int index) {
			return ofColor().fromHex(getCurrentColors()->getHex(index)).getSaturation();
		}
		static float getBrightness(int index) {
			return ofColor().fromHex(getCurrentColors()->getHex(index)).getBrightness();
		}
		static float getHue(int index) {
			return ofColor().fromHex(getCurrentColors()->getHex(index)).getHue();
		}
		static ofColor getOfColor(int index) {
			return ofColor().fromHex(getCurrentColors()->getHex(index));
		}
		static ofFloatColor getFloatColor(int index) {
			return ofFloatColor().fromHex(getCurrentColors()->getHex(index));
		}
		// more like painting
		static float getHueAngle(int index)	{
			return ofColor().fromHex(getCurrentColors()->getHex(index)).getHueAngle();
		}

		static int getForeground() {
			return getCurrentColors()->getHex(foreColor);
		}
		static int getLight() {
			return getFontColor();
		}
		static  int getBackground() {
			return getCurrentColors()->getHex(backColor);
		}
		static  int getFontColor() {
			if (getCurrentColors()->size() < fontColor) {
				return getForeground(); // no need to force a font color
			}
			return getCurrentColors()->getHex(fontColor);
		}
#define COLORNAME_COUNT 15
		void update();
		// set current font color
		static void setFontColor() {
			ofSetHexColor(getFontColor());
		}
		static void setForegroundColor() {
			ofSetHexColor(getForeground());
		}
		static void setBackgroundColor() {
			ofSetBackgroundColorHex(getBackground());
		}
		static shared_ptr<ColorSet> getCurrentColors() { return get(); }
		// call getNext at start up and when ever colors should change
		// do not break colors up or things will not match
		// get next color based on type and usage count
		// example: type==cool gets the next cool type, type=Random gets any next color
		static shared_ptr<ColorSet> getNextColors(ColorSet::ColorGroup type = ColorSet::ColorGroup::Default);
		static shared_ptr<ColorSet> getFirstColors(ColorSet::ColorGroup group);
		static shared_ptr<ColorSet> getLastColors(ColorSet::ColorGroup group);

		class colordata {
		public:
			colordata() {
				currentColor = -1;
				smallest = -1;
			}
			vector<shared_ptr<ColorSet>> colorlist;
			std::map<std::pair <ColorSet::ColorGroup, Colors::ColorName>, int> colorTable; // key,hex pair
			int currentColor;
			int smallest;//index of smallest value
			shared_ptr<ColorSet> defaultColorset=nullptr;
		};

	private:
		template<typename T> void removeExpiredItems(vector<shared_ptr<T>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), objectLifeTimeManager::OKToRemove), v.end());
		}

		static shared_ptr<colordata> privateData;// never call directly to assure allocation
		static shared_ptr<colordata> getprivateData();
		static shared_ptr<ColorSet> getDefaultColors();
		// foreground, background, font
		static shared_ptr<ColorSet> get();
		void setup();
		int find(ColorSet::ColorGroup group, ColorName name);
		void setupBasicColors(ColorSet::ColorGroup type, std::array<int, COLORNAME_COUNT>);
		void Colors::add(ColorSet::ColorGroup group, ColorName fore, ColorName back, ColorName text, ColorName light);
		void Colors::add(ColorSet::ColorGroup group, ColorName fore, ColorName back, const ofColor& text, const ofColor& light);
		void Colors::add(ColorSet::ColorGroup group, const ofColor& fore, const ofColor& back, const ofColor& text, const ofColor& light);
		void Colors::AddColorRow(ColorSet::ColorGroup group, ColorName name, int val);
		static void setSmallest(int i) { getprivateData()->smallest = i; }
		static int  getSmallest() { return getprivateData()->smallest; }
		static void setCurrent(int i) { getprivateData()->currentColor = i; }
		static int  getCurrent() { return getprivateData()->currentColor; }
		static std::map<std::pair <ColorSet::ColorGroup, Colors::ColorName>, int>& getTable() { return getprivateData()->colorTable; }
		static void setColorTableItem(pair <ColorSet::ColorGroup, ColorName> p, int i) { getprivateData()->colorTable[p] = i; }
		static vector<shared_ptr<ColorSet>>& getList() { return getprivateData()->colorlist; }
		static shared_ptr<ColorSet> getListItem(int i);
		// there must always be at least one color
	};


	template <class T> class ColorBase {
	public:
		ColorBase() {}
		ColorBase(const T& colorIn) {
			color = colorIn;
		}
		bool readFromScript(const Json::Value &data) {
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