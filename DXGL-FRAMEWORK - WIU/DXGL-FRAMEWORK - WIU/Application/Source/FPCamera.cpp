#include "FPCamera.h"
#include "KeyboardController.h"
#include "MouseController.h"

//Include GLFW
#include <GLFW/glfw3.h>

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
	this->target = target;
	this->up = up;
	this->isDirty = true;

	Refresh();
}

void FPCamera::Reset()
{
}

void FPCamera::Update(double dt)
{
	static const float ROTATE_SPEED = 15.0f;
	static const float MOVE_SPEED = 10.0f;
	static const float ZOOM_SPEED = 10.0f;
	static const float MAX_PITCH = 89.0f; // Prevent camera flip

	glm::vec3 view = glm::normalize(target - position); //calculate the new view vector
	glm::vec3 right = glm::normalize(glm::cross(view, up));
	glm::vec3 worldUp = glm::vec3(0, 1, 0); // Fixed world up
	glm::vec3 forward = glm::normalize(glm::vec3(view.x, 0, view.z));

	// Store current pitch angle to clamp it
	static float currentPitch = 0.0f;

	if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
	{
		//mouse input
		double deltaX = MouseController::GetInstance()->GetMouseDeltaX();
		double deltaY = MouseController::GetInstance()->GetMouseDeltaY();

		//yaw input
		float yawAngle = -deltaX * ROTATE_SPEED * 0.5 * static_cast<float>(dt);
		glm::mat4 yaw = glm::rotate(
			glm::mat4(1.f),// matrix to modify
			glm::radians(yawAngle),// rotation angle in degree and converted to radians
			worldUp
		);

		view = glm::vec3(yaw * glm::vec4(view, 0.f));


		//pitch	
		float pitchAngle = deltaY * ROTATE_SPEED * 0.5f * static_cast<float>(dt);

		// Update and clamp pitch
		currentPitch += pitchAngle;
		currentPitch = glm::clamp(currentPitch, -MAX_PITCH, MAX_PITCH);

		right = glm::normalize(glm::cross(view, worldUp));


		glm::mat4 pitch = glm::rotate(
			glm::mat4(1.f),
			glm::radians(pitchAngle),
			right
		);

		glm::vec3 newView = glm::vec3(pitch * glm::vec4(view, 0.f));

		// Only apply pitch if it doesn't exceed limits
		float newPitchCheck = glm::degrees(asin(glm::clamp(newView.y, -1.0f, 1.0f)));
		if (abs(newPitchCheck) < MAX_PITCH)
		{
			view = newView;
		}

		target = position + view;
		isDirty = true;
	}

	//mouse scroll input for zooming
	double scrollDelta = MouseController::GetInstance()->GetMouseScrollStatus(MouseController::SCROLL_TYPE_YOFFSET);
	if (scrollDelta != 0.0)
	{
		// Move camera forward/backward along view direction
		position += view * static_cast<float>(scrollDelta) * ZOOM_SPEED * static_cast<float>(dt) * 10.0f;
		target = position + view;
		isDirty = true;
	}


	float moveAmount = MOVE_SPEED * static_cast<float>(dt);

	// Recalculate right and forward for movement
	right = glm::normalize(glm::cross(view, worldUp));
	forward = glm::normalize(glm::vec3(view.x, 0, view.z)); // Keep movement on horizontal plane


	// W - Move Forward
	if (KeyboardController::GetInstance()->IsKeyDown('W'))
	{
		position += forward * moveAmount;
		target = position + view;
		isDirty = true;
	}

	// S - Move Backward
	if (KeyboardController::GetInstance()->IsKeyDown('S'))
	{
		position -= forward * moveAmount;
		target = position + view;
		isDirty = true;
	}

	// A - Move Left
	if (KeyboardController::GetInstance()->IsKeyDown('A'))
	{
		position -= right * moveAmount;
		target = position + view;
		isDirty = true;
	}

	// D - Move Right
	if (KeyboardController::GetInstance()->IsKeyDown('D'))
	{
		position += right * moveAmount;
		target = position + view;
		isDirty = true;
	}

	// E - Move Up
	if (KeyboardController::GetInstance()->IsKeyDown('E'))
	{
		position += worldUp * moveAmount;
		target = position + view;
		isDirty = true;
	}

	// Q - Move Down
	if (KeyboardController::GetInstance()->IsKeyDown('Q'))
	{
		position -= worldUp * moveAmount;
		target = position + view;
		isDirty = true;
	}

	previousPosition = position;

	this->Refresh();
}

void FPCamera::Refresh()
{
	if (!this->isDirty) return;

	glm::vec3 view = glm::normalize(target - position);
	glm::vec3 worldUp = glm::vec3(0, 1, 0);
	glm::vec3 right = glm::normalize(glm::cross(worldUp, view));

	// Recalculate the up vector
	this->up = glm::normalize(glm::cross(view, right));

	this->isDirty = false;
}