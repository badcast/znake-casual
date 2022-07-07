#include "framework.h"

namespace RoninEngine {
float Time::timeScale, Time::_lastTime, Time::_time, Time::_deltaTime;
uint32_t Time::_frames;
float _startedTime = 0;

void Time::Init_TimeEngine() {
    _time = 0;
    _lastTime = 0;
    timeScale = 1;
    _startedTime = 0;
    _startedTime = startUpTime();
}

float Time::time() { return _time; }

float Time::startUpTime() { return static_cast<float>(tickMillis() - _startedTime); }

float Time::deltaTime() { return _deltaTime; }

bool Time::is_paused() { return timeScale == 0; }

float Time::get_time_scale() { return Time::timeScale; }

void Time::set_time_scale(float scale) { Time::timeScale = Mathf::Min(Mathf::Max(scale, 0.F), 1.f); }

std::uint32_t Time::frame() { return _frames; }

std::uint32_t Time::tickMillis() { return SDL_GetTicks(); }

}  // namespace RoninEngine
