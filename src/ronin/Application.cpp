
#include "framework.h"

using namespace UI;

namespace RoninEngine {
Level* _lastSceneToFree = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Window* window = nullptr;
bool m_inited = false;
bool m_sceneAccept = false;
Level* m_level = nullptr;
bool m_sceneLoaded = false;
bool _queryQuit;

void Application::Init(const std::uint32_t& width, const std::uint32_t& height) {
    char errorStr[128];
    if (m_inited) return;

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO)) fail("Fail init main system.");

    // init graphics
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF)) fail("Fail init imageformats. (libPNG, libJPG) not defined");

    // init Audio system
    if (!Mix_Init(MIX_InitFlags::MIX_INIT_OGG | MIX_InitFlags::MIX_INIT_MP3)) fail("Fail init audio.");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512)) fail("Fail open audio.");

    window = SDL_CreateWindow("Ronin Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height,
                              SDL_WINDOW_SHOWN);

    if(!window)
        fail(SDL_GetErrorMsg(errorStr, 128));

    renderer = SDL_CreateRenderer(window, -1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer)
        fail(SDL_GetErrorMsg(errorStr, 128));

    // Brightness - Яркость
    SDL_SetWindowBrightness(window, 1);

    Time::Init_TimeEngine();

    LoadGame();
    m_inited = true;
    _queryQuit = false;
}

void Application::RequestQuit() { _queryQuit = true; }

void Application::Quit() {
    if (!m_inited) return;

    GC::gc_free();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Free_Controls();
    // sdl quit
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();

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
    //SDL_SetCursor(GC::GetCursor("cursor", {1, 1}));

    Levels::Level_Init();
}

void Application::LoadedLevel() {
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

    m_level->awake();
}

void Application::LoadLevel(Level* level) {
    if (!level || m_level == level) throw std::bad_cast();

    if (m_level) {
        _lastSceneToFree = m_level;
        m_level->Unload();
    }

    if (!level->is_hierarchy()) {
        // init main object
        level->main_object = create_empty();

        if (level->main_object == nullptr) throw std::bad_exception();

        level->main_object->transform()->name("Root Transform");
    }

    m_level = level;
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

SDL_DisplayMode Application::getDisplayMode() {
    SDL_DisplayMode display;
    SDL_GetWindowDisplayMode(window, &display);
    return display;
}

RoninEngine::Resolution RoninEngine::Application::getResolution()
{
    Resolution res;
    SDL_RendererInfo rf;
    SDL_GetRendererInfo(renderer, &rf);

    SDL_GetWindowSize(window, &res.width, &res.height);
    return res;
}

bool Application::Simulate() {
    /**/ SDL_Event e;
    /**/ int firstStep;
    /**/ char _title[128];
    /**/ float fpsRound = 0;
    /**/ float fps = 0;
    SDL_WindowFlags wndFlags;

    while (!_queryQuit) {
        input::Reset();
        wndFlags = static_cast<SDL_WindowFlags>(SDL_GetWindowFlags(Application::GetWindow()));
        while (SDL_PollEvent(&e)) {
            input::Update_Input(&e);
            if (e.type == SDL_QUIT) _queryQuit = true;
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
                LoadedLevel();
                m_sceneLoaded = true;
            } else {
                //Обновляем сцену
                if (!m_sceneAccept) {
                    SDL_RenderFlush(renderer);  // flush renderer before first render
                    m_level->start();
                    m_sceneAccept = true;
                } else {
                    m_level->update();
                }

                m_level->RenderLevel(renderer);
                m_level->onDrawGizmos();  // Draw gizmos
                m_level->RenderUI(renderer);

                if (!_lastSceneToFree) {
                    SDL_RenderPresent(renderer);
                    m_level->lateUpdate();
                    m_level->RenderSceneLate(renderer);
                }
            }

            fps = SDL_ceilf(Time::frame() / (SDL_GetTicks() / 1000.f));
            if (Time::time() > fpsRound) {
#ifdef _GLIBCXX_DEBUG_ONLY
                std::sprintf(_title,
                             "Ronin Engine (Debug) FPS:%d Memory:%luMiB, "
                             "GC_Allocated:%lu, SDL_Allocated:%d",
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

