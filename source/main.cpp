#include <exception>
#include <iostream>

#include "levels/testlevel.h"
#include "levels/gamelevel.h"
#include "levels/terrain2deditor.h"

using namespace std;

int main() {
    using namespace RoninEngine;

    auto locale = setlocale(LC_ALL, nullptr);
    std::cout << "Current locale: " << locale << std::endl;
    Application::Init(1366, 768);

    Terrain2DEditor level;
    Application::LoadLevel(&level);

    Application::Simulate();

    Application::Quit();

    return 0;
}
