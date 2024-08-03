#pragma once
#include "CommonObject.h"
#include "BubbleManager.h"

// Function declarations
void SetupImGuiStyle();
void DrawMenuBar(CommonObjects* common);
void DisplayWelcomeText();
void DrawInputControls(CommonObjects* common, int& type_currentItem, int& country_currentItem, std::vector<bool>& expanded);
void DrawBreweryTable(CommonObjects* common, std::vector<bool>& expanded);
void DisplayNoDataMessage();
void ResetExpanded(std::vector<bool>& expanded);

class DrawThread
{
public:
	void operator()(CommonObjects& common);
	static void black_line();


};

