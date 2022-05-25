#pragma once

#include <string>


namespace RoninEngine
{
	enum class FolderKind
	{
		//graphics
		GFX,
		//SOUND
		SOUND,
		//MUSIC
		MUSIC,
		//LOADER
		LOADER,
		//LEVELS
		LEVELS,
		//TEXTURES
		TEXTURES
	};

	const char* applicationPath();

	const char* prefDataPath();

	const std::string dataPath();

	const std::string dataAt(FolderKind type);

    void release_sdlpaths();
}
