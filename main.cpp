#include <exception>
#include <iostream>

#include "levels/testlevel.h"
#include "levels/gamelevel.h"

using namespace std;

int main() {
    using namespace RoninEngine;

    Application::Init(800, 600);


    GameLevel lvl;
    Application::LoadLevel(&lvl);

    Application::Simulate();

    Application::Quit();

    return 0;
}
