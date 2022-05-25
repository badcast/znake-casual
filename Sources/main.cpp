#include <cstring>

#include "inputSystem.h"
#include "pch.h"

using namespace std;
using namespace RoninEngine;

// default Width screen
const int WindowWidth = 800;
// default Height screen
const int WindowHeight = 600;

int main(int /*argc*/, char** /*argv[]*/) {
#ifdef __WIN32__
    //release WINAPI console (hide console)
    FreeConsole();
#endif

    // SDL_SetMemoryFunctions(malloc, calloc, realloc, free);
    // SDL_SetMemoryFunctions(&std::malloc, &std::calloc, &std::realloc, &std::free);

    Application::Init(WindowWidth, WindowHeight);
    //Загружаем главное меню
    auto scene = GC::gc_alloc<MainMenu>();
    Application::LoadLevel(scene);

    bool result = Application::Simulate();
    if (result)
        Application::Quit();
    else
        Application::fail("Failed");

    int totaln = GC::gc_total_allocated();

    if (totaln != 0) static_assert(true, "Есть не освобожденные объекты в памяти. Приоритетно исправить!");

    return 0;
}
