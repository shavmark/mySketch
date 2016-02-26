#pragma once
#include "2552software.h"
#include "model.h"
#include "view.h"

// join data and view, this class knows about both the data and how its shown

namespace Software2552 {
	class Timeline {
	public:
		enum Type {Setup, Update, Draw};
		Timeline();
		void setup();
		void update();
#if _DEBUG
		void trace() { story.trace(); };
#endif
		void draw();
		void removeExpiredScenes();
	private:
		void enumerateSetup(Scene &scene);
		void enumerateUpdate(Scene &scene);
		void enumerateDraw(Scene &scene);
		void enumerate(Type);
		DrawingTools drawingTools;
		Story story;
		
	};


}