#include "OperatorManager.h"

OperatorManager& OperatorManager::GetInstance(Mouse& m, Keyboard& k, Window& w)
{
    static OperatorManager OM(m, k, w);
    return OM;
}

void OperatorManager::DoFrame()
{
    switch (currentStatue)
    {
    case OperatorManager::Default:
        OnDefault();
        break;
    case OperatorManager::CameraMove:
        OnCameraMove();
        break;
    case OperatorManager::CtrlGeo:
        OnCtrlGeo();
        break;
    }
}

OperatorManager::OperatorManager(Mouse& m, Keyboard& k, Window& w):
    mouse(m),
    kbd(k),
    wnd(w)
{
}
