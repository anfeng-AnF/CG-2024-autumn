#include "DebugGraphsMannger.h"

void DebugGraphsMannger::Draw(Graphics& gfx)
{
    for (auto& geo : umapDebugGeoWithExistTime) {
        if (geo.second == KEEP_EXIST)continue;
        geo.second -= time.Peek();
        if (geo.second < 0) {
            umapDebugGeoWithExistTime.erase(geo.first);
        }
    }
    time.Mark();
}

void DebugGraphsMannger::AddGeo(Drawable* geometry, float existTime)
{
    this->umapDebugGeoWithExistTime[geometry] = existTime;
}

void DebugGraphsMannger::ChangeGeoExistTime(Drawable* geometry, float DeltaTime)
{
    if (umapDebugGeoWithExistTime.find(geometry) == umapDebugGeoWithExistTime.end())
    {
        umapDebugGeoWithExistTime[geometry] = DeltaTime;
    };
    umapDebugGeoWithExistTime[geometry] += DeltaTime;
}

DebugGraphsMannger& DebugGraphsMannger::GetDGMRefference()
{
    static DebugGraphsMannger DGM;
    return DGM;
}
