#pragma once
#ifndef BUBBLE_H
#define BUBBLE_H

#include "../../shared/ImGuiSrc/imgui.h"
#include <vector>
#include <cstdlib>

class Bubble {
public:
    Bubble(ImVec2 position, float radius, float speed);
    void Update(float deltaTime, ImVec2 windowSize);
    void Draw(ImDrawList* drawList) const;

private:
    ImVec2 position;
    float radius;
    float speed;
};



#endif // BUBBLE_H
