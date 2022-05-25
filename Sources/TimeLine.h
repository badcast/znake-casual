#pragma once

#include "framework.h"

using namespace RoninEngine::Runtime;

namespace RoninEngine::UI
{
	struct TimelineRoad
	{
		Texture* texture;
		float duration;
	}; 

	enum class TimelineOptions
	{
		Linear,
		LinearReverse
	};

	class Timeline
	{
	private:
		TimelineOptions _option;
		list<TimelineRoad> _roads;
		list<TimelineRoad>::iterator iter;
		char state;
	public: 
		Timeline();
		~Timeline();

		void AddRoad(const TimelineRoad& road);
		void AddRoad(Texture* texture, const float duration);
		void AddRoads(const list<TimelineRoad>& roads);

		TimelineRoad* Evaluate(float time);

		void SetOptions(TimelineOptions option);
		TimelineOptions GetOptions();

		inline void Reset();
		inline void Stop();
		inline void Play();
		inline void Pause();
		inline bool isPlay();
		inline bool isPause();
	};
}