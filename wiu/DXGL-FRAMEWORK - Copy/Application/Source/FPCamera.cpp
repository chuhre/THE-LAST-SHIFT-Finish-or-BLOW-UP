#include "FPCamera.h"
#include "KeyboardController.h"

//Include GLFW
#include <GLFW/glfw3.h>
#include <MouseController.h>
#include <iostream>

FPCamera::FPCamera() : isDirty(false)
{
	this->position = glm::vec3(0, 0, 0);
	this->target = glm::vec3(0, 0, 0);
	this->up = glm::vec3(0, 1, 0);
}

FPCamera::~FPCamera()
{
}

void FPCamera::Init(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
    this->position = position;
    this->up = up;

    // Calculate initial yaw and pitch from the target
    glm::vec3 dir = glm::normalize(target - position);
    this->pitch = glm::degrees(asin(dir.y));
    this->yaw = glm::degrees(atan2(dir.x, dir.z));  // Changed from atan2(dir.z, dir.x)

    // Recalculate target to ensure consistency
    this->target = position + dir;

    this->isDirty = true;
    Refresh();
}

void FPCamera::Reset()
{
	// Reset to initial orientation (looking along -Z axis)
	yaw = 0.0f;
	pitch = 0.0f;

	glm::vec3 view;
	view.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	view.y = sin(glm::radians(pitch));
	view.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

	target = position + glm::normalize(view);
	up = glm::vec3(0, 1, 0);
	isDirty = true;
}

void FPCamera::Update(double dt)
{
    static const float MOVE_SPEED = 5.0f;
    static const float MOUSE_SENSITIVITY = 0.15f;
    static int initFrames = 0;

    // ===== RESET CAMERA ORIENTATION (press R key) =====
    if (KeyboardController::GetInstance()->IsKeyPressed('R')) {
        yaw = 0.0f;
        pitch = 0.0f;
        isDirty = true;
    }

    // ===== MOUSE LOOK =====
    double deltaX = MouseController::GetInstance()->GetMouseDeltaX();
    double deltaY = MouseController::GetInstance()->GetMouseDeltaY();

    if (initFrames < 2) {
        initFrames++;
        deltaX = 0;
        deltaY = 0;
    }

    // Update yaw (horizontal) - no limits
    if (abs(deltaX) > 0.001) {
        yaw -= deltaX * MOUSE_SENSITIVITY;
        isDirty = true;
    }

    // Update pitch (vertical) - clamped
    if (abs(deltaY) > 0.001) {
        pitch += deltaY * MOUSE_SENSITIVITY;
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
        isDirty = true;
    }

    // Calculate view direction from yaw and pitch
    glm::vec3 direction;
    direction.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

    // Calculate right and forward vectors for movement (keep horizontal)
    glm::vec3 forward;
    forward.x = sin(glm::radians(yaw));
    forward.y = 0;
    forward.z = cos(glm::radians(yaw));
    forward = glm::normalize(forward);

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

    // ===== KEYBOARD MOVEMENT =====
    if (KeyboardController::GetInstance()->IsKeyDown('W') ||
        KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_UP)) {
        position += forward * MOVE_SPEED * static_cast<float>(dt);
        isDirty = true;
    }
    if (KeyboardController::GetInstance()->IsKeyDown('S') ||
        KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_DOWN)) {
        position -= forward * MOVE_SPEED * static_cast<float>(dt);
        isDirty = true;
    }
    if (KeyboardController::GetInstance()->IsKeyDown('A') ||
        KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_LEFT)) {
        position -= right * MOVE_SPEED * static_cast<float>(dt);
        isDirty = true;
    }
    if (KeyboardController::GetInstance()->IsKeyDown('D') ||
        KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_RIGHT)) {
        position += right * MOVE_SPEED * static_cast<float>(dt);
        isDirty = true;
    }

    // Update target based on current position and direction
    target = position + direction;

    this->Refresh();
}

void FPCamera::Refresh()
{
	if (!this->isDirty) return;

	// Always keep up vector pointing straight up to prevent camera roll
	this->up = glm::vec3(0, 1, 0);

	this->isDirty = false;
}

void FPCamera::RotateYaw(float degrees)
{
    yaw += degrees;
    isDirty = true;
}