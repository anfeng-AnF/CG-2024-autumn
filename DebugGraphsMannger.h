#pragma once
#include "Graphics.h"
#include "Drawable.h"
#include "unordered_map"
#include "ChiliTimer.h"

#define KEEP_EXIST -D3D11_FLOAT32_MAX
class DebugGraphsMannger
{
public:
	void Draw(Graphics& gfx);
	void AddGeo(Drawable* geometry, float existTime = 5.0f);
	void ChangeGeoExistTime(Drawable* geometry,float DeltaTime=0.0f);
	static DebugGraphsMannger& GetDGMRefference();

private:
	DebugGraphsMannger();
	DebugGraphsMannger(const DebugGraphsMannger&) = delete;
	DebugGraphsMannger operator=(const DebugGraphsMannger&) = delete;
private:
	std::unordered_map<Drawable*, float>umapDebugGeoWithExistTime;
	ChiliTimer time;
};
