#pragma once
#include "CommonObject.h"
#include "BubbleManager.h"

class DrawThread
{
public:
	void operator()(CommonObjects& common);
	static void black_line();
};

