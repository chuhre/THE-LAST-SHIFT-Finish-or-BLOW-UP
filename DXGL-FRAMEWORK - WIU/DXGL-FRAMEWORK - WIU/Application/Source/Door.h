#ifndef DOOR_H
#define DOOR_H

#include <glm/glm.hpp>
#include "SceneManager.h"

class Door
{
public:
    // Door state
    glm::vec3 position;
    float width;
    float height;
    SceneManager::SCENE_TYPE leadsTo;

    // Animation state
    float rotation;       // 0 = closed, 90 = open
    bool isOpen;
    bool isAnimating;

    Door();
    Door(glm::vec3 pos, float w, float h, SceneManager::SCENE_TYPE dest);

    // Call every frame — handles open/close animation
    // Returns true if the door has fully opened (scene switch should happen)
    bool Update(double dt, const glm::vec3& playerPos, float playerHalfX, float playerHalfZ);
    void Open();
    void Close();
    bool IsPlayerNear(const glm::vec3& playerPos, float radius) const;
    bool IsPlayerInside(const glm::vec3& playerPos, float playerHalfX, float playerHalfZ) const;

    float openAngle;
    float swingSpeed;

private:
    bool fullyOpenedThisFrame; 
};

#endif 
