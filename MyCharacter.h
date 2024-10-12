#pragma once
#include "ACharacter.h"
#include "Graphics.h"

class MyCharacter :public ACharacter
{
	typedef ACharacter Super;
public:
	MyCharacter(Graphics&gfx);
	~MyCharacter();

};