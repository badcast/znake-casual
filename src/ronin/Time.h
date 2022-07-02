#ifndef _TIME_H_____
#define _TIME_H_____

namespace RoninEngine
{
	class Time
	{
	private:
		static float timeScale, _lastTime, _time, _deltaTime;
		static uint32_t _frames;
	public:

		static void Init_TimeEngine();
		//обновляет локальное время для метода time()
		static void update();
		static float time();
		static float startUpTime();
		static float deltaTime();
		static bool is_paused();
		static float get_time_scale();
		static void set_time_scale(float scale);
		static uint32_t frame();
	};
}
#endif