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
    return glm::length(diff) < radius;
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
    fullyOpenedThisFrame = false;

    if (isOpen)
    {
        // Swing open toward openAngle
        if (std::abs(rotation) < std::abs(openAngle))
        {
            float dir = (openAngle >= 0.f) ? 1.f : -1.f;
            rotation += dir * swingSpeed * static_cast<float>(dt);

            // Clamp
            if (std::abs(rotation) >= std::abs(openAngle))
            {
                rotation = openAngle;
                isAnimating = false;

                // Fire the walk-through check once per open completion
                fullyOpenedThisFrame = true;
            }
        }
    }
    else
    {
        // Swing closed toward 0
        if (std::abs(rotation) > 0.f)
        {
            float dir = (rotation > 0.f) ? -1.f : 1.f;
            rotation += dir * swingSpeed * static_cast<float>(dt);

            if ((rotation > 0.f && dir < 0.f && rotation < 0.f) || rotation * dir > 0.f)
                rotation = 0.f;

            if (std::abs(rotation) < 0.01f)
            {
                rotation = 0.f;
                isAnimating = false;
            }
        }
    }

    // Return true once the door is fully open AND the player is inside it
    if (isOpen && std::abs(rotation) >= std::abs(openAngle))
    {
        return IsPlayerInside(playerPos, playerHalfX, playerHalfZ);
    }

    return false;
}