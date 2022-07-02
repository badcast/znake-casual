#pragma once

#include "dependency.h"

namespace RoninEngine::Runtime {

	class Random {
	public:
		static void srand(int seed);

		static int range(int min, int max);

		static float range(float min, float max);

		static float value();

          static Vec2 randomVector();

		static float randomAngle();
	};

	class Mathf
	{
	public:
        static constexpr float FloatMinNormal = 1.175494E-38;
        static constexpr float FloatMinDenormal = 1.401298E-45;
        static constexpr bool IsFlushToZeroEnabled = FloatMinNormal == 0;
        static constexpr float Epsilon = (!IsFlushToZeroEnabled) ? FloatMinDenormal : FloatMinNormal;
        static constexpr long double LongPI = 3.14159265358979323846264338327950288;
        static constexpr float PI = static_cast<float>(LongPI); // calc PI formula l / d (Length circle div diameter)
        static constexpr float Infinity = 1e+600;
        static constexpr float NegativeInfinity = -Infinity;
        static constexpr float Deg2Rad = 0.01745329;
        static constexpr float Rad2Deg = 180 / PI;
        static constexpr int RLevelDigits = 10000;

		static bool Approximately(float a, float b);

		static float cos(float x);

		static float sin(float x);

		static float tan(float x);

		static float acos(float x);

		static float atan(float x);

		static float atan2(float y, float x);

		static float Clamp01(float val);

		static float Clamp(float val, const float min, const float max);

		static int Max(int x, int y);

		static float Max(float x, float y);

		static int Min(int x, int y);

		static float Min(float x, float y);

		static float abs(float x);

		static int abs(int x);

		//negative absolute value
		static float nabs(float x);

		static int nabs(int x);

		static float sign(float x);

		static int sign(int x);

		static float round(float x);

		static float ceil(float x);

		static float floor(float x);

		static float frac(float x);

		static int number(float x);

		static float exp(float x);

        static float pow2(float x);

        static int pow2(int x);

		static int pow(int x, int y);

		static float pow(float x, float y);

		static float sqrt(float x);

		static float repeat(float t, float length);

		static float DeltaAngle(float current, float target);

		static float Gamma(float value, float absmax, float gamma);

		static float InverseLerp(float a, float b, float value);

		static float Lerp(float a, float b, float t);

		static float LerpAngle(float a, float b, float t);

		static float LerpUnclamped(float a, float b, float t);

		static bool LineIntersection(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec2& result);

		static float MoveTowards(float current, float target, float maxDelta);

		static bool LineSegmentIntersection(Vec2 p1, Vec2 p2, Vec2 p3, Vec2 p4, Vec2& result);

		static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed);

		static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime);

		static float SmoothDamp(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

		static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed);

		static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime);

		static float SmoothDampAngle(float current, float target, float& currentVelocity, float smoothTime, float maxSpeed, float deltaTime);

		static float SmoothStep(float from, float to, float t);
	};
}

