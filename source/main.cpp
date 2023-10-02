#include <exception>
#include <iostream>

#include "levels/gamelevel.h"

using namespace std;
using namespace RoninEngine;

#if WIN32
typedef void *HINSTANCE;
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char *lpCmdLine, int nShowCmd)
#else
int main()
#endif
{

    RoninSimulator::Init();
    RoninSimulator::Show({1024, 600}, false);
    RoninSimulator::SetDebugMode(true);
    RoninSimulator::LoadWorld<GameLevel>();
    RoninSimulator::Simulate();
    RoninSimulator::Finalize();

    return EXIT_SUCCESS;
}
