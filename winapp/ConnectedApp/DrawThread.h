#pragma once
#include "CommonObject.h"

class DrawThread
{
public:
	void operator()(CommonObjects& common);
	static void black_line();
	//void DrawAppWindow();
};

