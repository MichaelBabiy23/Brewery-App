#include "Bubble.h" // Include the header for the Bubble class

// Constructor for the Bubble class
Bubble::Bubble(ImVec2 position, float radius, float speed)
    : position(position), radius(radius), speed(speed) {}

// Update the position of the bubble
void Bubble::Update(float deltaTime, ImVec2 windowSize) {
    position.y -= speed * (deltaTime); // Move bubble upwards
    if (position.y + radius < 0) { // If bubble goes off screen, reset its position
        position.y = windowSize.y + radius; // Reset y position below the screen
        position.x = rand() % (int)windowSize.x; // Reset x position randomly within window width
        //printf("reset");
    }
}

// Draw the bubble
void Bubble::Draw(ImDrawList* drawList) const {
    drawList->AddCircleFilled(position, radius, IM_COL32(200, 200, 200, 128)); // Draw bubble with specified position, radius, and color
}
