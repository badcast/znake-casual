#include "pch.h"

namespace RoninEngine
{
	float Time::timeScale, Time::_lastTime, Time::_time, Time::_deltaTime;
	uint32_t Time::_frames;
	float _startedTime = 0;

	void Time::Init_TimeEngine()
    {
		_time = 0;
		_lastTime = 0;
		timeScale = 1;
		_startedTime = startUpTime();

		update();
	}

	//обновляет локальное время для метода time()
	void Time::update()
	{
		float _newTime = startUpTime();
		++_frames;
		_deltaTime = static_cast<float>(Mathf::Clamp01((float)_time - _lastTime));
		if (_deltaTime == 1)
		{
			_lastTime = _time;
		}
		_time = _newTime;
	}

	float Time::time()
	{
		return _time;
	}

	float Time::startUpTime()
	{
		return static_cast<float>(SDL_GetTicks()) / 1000 - _startedTime;
	}

	float Time::deltaTime()
	{
		return _deltaTime;
	}

	bool Time::is_paused()
	{
		return timeScale==0;
	}

	float Time::get_time_scale()
	{
		return Time::timeScale;
	}

	void Time::set_time_scale(float scale)
	{
		Time::timeScale = fmin((float)fmax((float)scale, 0.F), 1);
	}

	uint32_t Time::frame() {
		return _frames;
	}

}
