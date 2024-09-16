#include "DebugGraphsMannger.h"

void DebugGraphsMannger::Draw(Graphics& gfx)
{
    for (auto it = umapDebugGeoWithExistTime.begin(); it != umapDebugGeoWithExistTime.end(); )
    {
        if (it->second.first == KEEP_EXIST) {
            ++it;
            continue;
        }
        it->second.first -= time.Peek();
#ifndef _DEBUG
        if (it->second.second != DEBUG)
#endif // _DEBUG
            it->first->Draw(gfx);

        if (it->second.first < 0) {
            it->first->destory();
            it = umapDebugGeoWithExistTime.erase(it);
        }
        else {
            ++it;
        }
    }
    time.Mark();
}

void DebugGraphsMannger::AddGeo(std::shared_ptr<Drawable> geometry, float existTime, GeometryType type)
{
#ifndef _DEBUG
    if(type!=DEBUG)
#endif // _DEBUG
    umapDebugGeoWithExistTime[std::move(geometry)] = { existTime,type };
}

void DebugGraphsMannger::ChangeGeoExistTime(std::shared_ptr<Drawable> geometry, float DeltaTime)
{
    umapDebugGeoWithExistTime[geometry].first += DeltaTime;
}

DebugGraphsMannger& DebugGraphsMannger::GetInstence()
{
    static DebugGraphsMannger DGM;
    return DGM;
}