#include <exception>
#include <iostream>

#include "levels/gamelevel.h"
#include "levels/terrain2deditor.h"
#include "levels/testlevel.h"

using namespace std;

int main() {
    using namespace RoninEngine;
    setlocale(LC_ALL, "");

    Application::Init(1366, 768);

    GameLevel level;
    Application::LoadLevel(&level);

    Application::Simulate();

    Application::Quit();

    return EXIT_SUCCESS;
}
