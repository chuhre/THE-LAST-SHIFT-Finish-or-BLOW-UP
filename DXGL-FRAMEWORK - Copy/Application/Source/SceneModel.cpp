#include "SceneModel.h"
#include "GL\glew.h"

// GLM Headers
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include <KeyboardController.h>
#include <GLFW/glfw3.h>
#include <glm/glm/gtc/matrix_inverse.hpp>

SceneModel::SceneModel()
{
}

SceneModel::~SceneModel()
{
}

void SceneModel::Init()
{
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Load the shader programs
	m_programID = LoadShaders("Shader//Shading.vertexshader",
								"Shader//Shading.fragmentshader");
	glUseProgram(m_programID);

	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");

	// Add the following codes
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");

	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_BODY_CUBE] = MeshBuilder::GenerateCube("Body", glm::vec3(0.5f, 0.3f, 0.2f), 1.f);
	meshList[GEO_PELVIS_CUBOID] = MeshBuilder::GenerateCuboid("Pelvis", glm::vec3(0.2f, 0.5f, 0.3f), 2.f, 1.f, 3.f);
	meshList[GEO_SHOULDER_CUBOID] = MeshBuilder::GenerateCuboid("Shoulder", glm::vec3(0.2f, 0.5f, 0.3f), 2.f);
	meshList[GEO_UPPER_ARM_CYLINDER] = MeshBuilder::GenerateCylinder("UpperArm", glm::vec3(0.3f, 0.2f, 0.5f), 16, 1.f, 2.f);
	meshList[GEO_LOWER_ARM_CYLINDER] = MeshBuilder::GenerateCylinder("LowerArm", glm::vec3(0.3f, 0.2f, 0.5f), 16, 1.f, 2.f);
	meshList[GEO_HAND_CUBE] = MeshBuilder::GenerateCube("Hand", glm::vec3(0.5f, 0.3f, 0.2f), 1.f);
	meshList[GEO_UPPER_LEG_CYLINDER] = MeshBuilder::GenerateCylinder("UpperLeg", glm::vec3(0.3f, 0.2f, 0.5f), 16, 1.f, 2.f);
	meshList[GEO_LOWER_LEG_CYLINDER] = MeshBuilder::GenerateCylinder("LowerLeg", glm::vec3(0.3f, 0.2f, 0.5f), 16, 1.f, 2.f);
	meshList[GEO_FOOT_CUBOID] = MeshBuilder::GenerateCuboid("Foot", glm::vec3(0.5f, 0.3f, 0.2f), 1.f, 0.5, 2.f);
	meshList[GEO_JOINT_SPHERE] = MeshBuilder::GenerateSphere("Joint", glm::vec3(1.f, 1.f, 1.f), 1.f, 16, 16);
	meshList[GEO_NECK_HEMISPHERE] = MeshBuilder::GenerateHemisphere("Neck", glm::vec3(0.5f, 0.3f, 0.2f), 16, 16, 1.f);
	meshList[GEO_HEAD_SPHERE] = MeshBuilder::GenerateSphere("Head", glm::vec3(0.5f, 0.3f, 0.2f), 1.f, 16, 16);
	meshList[GEO_EYE_SPHERE] = MeshBuilder::GenerateSphere("Eye", glm::vec3(0.f, 0.f, 0.f), 0.2f, 16, 16);
	meshList[GEO_HAT_CONE] = MeshBuilder::GenerateCone("Hat", glm::vec3(1.f, 0.f, 0.f), 16, 32, 1.f, 2.f);

	meshList[GEO_LIGHT_SPHERE] = MeshBuilder::GenerateSphere("Light", glm::vec3(1.f, 1.f,	1.f), 1.f, 16, 16);

	// Initialise camera properties
	camera.Init(45.f, 45.f, 10.f);

	// Set projection matrix to perspective mode
	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);


	light[0].position = glm::vec3(0, 20, 0);
	light[0].color = glm::vec3(1, 1, 1);
	light[0].power = 10;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, glm::value_ptr(light[0].color));
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);

	waveRotation = 0.f;
	waveSpeed = 120.f; // degrees per second
	waveDirection = true;

	jumpHeight = 0.f;
	jumpSpeed = 5.f;

	headRotationY = 0.f;
	headRotationSpeed = 45.f; // degrees per second
	headRotationDirection = true;
	bodyBendZ = 0.f;
	armSwingRotation = 0.f;
	kneeBend = 0.f;
	pelvisShiftX = 0.f;

	isSpinAttacking = false;
	spinTimer = 0.0f;
	spinDuration = 0.8f;      // full spin takes 0.8 seconds
	spinRotation = 0.0f;
	spinSpeed = 360.0f;       // degrees per second during spin

	somersaultRotation = 0.f;
	somersaultSpeed = 360.f;  // degrees per second
	somersaultTimer = 0.0f;
	somersaultDuration = 1.0f;  // complete somersault in 1 second
	somersaultPrepDuration = 0.3f; // preparation time before somersault
	isSomersaulting = false;

	isTransitioning = false;
	targetAnim = ANIM_RESET;
	transitionTimer = 0.0f;
	transitionDuration = 0.3f;  // 0.3 seconds to blend to neutral

    currAnim = ANIM_RESET;
}

void SceneModel::RenderMesh(Mesh* mesh, bool enableLight)
{
	glm::mat4 MVP, modelView, modelView_inverse_transpose;
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, glm::value_ptr(modelView));
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = glm::inverseTranspose(modelView);
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE,
			glm::value_ptr(modelView_inverse_transpose));
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, glm::value_ptr(mesh -> material.kAmbient));
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, glm::value_ptr(mesh -> material.kDiffuse));
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, glm::value_ptr(mesh -> material.kSpecular));
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	mesh->Render();
}

void SceneModel::Update(double dt)
{
	// Check for key press, you can add more for interaction
	HandleKeyPress();

	camera.Update(dt);

	if (isTransitioning) {
		transitionTimer += static_cast<float>(dt);

		// Blend everything back to zero
		float t = transitionTimer / transitionDuration;
		waveRotation = glm::mix(waveRotation, 0.f, t);
		headRotationY = glm::mix(headRotationY, 0.f, t);
		bodyBendZ = glm::mix(bodyBendZ, 0.f, t);
		armSwingRotation = glm::mix(armSwingRotation, 0.f, t);
		kneeBend = glm::mix(kneeBend, 0.f, t);
		pelvisShiftX = glm::mix(pelvisShiftX, 0.f, t);
		jumpHeight = glm::mix(jumpHeight, 0.f, t);
		spinRotation = glm::mix(spinRotation, 0.f, t);
		somersaultRotation = glm::mix(somersaultRotation, 0.f, t);

		// Check if transition is complete
		if (transitionTimer >= transitionDuration) {
			isTransitioning = false;
			transitionTimer = 0.0f;

			// Reset everything to zero
			waveRotation = 0.f;
			headRotationY = 0.f;
			bodyBendZ = 0.f;
			armSwingRotation = 0.f;
			kneeBend = 0.f;
			pelvisShiftX = 0.f;
			jumpHeight = 0.f;
			spinRotation = 0.f;
			somersaultRotation = 0.f;

			// Now switch to target animation
			currAnim = targetAnim;

			// Initialize animation-specific variables
			if (currAnim == ANIM_ATTACK) {
				isSpinAttacking = true;
				spinTimer = 0.0f;
			}
			else if (currAnim == ANIM_SOMERSAULT) {
				isSomersaulting = true;
				somersaultTimer = 0.0f;
			}
		}

		return;  // Don't run animation logic during transition
	}

	if (currAnim == ANIM_IDLE) { // idle animation
		if (headRotationDirection) {

			headRotationY += static_cast<float>(dt) * headRotationSpeed;

			if (headRotationY >= 45.f) { // look right 45 degrees
				headRotationDirection = false;
			}
		}
		else {
			headRotationY -= static_cast<float>(dt) * headRotationSpeed;

			if (headRotationY <= -45.f) { // look left 45 degrees
				headRotationDirection = true;
			}
		}

		bodyBendZ = 5.f * sin(glm::radians(headRotationY * 2.f)); // body leans slightly as head turns
		
		pelvisShiftX = -sin(headRotationY * 3.14159f / 180.f) * 0.3f;

		armSwingRotation = 5.f * sin(glm::radians(headRotationY * 4.f)); // arms swing slightly

		kneeBend = 2.f * cos(glm::radians(headRotationY * 4.f)); // knees bend slightly
	}
	else if (currAnim == ANIM_WAVE) { // Wave animation logic

		// update wave rotation
		if (waveDirection) {

			waveRotation += static_cast<float>(dt) * waveSpeed;

			if (waveRotation >= 30.f) { // max wave angle
				waveDirection = false;
			}
		}
		else {
			waveRotation -= static_cast<float>(dt) * waveSpeed;
			if (waveRotation <= -45.f) {// min wave angle
				waveDirection = true;
			}
		}

		// update jump height
		jumpHeight += jumpSpeed * static_cast<float>(dt);

		if (jumpHeight >= 5.f) {
			jumpSpeed = -5.f;
		}
		else if (jumpHeight <= 0.f) {
			jumpSpeed = 5.f;
			jumpHeight = 0.f; // reset to ground level
		}

		// bending body while jumping
		if (jumpSpeed > 0.f) {
			// bending knees while going up
			kneeBend = glm::mix(25.f, 5.f, jumpHeight / 5.f);
		}
		else {
			// bending knees while going down
			kneeBend = glm::mix(5.f, 25.f, (5.f - jumpHeight) / 5.f);
		}
	}
	else if (currAnim == ANIM_ATTACK) {
		if (isSpinAttacking) {

			spinTimer += static_cast<float>(dt);

			if (spinTimer >= spinDuration) {

				//isSpinAttacking = false;
				spinTimer = 0.f;
			}
			else {
				// Continue spinning
				float progress = spinTimer / spinDuration;
				spinRotation = progress * 360.0f * 1.5f;
			}
		}
	}
	else if (currAnim == ANIM_SOMERSAULT) {

		somersaultTimer += static_cast<float>(dt);

		// Phase 1: Preparation (crouch down)
		if (somersaultTimer < somersaultPrepDuration) {

			float prepProgress = somersaultTimer / somersaultPrepDuration;

			// crouch down
			jumpHeight = -prepProgress * 0.5f;  // lower body slightly
			kneeBend = prepProgress * 60.0f;    // bend knees deeply
			bodyBendZ = prepProgress * 15.0f;   // lean forward

			// arms prepare
			armSwingRotation = -prepProgress * 30.0f;  // pull arms back

			somersaultRotation = 0.0f;
		}
		// Phase 2: Jump and somersault
		else {

			float flipTime = somersaultTimer - somersaultPrepDuration;
			float flipDuration = somersaultDuration - somersaultPrepDuration;

			if (flipTime >= flipDuration) {
				// loop back to preparation
				somersaultTimer = 0.0f;
				somersaultRotation = 0.0f;
			}
			else {

				float flipProgress = flipTime / flipDuration;

				// rotate for somersault
				somersaultRotation = -flipProgress * 360.0f;

				// jump arc (higher and more dramatic)
				jumpHeight = sin(flipProgress * 3.14159f) * 4.5f;

				// tuck body tightly during flip
				kneeBend = 70.0f;
				bodyBendZ = 25.0f;

				// arms tucked in
				armSwingRotation = -20.0f;
			}
		}
	}
	else {
		// Smoothly interpolate back to neutral pose
		waveRotation = glm::mix(waveRotation, 0.f, static_cast<float>(dt) * 5.f);
		headRotationY = glm::mix(headRotationY, 0.f, static_cast<float>(dt) * 5.f);
		bodyBendZ = glm::mix(bodyBendZ, 0.f, static_cast<float>(dt) * 5.f);
		armSwingRotation = glm::mix(armSwingRotation, 0.f, static_cast<float>(dt) * 5.f);
		kneeBend = glm::mix(kneeBend, 0.f, static_cast<float>(dt) * 5.f);
		pelvisShiftX = glm::mix(pelvisShiftX, 0.f, static_cast<float>(dt) * 5.f);
		jumpHeight = glm::mix(jumpHeight, 0.f, static_cast<float>(dt) * 5.f);
		spinRotation = glm::mix(spinRotation, 0.f, static_cast<float>(dt) * 5.f);
		somersaultRotation = glm::mix(somersaultRotation, 0.f, static_cast<float>(dt) * 5.f);
		isSpinAttacking = false;  // stops attack
		isSomersaulting = false;
	}

	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		light[0].position.z -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		light[0].position.z += static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		light[0].position.x -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		light[0].position.x += static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		light[0].position.y -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		light[0].position.y += static_cast<float>(dt) * 5.f;


}

void SceneModel::Render()
{
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Setup Model View Projection matrix
	// Define the model matrix and set it to identity
	glm::mat4 model = glm::mat4(1.0f);

	//// Define the view matrix and set it with camera position, target position and up direction
	//glm::mat4 view = glm::lookAt(
	//	glm::vec3(camera.position.x, camera.position.y,	camera.position.z),
	//	glm::vec3(camera.target.x, camera.target.y, camera.target.y),
	//	glm::vec3(0.f, 1.f, 0.f)
	//);


	//// Define the projection matrix either in perspective ...
	//glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	
	// or orthographic ...
	//glm::mat4 projection = glm::ortho(-10.f, 10.f, -10.f, 10.f, 0.1f, 1000.f);

	// Load view matrix stack and set it with camera position,
	//target position and up direction
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);

	// Calculate the light position in camera space
	glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));

	// Load identity matrix into the model stack
	modelStack.LoadIdentity();

	// Calculate the Model-View-Projection matrix
	glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
	modelStack.PushMatrix();

	// Render objects
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

    // Render light
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	modelStack.Scale(0.1f, 0.1f, 0.1f);
	RenderMesh(meshList[GEO_LIGHT_SPHERE], false);
	modelStack.PopMatrix();

	// add materials and before rendering objects


	meshList[GEO_PELVIS_CUBOID]->material.kAmbient = glm::vec3(0.3f, 0.5f, 0.2f);
	meshList[GEO_PELVIS_CUBOID]->material.kDiffuse = glm::vec3(1.0f, 0.5f, 0.3f);
	meshList[GEO_PELVIS_CUBOID]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_PELVIS_CUBOID]->material.kShininess = 100.f;

	meshList[GEO_BODY_CUBE]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.9f);
	meshList[GEO_BODY_CUBE]->material.kDiffuse = glm::vec3(1.0f, 0.5f, 0.3f);
	meshList[GEO_BODY_CUBE]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_BODY_CUBE]->material.kShininess = 100.f;

	meshList[GEO_SHOULDER_CUBOID]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.5f);
	meshList[GEO_SHOULDER_CUBOID]->material.kDiffuse = glm::vec3(0.6f, 0.4f, 1.0f);
	meshList[GEO_SHOULDER_CUBOID]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SHOULDER_CUBOID]->material.kShininess = 50.f;

	meshList[GEO_UPPER_ARM_CYLINDER]->material.kAmbient = glm::vec3(0.2f, 0.1f, 0.3f);
	meshList[GEO_UPPER_ARM_CYLINDER]->material.kDiffuse = glm::vec3(0.6f, 0.4f, 1.0f);
	meshList[GEO_UPPER_ARM_CYLINDER]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_UPPER_ARM_CYLINDER]->material.kShininess = 50.f;

	meshList[GEO_LOWER_ARM_CYLINDER]->material.kAmbient = glm::vec3(0.2f, 0.1f, 0.3f);
	meshList[GEO_LOWER_ARM_CYLINDER]->material.kDiffuse = glm::vec3(0.6f, 0.4f, 1.0f);
	meshList[GEO_LOWER_ARM_CYLINDER]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_LOWER_ARM_CYLINDER]->material.kShininess = 50.f;

	meshList[GEO_HAND_CUBE]->material.kAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
	meshList[GEO_HAND_CUBE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_HAND_CUBE]->material.kSpecular = glm::vec3(1.0, 1.0f, 1.0f);
	meshList[GEO_HAND_CUBE]->material.kShininess = 50.f;

	meshList[GEO_JOINT_SPHERE]->material.kAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
	meshList[GEO_JOINT_SPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_JOINT_SPHERE]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_JOINT_SPHERE]->material.kShininess = 50.f;

	meshList[GEO_UPPER_LEG_CYLINDER]->material.kAmbient = glm::vec3(0.2f, 0.3f, 0.1f);
	meshList[GEO_UPPER_LEG_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_UPPER_LEG_CYLINDER]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_UPPER_LEG_CYLINDER]->material.kShininess = 10.f;

	meshList[GEO_LOWER_LEG_CYLINDER]->material.kAmbient = glm::vec3(0.2f, 0.3f, 0.1f);
	meshList[GEO_LOWER_LEG_CYLINDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_LOWER_LEG_CYLINDER]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_LOWER_LEG_CYLINDER]->material.kShininess = 10.f;

	meshList[GEO_FOOT_CUBOID]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_FOOT_CUBOID]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_FOOT_CUBOID]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FOOT_CUBOID]->material.kShininess = 10.f;

	meshList[GEO_NECK_HEMISPHERE]->material.kAmbient = glm::vec3(0.5f, 0.3f, 0.2f);
	meshList[GEO_NECK_HEMISPHERE]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.3f);
	meshList[GEO_NECK_HEMISPHERE]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_NECK_HEMISPHERE]->material.kShininess = 20.f;

	meshList[GEO_HEAD_SPHERE]->material.kAmbient = glm::vec3(0.5f, 0.3f, 0.2f);
	meshList[GEO_HEAD_SPHERE]->material.kDiffuse = glm::vec3(1.0f, 0.5f, 0.3f);
	meshList[GEO_HEAD_SPHERE]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_HEAD_SPHERE]->material.kShininess = 30.f;

	meshList[GEO_EYE_SPHERE]->material.kAmbient = glm::vec3(0.f, 0.f, 0.f);
	meshList[GEO_EYE_SPHERE]->material.kDiffuse = glm::vec3(0.f, 0.f, 0.f);
	meshList[GEO_EYE_SPHERE]->material.kSpecular = glm::vec3(0.f, 0.f, 0.f);
	meshList[GEO_EYE_SPHERE]->material.kShininess = 1.f;

	meshList[GEO_HAT_CONE]->material.kAmbient = glm::vec3(0.3f, 0.f, 0.f);
	meshList[GEO_HAT_CONE]->material.kDiffuse = glm::vec3(1.0f, 0.5f, 0.f);
	meshList[GEO_HAT_CONE]->material.kSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_HAT_CONE]->material.kShininess = 30.f;

	// render pelvis cuboid
	modelStack.PushMatrix();
	modelStack.Translate(pelvisShiftX, -1.0f + jumpHeight, 0.f);
	if (currAnim == ANIM_ATTACK) {
		modelStack.Rotate(spinRotation, 0.f, 1.f, 0.f);
	}
	else if (currAnim == ANIM_SOMERSAULT) {
		modelStack.Rotate(somersaultRotation, 1.f, 0.f, 0.f);
	}
	modelStack.PushMatrix();
	modelStack.Scale(1.5f, 0.4f, 1.5f);
	RenderMesh(meshList[GEO_PELVIS_CUBOID], true);
	modelStack.PopMatrix(); // end pelvis

	// render body cube
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 1.9f, 0.f);
	if (currAnim == ANIM_IDLE) {
		modelStack.Rotate(bodyBendZ * 0.5f, 0.f, 0.f, 1.f); // body leaning forward/back
	}
	else if (currAnim == ANIM_SOMERSAULT) {
		modelStack.Rotate(bodyBendZ, 1.f, 0.f, 0.f); // body leaning forward/back
	}
	modelStack.PushMatrix(); // end translate up
	modelStack.Scale(3.5f, 3.5f, 3.0f);
	RenderMesh(meshList[GEO_BODY_CUBE], true);
	modelStack.PopMatrix(); // isolate

	////// head and neck //////
	// render neck hemisphere
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 1.75f, 0.f);
	modelStack.Scale(0.3f, 0.3f, 0.3f);
	RenderMesh(meshList[GEO_NECK_HEMISPHERE], true);
	
	// render head sphere
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 4.5f, 0.f);
	modelStack.Rotate(headRotationY, 0.f, 1.f, 0.f); // head side to side
	modelStack.Scale(4.0f, 4.0f, 4.0f);
	RenderMesh(meshList[GEO_HEAD_SPHERE], true);

	// render left eye sphere
	modelStack.PushMatrix();
	modelStack.Translate(0.5f, 0.0f, 1.0f);
	modelStack.Scale(1.0f, 1.0f, 1.0f);
	RenderMesh(meshList[GEO_EYE_SPHERE], true);
	modelStack.PopMatrix(); // end left eye
	
	// render right eye sphere
	modelStack.PushMatrix();
	modelStack.Translate(-0.5f, 0.0f, 1.0f);
	modelStack.Scale(1.0f, 1.0f, 1.0f);
	RenderMesh(meshList[GEO_EYE_SPHERE], true);
	modelStack.PopMatrix(); // end right eye

	// render hat cone
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.5f, 0.f);
	modelStack.Scale(1.0f, 1.0f, 1.0f);
	RenderMesh(meshList[GEO_HAT_CONE], true);
	modelStack.PopMatrix(); // end hat

	modelStack.PopMatrix(); // end head

	modelStack.PopMatrix(); // end neck
	////// end head and neck //////

	////// left arm //////
	// render left shoulder thingy cuboid
	modelStack.PushMatrix();
	modelStack.Translate(2.2f, 1.0f, 0.f);
	modelStack.Scale(1.0f, 1.0f, 1.0f);
	RenderMesh(meshList[GEO_SHOULDER_CUBOID], true);

	// render left shoulder joint sphere
	modelStack.PushMatrix();
	modelStack.Translate(0.9f, 0.f, 0.f);
	if (currAnim == ANIM_WAVE) {
		modelStack.Rotate(80.f, 0.f, 0.f, 1.f);
	}
	else if (currAnim == ANIM_IDLE) {
		modelStack.Rotate(-armSwingRotation, 0.f, 0.f, 1.f); // arm swings slightly
	}
	else if (currAnim == ANIM_ATTACK) {
		modelStack.Rotate(90.f, 0.f, 0.f, 1.f); // counter spin for arm
	}
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix();

	// render left cylinder upper arm
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.0f, 0.f);

	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 1.0f, 0.5f);
	RenderMesh(meshList[GEO_UPPER_ARM_CYLINDER], true);
	modelStack.PopMatrix(); // isolate scaling

	// render left elbow joint
	modelStack.PushMatrix();
	modelStack.Translate(0.0f, -1.2f, 0.f);
	// add wave rotation here
	modelStack.Rotate(-waveRotation, 0.f, 0.f, 1.f);

	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling
	
	// render left cylinder lower arm
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.0f, 0.f);
	modelStack.Scale(0.5f, 1.0f, 0.5f);
	RenderMesh(meshList[GEO_LOWER_ARM_CYLINDER], true);

	// render left hand cube
	modelStack.PushMatrix();
	modelStack.Translate(-0.02f, -1.5f, 0.f);
	modelStack.Scale(2.0f, 1.0f, 2.0f);
	RenderMesh(meshList[GEO_HAND_CUBE], true);
	modelStack.PopMatrix(); // end left hand

	modelStack.PopMatrix(); // end lower arm

    modelStack.PopMatrix(); // end elbow joint

	modelStack.PopMatrix(); // end cylinder arm

	modelStack.PopMatrix(); // end joint sphere
	
	modelStack.PopMatrix(); // end left shoulder thingy
	////// end left arm //////

	////// right arm //////
	// right shoulder thingy cuboid
	modelStack.PushMatrix();
	modelStack.Translate(-2.2f, 1.0f, 0.f);
	modelStack.Scale(1.0f, 1.0f, 1.0f);
	RenderMesh(meshList[GEO_SHOULDER_CUBOID], true);

	// right shoulder joint sphere
	modelStack.PushMatrix();
	modelStack.Translate(-0.9f, 0.f, 0.f);
	if (currAnim == ANIM_WAVE) {
		modelStack.Rotate(-80.f, 0.f, 0.f, 1.f);
	}
	else if (currAnim == ANIM_IDLE) {
		modelStack.Rotate(armSwingRotation, 0.f, 0.f, 1.f); // arm swings slightly
	}
	else if (currAnim == ANIM_ATTACK) {
		modelStack.Rotate(-90.f, 0.f, 0.f, 1.f); // counter spin for arm
	}
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling

	// right cylinder upper arm
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.0f, 0.f);

	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 1.0f, 0.5f);
	RenderMesh(meshList[GEO_UPPER_ARM_CYLINDER], true);
	modelStack.PopMatrix(); // isolate scaling
	
	// right elbow joint
	modelStack.PushMatrix();
	modelStack.Translate(0.0f, -1.2f, 0.f);
    // add wave rotation here
	modelStack.Rotate(waveRotation, 0.f, 0.f, 1.f); // wave hand back and forth

	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling

	// right cylinder lower arm
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.0f, 0.f);
	//modelStack.Rotate(5.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 1.0f, 0.5f);
	RenderMesh(meshList[GEO_LOWER_ARM_CYLINDER], true);

	// right hand cube
	modelStack.PushMatrix();
	modelStack.Translate(0.02f, -1.5f, 0.f);
	modelStack.Scale(2.0f, 1.0f, 2.0f);
	RenderMesh(meshList[GEO_HAND_CUBE], true);
	modelStack.PopMatrix(); // right hand end

	modelStack.PopMatrix(); // right lower arm end

	modelStack.PopMatrix(); // right elbow joint end

	modelStack.PopMatrix(); // right cylinder arm end

	modelStack.PopMatrix();  // right shoulder joint end	
	
	modelStack.PopMatrix(); // end right shoulder thingy
	////// end right arm //////

	modelStack.PopMatrix(); // end body cube

	////// left leg //////
	// left leg joint sphere
	modelStack.PushMatrix();
	modelStack.Translate(1.0f, -0.5f, 0.0f);
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling

	// left upper leg cylinder
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.0f, 0.f);
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 1.0f, 0.5f);
	RenderMesh(meshList[GEO_UPPER_LEG_CYLINDER], true);
	modelStack.PopMatrix(); // isolate scaling

	// left knee joint sphere
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.1f, 0.f);
	if (currAnim == ANIM_IDLE) {
		modelStack.Rotate(-kneeBend, 1.f, 0.f, 0.f); // knees bend slightly
	}
	else if (currAnim == ANIM_WAVE) {
		modelStack.Rotate(-kneeBend, 1.f, 0.f, 0.f); // knees bend while waving
	}
	else if (currAnim == ANIM_SOMERSAULT) {
		modelStack.Rotate(kneeBend, 1.f, 0.f, 0.f); // knees bend while somersaulting
	}
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling

	// left lower leg cylinder
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.6f, 0.f);
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 1.5f, 0.5f);
	RenderMesh(meshList[GEO_LOWER_LEG_CYLINDER], true);
	modelStack.PopMatrix(); // isolate scaling

	// left foot cuboid
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.1f, 0.f);
	modelStack.Scale(0.5f, 1.0f, 1.5f);
	RenderMesh(meshList[GEO_FOOT_CUBOID], true);
	modelStack.PopMatrix(); // end left foot cuboid

	modelStack.PopMatrix(); // end left lower leg

	modelStack.PopMatrix(); // end left knee joint sphere

	modelStack.PopMatrix(); // end left upper leg

	modelStack.PopMatrix(); // end left leg joint sphere
	////// end left leg //////

	////// right leg //////
	// right leg joint sphere
	modelStack.PushMatrix();
	modelStack.Translate(-1.0f, -0.5f, 0.0f);
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling
	
	// right upper leg cylinder
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.0f, 0.f);
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 1.0f, 0.5f);
	RenderMesh(meshList[GEO_UPPER_LEG_CYLINDER], true);
	modelStack.PopMatrix(); // isolate scaling

	// right knee joint sphere
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.1f, 0.f);
	if (currAnim == ANIM_IDLE) {
		modelStack.Rotate(kneeBend, 1.f, 0.f, 0.f); // knees bend slightly
	}
	else if (currAnim == ANIM_WAVE) {
		modelStack.Rotate(kneeBend, 1.f, 0.f, 0.f); // knees bend while waving
	}
	else if (currAnim == ANIM_SOMERSAULT) {
		modelStack.Rotate(kneeBend, 1.f, 0.f, 0.f); // knees bend while somersaulting
	}
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_JOINT_SPHERE], true);
	modelStack.PopMatrix(); // isolate scaling
	
	// right lower leg cylinder
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.6f, 0.f);
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 1.5f, 0.5f);
	RenderMesh(meshList[GEO_LOWER_LEG_CYLINDER], true);
	modelStack.PopMatrix(); // isolate scaling

	// right foot cuboid
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.1f, 0.f);
	modelStack.Scale(0.5f, 1.0f, 1.5f);
	RenderMesh(meshList[GEO_FOOT_CUBOID], true);
	modelStack.PopMatrix(); // end right foot cuboid

	modelStack.PopMatrix(); // end right lower leg

	modelStack.PopMatrix(); // end right knee joint sphere

	modelStack.PopMatrix(); // end upper right leg
	
	modelStack.PopMatrix(); // end right leg joint sphere
	////// end right leg //////

	
	modelStack.PopMatrix(); // end pelvis cuboid

}

void SceneModel::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneModel::HandleKeyPress() 
{
	if (Application::IsKeyPressed(0x31))
	{
		// Key press to enable culling
		glEnable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(0x32))
	{
		// Key press to disable culling
		glDisable(GL_CULL_FACE);
	}
	if (Application::IsKeyPressed(0x33))
	{
		// Key press to enable fill mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (Application::IsKeyPressed(0x34))
	{
		// Key press to enable wireframe mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('Z'))
	{
		if (currAnim != ANIM_IDLE) {
			targetAnim = ANIM_IDLE;
			isTransitioning = true;
			transitionTimer = 0.0f;
		}
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('X'))
	{
		if (currAnim != ANIM_WAVE) {
			targetAnim = ANIM_WAVE;
			isTransitioning = true;
			transitionTimer = 0.0f;
		}
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('C'))
	{
		if (currAnim != ANIM_ATTACK) {
			targetAnim = ANIM_ATTACK;
			isTransitioning = true;
			transitionTimer = 0.0f;
		}
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('V'))
	{
		if (currAnim != ANIM_SOMERSAULT) {
			targetAnim = ANIM_SOMERSAULT;
			isTransitioning = true;
			transitionTimer = 0.0f;
		}
	}
	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		if (currAnim != ANIM_RESET) {
			targetAnim = ANIM_RESET;
			isTransitioning = true;
			transitionTimer = 0.0f;
		}
	}

}
