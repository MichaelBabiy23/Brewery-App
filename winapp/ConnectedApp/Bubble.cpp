#include "Bubble.h"

Bubble::Bubble(ImVec2 position, float radius, float speed)
    : position(position), radius(radius), speed(speed) {}

void Bubble::Update(float deltaTime, ImVec2 windowSize) {
    position.y -= speed * (deltaTime); // Move bubble upwards
    if (position.y + radius < 0) { // If bubble goes off screen, reset its position
        position.y = windowSize.y + radius;
        position.x = rand() % (int)windowSize.x;
        //printf("reset");
    }
}

void Bubble::Draw(ImDrawList* drawList) const {
    drawList->AddCircleFilled(position, radius, IM_COL32(200, 200, 200, 128)); // Draw bubble

}

