#pragma once

#include "dependency.h"

namespace RoninEngine::Runtime
{
	const char* applicationPath();

	const char* prefDataPath();

	const std::string dataPath();

	const std::string getDataFrom(FolderKind type);

    void release_sdlpaths();
}
