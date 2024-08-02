#pragma once
#ifndef BUBBLEMANAGER_H
#define BUBBLEMANAGER_H

#include "../../shared/ImGuiSrc/imgui.h"
#include <vector>
#include <cstdlib>
#include "Bubble.h"

class BubbleManager {
public:
    BubbleManager(int count, ImVec2 windowSize);
    void Update(float deltaTime, ImVec2 windowSize);
    void Draw(ImDrawList* drawList) const;

private:
    std::vector<Bubble> bubbles;
    void InitializeBubbles(int count, ImVec2 windowSize);
};


#endif 