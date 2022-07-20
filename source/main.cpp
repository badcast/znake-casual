#include <exception>
#include <iostream>

#include "levels/testlevel.h"
#include "levels/gamelevel.h"
#include "levels/terrain2deditor.h"

using namespace std;

int main() {
    using namespace RoninEngine;

    Application::Init(800, 600);

    GameLevel level;
    Application::LoadLevel(&level);

    Application::Simulate();

    Application::Quit();

    return 0;
}
