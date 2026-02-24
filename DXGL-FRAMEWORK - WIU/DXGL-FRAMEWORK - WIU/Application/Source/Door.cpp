#include "Door.h"
#include <cmath>

Door::Door()
    : position(0.f), width(1.5f), height(2.5f),
    leadsTo(SceneManager::SCENE_DUCKS),
    rotation(0.f), isOpen(false), isAnimating(false),
    openAngle(90.f), swingSpeed(90.f),
    fullyOpenedThisFrame(false)
{
}

Door::Door(glm::vec3 pos, float w, float h, SceneManager::SCENE_TYPE dest)
    : position(pos), width(w), height(h), leadsTo(dest),
    rotation(0.f), isOpen(false), isAnimating(false),
    openAngle(90.f), swingSpeed(90.f),
    fullyOpenedThisFrame(false)
{
}

void Door::Open()
{
    isOpen = true;
    isAnimating = true;
}

void Door::Close()
{
    isOpen = false;
    isAnimating = true;
}

bool Door::IsPlayerNear(const glm::vec3& playerPos, float radius) const
{
    glm::vec3 diff = playerPos - position;
    return glm::length(diff) <= radius;
}

bool Door::IsPlayerInside(const glm::vec3& playerPos, float playerHalfX, float playerHalfZ) const
{
    float dx = std::abs(playerPos.x - position.x);
    float dz = std::abs(playerPos.z - position.z);
    return (dx < width * 0.5f + playerHalfX) &&
        (dz < height * 0.5f + playerHalfZ);  // 'height' used as depth here for AABB
}

bool Door::Update(double dt, const glm::vec3& playerPos, float playerHalfX, float playerHalfZ)
{    
    // Animate door rotation
    if (isOpen && rotation < 90.0f)
    {
        rotation += 90.0f * static_cast<float>(dt);
        if (rotation > 90.0f) rotation = 90.0f;
    }
    else if (!isOpen && rotation > 0.0f)
    {
        rotation -= 90.0f * static_cast<float>(dt);
        if (rotation < 0.0f) rotation = 0.0f;
    }

    // Return true once the door is fully open AND the player is inside it
    if (isOpen && std::abs(rotation) >= std::abs(openAngle))
    {
        return IsPlayerInside(playerPos, playerHalfX, playerHalfZ);
    }

    return false;
}