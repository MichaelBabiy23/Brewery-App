#include "BubbleManager.h" // Include the header for the BubbleManager class
#include <time.h> // Include the time library for random number seeding

// Constructor for the BubbleManager class
BubbleManager::BubbleManager(int count, ImVec2 windowSize) {
    InitializeBubbles(count, windowSize); // Initialize the bubbles with the given count and window size
}

// Initialize the bubbles with random positions, sizes, and speeds
void BubbleManager::InitializeBubbles(int count, ImVec2 windowSize) {
    bubbles.clear(); // Clear any existing bubbles
    srand((time(0))); // Seed the random number generator with the current time

    for (int i = 0; i < count; ++i) {
        bubbles.emplace_back(
            ImVec2(rand() % (int)windowSize.x, windowSize.y + rand() % (int)(windowSize.y * 0.5f)), // Random x position, start y position below the screen
            3.0f + static_cast<float>(rand() % 7), // Random radius between 3 and 10
            20.0f + static_cast<float>(rand() % 40) // Random speed between 20 and 60
        );
    }
}

// Update the position of all bubbles
void BubbleManager::Update(float deltaTime, ImVec2 windowSize) {
    for (Bubble& bubble : bubbles) {
        bubble.Update(deltaTime, windowSize); // Update each bubble's position
    }
}

// Draw all the bubbles
void BubbleManager::Draw(ImDrawList* drawList) const {
    for (const Bubble& bubble : bubbles) {
        bubble.Draw(drawList); // Draw each bubble
    }
}
