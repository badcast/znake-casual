#include "LevelEntity.h"
#include "ObjectParser.h"
#include "inputSystem.h"
#include "pch.h"
#include "sysInfo.h"

using namespace UI;

namespace RoninEngine {
Level* _lastSceneToFree = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;
bool m_inited = false;
bool m_sceneAccept = false;
Level* m_scene = nullptr;
bool m_sceneLoaded = false;

void Application::Init(const int& width, const int& height) {
    if (m_inited) return;

    // set locale
    setlocale(LC_ALL, "ru_RU.Windows1251");

    int allocated, lalloc;
    lalloc = allocated = GC::gc_total_allocated();
    jno::jno_object_parser parser;
    parser.deserialize("./obj.jno");
    allocated = GC::gc_total_allocated();
    string words = parser.find_node("node/hello")->toString() + parser.find_node("node/world")->toString();

    allocated = GC::gc_total_allocated();

    // SDL main init
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO)) fail("Fail init main system.");

    // init graphics
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG)) fail("Fail init imageformats. zlib (libPNG, libJPG) not defined");

    // init Audio system
    if (!Mix_Init(MIX_InitFlags::MIX_INIT_OGG | MIX_InitFlags::MIX_INIT_MP3)) fail("Fail init audio.");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512)) fail("Fail open audio.");

    window = SDL_CreateWindow("NightFear@lightmister", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Brightness - Яркость
    SDL_SetWindowBrightness(window, 1);

    Time::Init_TimeEngine();

    LoadGame();
    m_inited = true;
}

void Application::Quit() {
    if (!m_inited) return;

    GC::gc_free();
    // SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Free_Controls();
    // sdl quit
    SDL_Quit();
    IMG_Quit();
    Mix_Quit();
    m_inited = false;
}

void Application::LoadGame() {
    GC::gc_lock();

    //Загружаем данные и готовим программу к запуску

    GC::CheckResources();

    // initialize GC
    GC::gc_init();

    string path = dataAt(FolderKind::LOADER);
    string temp = path + "graphics.conf";
    GC::LoadImages(temp.c_str());

    // load textures
    path = dataAt(FolderKind::LOADER);
    temp = path + "textures.conf";
    GC::LoadImages(temp.c_str());

    //Загрузк шрифта и оптимизация дэффектов
    UI::Initialize_Fonts(true);

    //Инициализация инструментов
    InitalizeControls();

    // Set cursor
    SDL_SetCursor(GC::GetCursor("cursor", {1, 1}));

    Levels::Level_Init();
}

void Application::LoadedScene() {
    if (_lastSceneToFree) {
        _lastSceneToFree->onUnloading();

        GC::gc_unalloc(_lastSceneToFree);

        _lastSceneToFree = nullptr;
        // GC::UnloadUnused();
        GC::gc_free_source();
    }

    m_sceneLoaded = true;

    // capture memory as GC
    GC::gc_unlock();

    m_scene->awake();
}

void Application::LoadLevel(Level* level) {
    if (!level || m_scene == level) throw std::bad_cast();

    if (m_scene) {
        _lastSceneToFree = m_scene;
        m_scene->Unload();
    }

    if (!level->is_hierarchy()) {
        // init main object
        level->main_object = create_empty();

        if (level->main_object == nullptr) throw std::bad_exception();

        level->main_object->transform()->name() = "Root Transform";
    }

    m_scene = level;
    m_sceneAccept = false;
    m_sceneLoaded = false;
}

SDL_Surface* Application::ScreenShot() {
    SDL_Rect rect;
    void* pixels;
    int pitch;

    SDL_RenderGetViewport(renderer, &rect);
    pitch = (rect.w - rect.x) * 4;
    pixels = malloc(pitch * (rect.h - rect.y));
    // read the Texture buffer
    SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);

    SDL_Surface* su =
        SDL_CreateRGBSurfaceFrom(pixels, pitch / 4, rect.h - rect.y, 32, pitch, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    return su;
}

void Application::ScreenShot(const char* filename) {
    SDL_Surface* surf = ScreenShot();
    IMG_SavePNG(surf, filename);
    SDL_FreeSurface(surf);
}

SDL_DisplayMode Application::display() {
    SDL_DisplayMode display;
    SDL_GetWindowDisplayMode(window, &display);
    return display;
}

void Application::RequestQuit() {
    input::event->type = SDL_QUIT;
    SDL_PushEvent(input::event);
}

bool Application::Simulate() {
    /**/ SDL_Event e;
    /**/ SDL_DisplayMode displayMode;
    /**/ bool _queryQuit = false;
    /**/ int firstStep;
    /**/ char _title[128];
    /**/ float fpsRound = 0;
    /**/ float fps = 0;
    SDL_WindowFlags wndFlags;

    SDL_GetDisplayMode(0, 0, &displayMode);
    while (!_queryQuit) {
        wndFlags = static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(Application::GetWindow()));

        while (SDL_PollEvent(&e)) {
            input::Update_Input(&e);

            _queryQuit = e.type == SDL_QUIT;
        }

        if ((wndFlags & SDL_WindowFlags::SDL_WINDOW_MINIMIZED) != SDL_WindowFlags::SDL_WINDOW_MINIMIZED) {
            // update
            input::Late_Update();

            // Updating
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black clear
            //Очистка
            SDL_RenderClear(renderer);

            if (!m_sceneLoaded) {
                // free cache
                LoadedScene();
                m_sceneLoaded = true;
            } else {
                //Обновляем сцену
                if (!m_sceneAccept) {
                    SDL_RenderFlush(renderer);  // flush renderer before first render
                    m_scene->start();
                    m_sceneAccept = true;
                } else {
                    m_scene->update();
                }

                m_scene->RenderScene(renderer);
                m_scene->onDrawGizmos();  // Draw gizmos
                m_scene->RenderUI(renderer);

                if (!_lastSceneToFree) {
                    SDL_RenderPresent(renderer);
                    m_scene->lateUpdate();
                    m_scene->RenderSceneLate(renderer);
                }
            }

            input::Reset();
            translate(&e);

            fps = SDL_ceilf(Time::frame() / (SDL_GetTicks() / 1000.f));
            if (Time::time() > fpsRound) {
#ifdef _GLIBCXX_DEBUG_ONLY
                std::sprintf(_title,
                             "NightFear (Debug) FPS:%d Memory:%luMiB, "
                             "GC_Allocated:%d, SDL_Allocated:%d",
                             static_cast<int>(fps), get_process_sizeMemory() / 1024 / 1024, GC::gc_total_allocated(),
                             SDL_GetNumAllocations());

#endif
                SDL_SetWindowTitle(Application::GetWindow(), _title);
                fpsRound = Time::time() + 0.15f;
            }

            Time::update();
        }
    }

    return _queryQuit;
}

void Application::translate(SDL_Event* e) {}

SDL_Window* Application::GetWindow() { return window; }

SDL_Renderer* Application::GetRenderer() { return renderer; }

void Application::back_fail(void) { exit(EXIT_FAILURE); }

void Application::fail(const std::string& message) {
    std::string _template = message;
    char _temp[32]{0};
    tm* ltime;
    time_t tt;
    time(&tt);

    ltime = localtime(&tt);
    strftime(_temp, sizeof(_temp), "%d.%m.%y %H:%M:%S", ltime);
    _template += "\n\n\t";
    _template += _temp;

    fprintf(stderr, "%s", _template.data());

    SDL_LogMessage(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL, _template.data());
    SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fail", _template.data(), window);
    back_fail();
}

void Application::fail_OutOfMemory() { fail("Out of memory!"); }
}  // namespace RoninEngine
