// store notes here
//https://github.com/google/liquidfun

// follow this https://github.com/openframeworks/openFrameworks/wiki/oF-code-style, but let them w/o bugs cast the first flames

//http://openframeworks.cc/ofBook/chapters/c++11.html\

//http://openframeworks.cc/tutorials/graphics/generativemesh.html

//https://github.com/Kinect/Docs

/* iclone import notes
	//model.setup("C:\\Users\\mark\\Documents\\iclone\\heidi\\heidi.fbx");
	//model.setup("..\\..\\heidi.fbx"); // should be in bin\data but need to figure how to that in git bugbug 
	From2552Software::TheModel model4;
	model4.setup("C:\\Users\\mark\\Documents\\iclone\\test1\\t8_motion0.bvh"); // should be in bin\data but need to figure how to that in git bugbug 
	model.setAnimations(model4);
	model.setup("C:\\Users\\mark\\Documents\\iclone\\test1\\t7.obj"); // should be in bin\data but need to figure how to that in git bugbug 
									  //model2.setup("C:\\Users\\mark\\Documents\\iclone\\reggie\\reggie_Catwalk.fbx");

									  //AssbinImporter import;
	Assimp::Exporter exporter;
	size_t size = exporter.GetExportFormatCount();
	for (int i = 0; ; ++i) {
		const aiExportFormatDesc*pd = exporter.GetExportFormatDescription(i);
		if (pd == nullptr) {
			break;
		}
	}
	
	//https://commons.wikimedia.org/wiki/File:Vincent_van_Gogh_-_Self-Portrait_-_Google_Art_Project.jpg
	//aiReturn r = exporter.Export(model.getAssimpScene(), "collada", "righthererightnow");
	string er1 = exporter.GetErrorString();
	Assimp::Importer importer;
	const aiScene *s = importer.ReadFile("righthererightnow", 0); // mem owned by importer
	//aiCopyScene(s, &model.getAssimpScene());
	string er = importer.GetErrorString();

	//model.setPlay(aiString("AnimStack::motion0"));
	model.setPlay(aiString("Motion"));

	//model2.setPlay(aiString("AnimStack::Catwalk"));

*/