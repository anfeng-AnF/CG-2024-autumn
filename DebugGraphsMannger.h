#pragma once
#include "Graphics.h"
#include "Drawable.h"
#include "unordered_map"
#include "ChiliTimer.h"
#include "DebugLine.h"
#include "DebugSphere.h"

#define KEEP_EXIST -D3D11_FLOAT32_MAX
class DebugGraphsMannger
{
public:
    enum GeometryType
    {
        DEBUG,
        USER_AID
    };
public:
    void Draw(Graphics& gfx);
    void AddGeo(std::shared_ptr<Drawable> geometry, float existTime = 5.0f, GeometryType type = DEBUG);
    void ChangeGeoExistTime(std::shared_ptr<Drawable> geometry, float DeltaTime = 0.0f);
    static DebugGraphsMannger& GetInstence();

private:
    DebugGraphsMannger() {};
    DebugGraphsMannger(const DebugGraphsMannger&) = delete;
    DebugGraphsMannger& operator=(const DebugGraphsMannger&) = delete;

private:
    std::unordered_map<std::shared_ptr<Drawable>, std::pair<float,GeometryType>> umapDebugGeoWithExistTime;
    ChiliTimer time;
};
