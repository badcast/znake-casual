#include "pch.h"

using namespace RoninEngine::UI;

UI::GUI* gui;
namespace RoninEngine {
void controlEvent(const uint8_t&, void*);
// groups
uint8_t group_mainmenu_screen, group_startgame_screen, group_setting_screen, group_author_screen;
// controls
uint8_t mmb_start_game, mmb_setting, mmb_aboutus, mmb_quit;
uint8_t sgb_playgame, sgb_option, sgb_backmenu;
uint8_t ssb_backmenu;
uint8_t sab_backmenu;

//Запуск после инициализаций
void MainMenu::awake() {
    gui = ui;
    constexpr int _OFSET = 0x21;

    //Инициализация меню
    SDL_DisplayMode gameDisplay = Application::display();

    point_t point = {gameDisplay.w / 2 - 127, 300};
    point_t lastPoint;
    // Main menu screen

    group_mainmenu_screen = gui->Create_Group();
    {
        mmb_start_game = gui->Push_Button("Start Game", point, group_mainmenu_screen);
        point.y += _OFSET;
        mmb_setting = gui->Push_Button("Options", point, group_mainmenu_screen);
        point.y += _OFSET;
        mmb_aboutus = gui->Push_Button("About US", point, group_mainmenu_screen);
        point.y += _OFSET;
        mmb_quit = gui->Push_Button("Quit", point, group_mainmenu_screen);

        gui->Push_TextureAnimator(*GC::LoadTextures("gameLogo"), 0.15f, TimelineOptions::LinearReverse, {250, 20});
    }
    lastPoint = point;
    point = {gameDisplay.w / 2 - 127, 300};
    // Start game screen
    group_startgame_screen = gui->Create_Group();
    {
        sgb_playgame = gui->Push_Button("Play", point, group_startgame_screen);
        point.y += _OFSET;
        sgb_backmenu = gui->Push_Button("< Back", lastPoint, group_startgame_screen);
    }
    // Setting screen
    group_setting_screen = gui->Create_Group();
    {
        gui->Push_Label("Graphics", {gameDisplay.w / 2 - 127, 300}, fontWidth, group_setting_screen);
        ssb_backmenu = gui->Push_Button("< Back", lastPoint, group_setting_screen);
    }
    // Author & About US
    group_author_screen = gui->Create_Group();
    {
        gui->Push_Label("Night Fear", {gameDisplay.w / 2 - 127, 300}, fontWidth, group_author_screen);
        sab_backmenu = gui->Push_Button("< Back", lastPoint, group_author_screen);
    }

    // random text
    gui->Push_DisplayRanomizer_Number(0, INT_MAX, TextAlign::BottomRight);

    // Show as First
    gui->Show_GroupUnique(group_mainmenu_screen);
    // Register main event
    gui->Register_Callback((ui_callback)controlEvent, nullptr);
}

void controlEvent(const uint8_t& id, void* userData) {
    //Эти определения просто сравнивают ID :)
#define e(ID) ID == id
#define e2(ID1, ID2) ID1 == id || ID2 == id
#define e3(ID1, ID2, ID3) ID1 == id || ID2 == id || ID3 == id
#define e4(ID1, ID2, ID3, ID4) ID1 == id || ID2 == id || ID3 == id || ID4 == id
    // main menu events
    if (e(mmb_start_game)) {
        gui->Show_GroupUnique(group_startgame_screen);
    } else if (e(mmb_setting)) {
        gui->Show_GroupUnique(group_setting_screen);
    } else if (e(mmb_aboutus)) {
        gui->Show_GroupUnique(group_author_screen);
    } else if (e(mmb_quit)) {
        Application::RequestQuit();
    }
    // start game menu events
    else if (e(sgb_playgame)) {
        Application::LoadLevel(GC::gc_alloc<GameScene>());
    }
    // setting menu events
    else if (e3(sgb_backmenu, ssb_backmenu, sab_backmenu)) {
        gui->Show_GroupUnique(group_mainmenu_screen);
    }
#undef e
#undef e2
#undef e3
#undef e4
}

//Запуск до первого кадра
void MainMenu::start() {}
//Обновляет все что происходит внутри сцены (игры) до кадра
void MainMenu::update() {}

void MainMenu::lateUpdate() {}

void MainMenu::onDrawGizmos() {}

void MainMenu::onUnloading() {}

}  // namespace RoninEngine
