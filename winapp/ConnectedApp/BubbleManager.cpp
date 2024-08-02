#include "BubbleManager.h"
#include <time.h>

BubbleManager::BubbleManager(int count, ImVec2 windowSize) {
    InitializeBubbles(count, windowSize);
}

void BubbleManager::InitializeBubbles(int count, ImVec2 windowSize) {
    bubbles.clear();
    srand((time(0))); // Seed the random number generator

    for (int i = 0; i < count; ++i) {
        bubbles.emplace_back(
            ImVec2(rand() % (int)windowSize.x, windowSize.y + rand() % (int)(windowSize.y * 0.5f)), // random x, start below screen
            3.0f + static_cast<float>(rand() % 7), // random radius between 3 and 10
            20.0f + static_cast<float>(rand() % 40) // random speed between 20 and 60
        );

    }
}

void BubbleManager::Update(float deltaTime, ImVec2 windowSize) {
    for (Bubble& bubble : bubbles) {
        bubble.Update(deltaTime, windowSize);
    }
}

void BubbleManager::Draw(ImDrawList* drawList) const {
    for (const Bubble& bubble : bubbles) {
        bubble.Draw(drawList);
    }
}
