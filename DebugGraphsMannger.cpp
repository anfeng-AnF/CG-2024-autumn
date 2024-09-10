#include "DebugGraphsMannger.h"

void DebugGraphsMannger::Draw(Graphics& gfx)
{
    for (auto it = umapDebugGeoWithExistTime.begin(); it != umapDebugGeoWithExistTime.end(); )
    {
        if (it->second == KEEP_EXIST) {
            ++it;
            continue;
        }
        it->second -= time.Peek();
        it->first->Draw(gfx);
        if (it->second < 0) {
            it = umapDebugGeoWithExistTime.erase(it);
        }
        else {
            ++it;
        }
    }
    time.Mark();
}

void DebugGraphsMannger::AddGeo(std::shared_ptr<Drawable> geometry, float existTime)
{
    umapDebugGeoWithExistTime[std::move(geometry)] = existTime;
}

void DebugGraphsMannger::ChangeGeoExistTime(std::shared_ptr<Drawable> geometry, float DeltaTime)
{
    umapDebugGeoWithExistTime[geometry] += DeltaTime;
}

DebugGraphsMannger& DebugGraphsMannger::GetDGMRefference()
{
    static DebugGraphsMannger DGM;
    return DGM;
}