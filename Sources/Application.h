#ifndef _GAME_H
#define _GAME_H

#include "ResourceManager.h"
#include "framework.h"

namespace RoninEngine {
class Application {
    static void LoadGame();
    static void LoadedScene();

   public:
    static void Init(const int& width, const int& height);
    static void Quit();

    static void LoadLevel(Level* level);
    static bool Simulate();
    static void translate(SDL_Event* e);
    static void ScreenShot(const char* filename);
    static SDL_Surface* ScreenShot();
    [[deprecated]] static SDL_DisplayMode display();
    static void RequestQuit();
    static SDL_Window* GetWindow();
    static SDL_Renderer* GetRenderer();

    static void back_fail(void);
    static void fail(const std::string& message);
    static void fail_OutOfMemory();
};

}  // namespace RoninEngine
#endif
