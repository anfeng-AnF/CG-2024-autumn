#pragma once
#include "AActor.h"
#include "Graphics.h"
class AWangshuInn :public AActor
{
	typedef AActor Super;
public:
	AWangshuInn(Graphics &gfx);
	virtual void Render(Graphics& Gfx)override;
};