#pragma once

#include "2552software.h"
#include "view.h"

namespace Software2552 {

	template <class T>
	class Wrapper : public  T {
	public:
		// wrap an Openframeworks or other such object so we can easily delete, track, test, debug  etc
		Wrapper(uint64_t id) { myID = id; }
		bool operator==(const Wrapper &rhs) {
			return myID == rhs.myID;
		}
		uint64_t id() { return myID; }
	private:
		uint64_t myID;
	};
	//template <class T>
	//class Wrapper : public  T, public Wrap {
	//public:
		//Wrapper(uint64_t id) :Wrap(id), T() {}
	//};

	// drawing tools etc, shared across objects
	class Tools {
	public:
		Tools() {
		}
		template<typename T> void removeVector(vector<Wrapper<T>>& vec, uint64_t myID) {
			vector<Wrapper<T>>::iterator it = vec.begin();
			while (it != vec.end()) {
				if (!it->id() == myID) {
					it = vec.erase(it);
				}
				else {
					++it;
				}
			}
		}

		ofLight	 light;
		ofCamera camera;
		vector<Wrapper<ofVideoPlayer>> videoPlayers;
	};

	// drawing related items start here
	class BaseClass2552WithDrawing : public BaseClass {
	public:
		BaseClass2552WithDrawing() { valid = false; }

		bool objectValid() { return valid; } // data is in a good state
		void setValid(bool b = true) { valid = b; };

	private:
		bool valid; // true when data is valid and ready to draw
	};
	class Graphics2552  {
	public:
		static void rotateToNormal(ofVec3f normal);

	};

}
