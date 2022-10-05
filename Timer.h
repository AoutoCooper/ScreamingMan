#pragma once

#include <SDL.h>

class Timer {

private:

	static Timer* sInstance;

	unsigned int mStartTicks;
	unsigned int mElapsedTicks;
	float mDeltaTime;
	float mTimeScale;

public:
	static Timer* Instance();
	static void Release();

	void Reset();
	float GetDeltaTime();
	void SetTimeScale(float t);
	float GetTimeScale();
	void Update();

private:

	Timer();
	~Timer();
};


