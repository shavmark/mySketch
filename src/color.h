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
		ColorSet(const ColorGroup groupIn, int fore, int back, int text, int other, int lightest, int darkest);
		ColorGroup setGroup(const string&name);
		ColorGroup getGroup() const {return group;}
		int getHex(int index) const  {return colors[index];	}
		bool operator== (const ColorSet& rhs) {	return getGroup() == rhs.getGroup();}
		// return true if less than, and both of the desired type or Random
		bool lessThan(const ColorSet& j, ColorGroup type);
		ColorSet& operator=(const ColorSet& rhs);
		int size() { return colors.size(); }
	private:
		void setSetcolors(int c, ...);
		ColorGroup group;
		vector<int> colors; //hex values of all matching colors
	};

	// global color data
	class ColorList {
	public:
		class ColorList() {
			// there must always be at least one color
			setup();
		}
		//color naming modeled after http://www.creativecolorschemes.com/products/ccs1/rgbColorGuide.shtml
		// names of customer colors, when paired they are a color set
		enum ColorName {
			// make sure A-O are just names, the other names are hex values
			A, B, C, D, E, F, G, H, I, J, K, L, M, N, O
		};
		// known colors, these are indexs into the color data
		enum ColorUse {foreColor,backColor,	fontColor, lightestColor, darkestColor
		};

		void update();

		class colordata {
		public:
			colordata() {
				currentColor = -1;
				smallest = -1;
			}
			vector<shared_ptr<ColorSet>> colorlist;
			int currentColor;
			int smallest;//index of smallest value
			int lightest = 0;
			int darkest = 255;
			shared_ptr<ColorSet> defaultColorset = nullptr;
		};


		static shared_ptr<ColorSet> getCurrentColors() { return get(); }
		// call getNext at start up and when ever colors should change
		// do not break colors up or things will not match
		// get next color based on type and usage count
		// example: type==cool gets the next cool type, type=Random gets any next color
		static shared_ptr<ColorSet> getNextColors(ColorSet::ColorGroup type = ColorSet::ColorGroup::Default);
		static shared_ptr<ColorSet> getFirstColors(ColorSet::ColorGroup group);

		static int LightestColor() { return getprivateData()->lightest; }
		static int DarkestColor() { return getprivateData()->darkest; }
		static void setLightest(int c) { getprivateData()->lightest = c; }
		static void setDarkest(int c) { getprivateData()->darkest = c; }

	protected:
		template<typename T> void removeExpiredItems(vector<shared_ptr<T>>&v) {
			v.erase(std::remove_if(v.begin(), v.end(), objectLifeTimeManager::OKToRemove), v.end());
		}

		static shared_ptr<colordata> privateData;// never call directly to assure allocation
		static shared_ptr<colordata> getprivateData();
		static shared_ptr<ColorSet> getDefaultColors() { return nullptr; };
		// foreground, background, font
		static shared_ptr<ColorSet> get();
		void setup();

		void add(const ColorSet::ColorGroup group, int fore, int back, int text, int other, int lightest, int darkest);

		static void setSmallest(int i) { getprivateData()->smallest = i; }
		static int  getSmallest() { return getprivateData()->smallest; }
		static void setCurrent(int i) { getprivateData()->currentColor = i; }
		static int  getCurrent() { return getprivateData()->currentColor; }
		static vector<shared_ptr<ColorSet>>& getList() { return getprivateData()->colorlist; }
		static shared_ptr<ColorSet> getListItem(int i);
		// there must always be at least one color
	};


	// color helpers
	class Colors : public ColorList {
	public:
		Colors() : ColorList(){		}
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
		static int getLightest() {
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