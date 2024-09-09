#include "OperatorManager.h"

OperatorManager& OperatorManager::GetInstance(Mouse& m, Keyboard& k, Window& w)
{
    static OperatorManager OM(m, k, w);
    return OM;
}

void OperatorManager::DoFrame()
{

}

OperatorManager::OperatorManager(Mouse& m, Keyboard& k, Window& w):
    mouse(m),
    kbd(k),
    wnd(w)
{
}
