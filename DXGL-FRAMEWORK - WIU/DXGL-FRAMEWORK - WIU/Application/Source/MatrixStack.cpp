#include "MatrixStack.h"


// Matric inside the stack is an Identity matrix
MatrixStack::MatrixStack() {
	glm::mat4 mat(1.f);
	ms.push(mat);
}


MatrixStack::~MatrixStack() {

}


// Return the top matrix value of the stack
const glm::mat4& MatrixStack::Top() const {
	return ms.top();
}


// Push (make) a copy of the present top matrix, puts on top of stack
void MatrixStack::PushMatrix() {
	ms.push(ms.top());
}


// Pop (remove) the top matrix off the stack
void MatrixStack::PopMatrix() {
	ms.pop();
}


// Clear all matrices except the bottom (identity) matrix
	// if only one matrix, do nothing. 
	// if more than one matrix, remove until only one left
	// stack must always have at least one matrix
void MatrixStack::Clear() {
	while (ms.size() > 1)
		ms.pop();
}



// Load Identity matrix to the top of the stack
void MatrixStack::LoadIdentity() {
	glm::mat4 mat(1.f);
	ms.top() = mat;
}


// Load given matrix to the top of the stack
void MatrixStack::LoadMatrix(const glm::mat4& matrix) {
	ms.top() = matrix;
}


// Top will be multiplied by given matrix
void MatrixStack::MultMatrix(const glm::mat4& matrix) {
	ms.top() = ms.top() * matrix;
}




void MatrixStack::Rotate(float degrees, float axisX, float axisY, float
	axisZ) {
	glm::mat4 mat = glm::rotate(
		glm::mat4(1.f), // matrix to modify
		glm::radians(degrees), // rotation angle in radians
		glm::vec3(axisX, axisY, axisZ) // the axis to rotate along
	);
	ms.top() = ms.top() * mat;
}


void MatrixStack::Translate(float translateX, float translateY, float
	translateZ) {
	glm::mat4 mat = glm::translate(
		glm::mat4(1.f),
		glm::vec3(translateX, translateY, translateZ)
	);
	ms.top() = ms.top() * mat;
}

void MatrixStack::Scale(float scaleX, float scaleY, float scaleZ) {
	glm::mat4 mat = glm::scale(
		glm::mat4(1.f),
		glm::vec3(scaleX, scaleY, scaleZ)
	);
	ms.top() = ms.top() * mat;
}


// Cam View

void MatrixStack::Frustum(double left, double right, double bottom,
	double top, double near, double far) {
	glm::mat4 mat = glm::frustum(left, right, bottom, top, near, far);
	ms.top() = ms.top() * mat;
}


void MatrixStack::LookAt(double eyeX, double eyeY, double eyeZ,
	double centerX, double centerY, double
	centerZ,
	double upX, double upY, double upZ)
{
	glm::mat4 mat = glm::lookAt(
		glm::vec3(eyeX, eyeY, eyeZ),
		glm::vec3(centerX, centerY, centerZ),
		glm::vec3(upX, upY, upZ)
	);
	ms.top() = ms.top() * mat;
}