#ifndef FPCAMERA_H
#define FPCAMERA_H

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

class FPCamera
{
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;

    FPCamera();
    ~FPCamera();

    void Init(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0,1,0));
    void Reset();
    void Update(double dt);

    void RotateYaw(float degrees);

private:

    bool isDirty;   // indicate if there is a need to recalculate the camera attributes
    float yaw;   // Horizontal rotation
    float pitch; // Vertical rotation
    void Refresh();
};

#endif

