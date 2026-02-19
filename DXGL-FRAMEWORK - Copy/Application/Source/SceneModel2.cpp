#include "SceneModel2.h"
#include "GL\glew.h"

// GLM Headers
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <glm/glm/gtc/matrix_inverse.hpp>

//Include GLFW
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "Application.h"
#include "Light.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "LoadTGA.h"
#include <iostream>

SceneModel2::SceneModel2()
{
}

SceneModel2::~SceneModel2()
{
}

void SceneModel2::Init()
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
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	glUseProgram(m_programID);

	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");

	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	//m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	//m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	//m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	//m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	//m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	//m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	//m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	//m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	//m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	//m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	//m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	// Initialise camera properties
	camera.Init(glm::vec3(0.f, 2.f, 0.f), glm::vec3(8.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	//camera.Init(45.f, 45.f, 10.f);

	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);


	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sun", glm::vec3(1.f, 1.f, 1.f), 1.f, 16, 16);
	
	//meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
	//meshList[GEO_PLANE]->textureID = LoadTGA("Images//color.tga");

	// Step 5 - Load the models
	// Without texture
	meshList[GEO_MODEL_MTL1] = MeshBuilder::GenerateOBJMTL("model2", "Models//Cursaed_Music_Box.obj", "Models//Cursaed_Music_Box.mtl");
	meshList[GEO_MODEL_MTL1]->textureID = LoadTGA("Images//cursed_music_box_basecolor.tga");
	meshList[GEO_MODEL_OBJ1] = MeshBuilder::GenerateOBJ("model3", "Models//iron_cabinet.obj");
	meshList[GEO_MODEL_OBJ1]->textureID = LoadTGA("Images//iron_cabinet.tga");
	meshList[GEO_MODEL_OBJ2] = MeshBuilder::GenerateOBJ("model4", "Models//tree.obj");
	meshList[GEO_MODEL_OBJ2]->textureID = LoadTGA("Images//fafa2.tga");
	meshList[GEO_MODEL_OBJ3] = MeshBuilder::GenerateOBJ("model5", "Models//uploads_files_234161_Table_OBJ.obj");
	meshList[GEO_MODEL_OBJ3]->textureID = LoadTGA("Images//Table_02_Diffuse.tga");
	meshList[GEO_MODEL_OBJ4] = MeshBuilder::GenerateOBJ("model6", "Models//wooden_doorOBJ.obj");
	meshList[GEO_MODEL_OBJ4]->textureID = LoadTGA("Images//table.tga");
	meshList[GEO_MODEL_OBJ5] = MeshBuilder::GenerateOBJMTL("model7", "Models//pumpkin.obj", "Models//pumpkin.mtl");
	meshList[GEO_MODEL_OBJ5]->textureID = LoadTGA("Images//pumpkin_Base_Color.tga");
	meshList[GEO_MODEL_OBJ6] = MeshBuilder::GenerateOBJ("model8", "Models//WOLF_OBJ.obj");
	meshList[GEO_MODEL_OBJ6]->textureID = LoadTGA("Images//wolf_Body_BaseColor.tga");

	// GUI
	meshList[GEO_GUI] = MeshBuilder::GenerateQuad("GUI", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_GUI]->textureID = LoadTGA("Images//color.tga");

	// text
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

	// skybox
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Images//posy.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Images//negy.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Images//posx.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Images//negx.tga");
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Images//posz.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Images//negz.tga");

	// With texture
	//meshList[GEO_MODEL_MTL2] = MeshBuilder::GenerateOBJMTL("model3", "Models//cottage_obj.obj", "Models//cottage_obj.mtl");
	//meshList[GEO_MODEL_MTL2]->textureID = LoadTGA("Images//cottage_diffuse.tga");

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);


	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	light[0].position = glm::vec3(0, 3, 0);
	light[0].color = glm::vec3(0.4f, 0.5f, 0.6f);
	light[0].type = Light::LIGHT_POINT;
	light[0].power = 0.3f;  // Much dimmer light (was 1.0)
	light[0].kC = 1.f;
	light[0].kL = 0.05f;  // Faster falloff
	light[0].kQ = 0.01f;  // Stronger quadratic falloff
	light[0].cosCutoff = 45.f;
	light[0].cosInner = 30.f;
	light[0].exponent = 3.f;
	light[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], cosf(glm::radians<float>(light[0].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], cosf(glm::radians<float>(light[0].cosInner)));
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	//// Light 1 - FLASHLIGHT (Spotlight)
	//light[1].position = camera.position;
	//light[1].color = glm::vec3(1.0f, 1.0f, 1.0f);  // Pure white (brighter)
	//light[1].type = Light::LIGHT_SPOT;
	//light[1].power = 50.0f;  // Much brighter! (was 2.0)
	//light[1].kC = 1.f;
	//light[1].kL = 0.01f;
	//light[1].kQ = 0.001f;
	//light[1].cosCutoff = 45.f;  // Wider cone (was 30)
	//light[1].cosInner = 35.f;   // Wider bright center (was 20)
	//light[1].exponent = 3.f;    // Softer falloff (was 5)
	//light[1].spotDirection = camera.target - camera.position;

	//glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	//glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	//glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	//glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	//glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	//glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	//glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], cosf(glm::radians<float>(light[1].cosCutoff)));
	//glUniform1f(m_parameters[U_LIGHT1_COSINNER], cosf(glm::radians<float>(light[1].cosInner)));
	//glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	enableLight = true;
	//flashLightEnabled = true;

	// Setup collision parameters
	cameraRadius = 1.0f;
	numCollisionBoxes = 0;

	// Table - matching your render position and scale
	// Your render: Translate(10.f, 1.3f, 0.f), Scale(0.5f, 0.5f, 0.5f)
	collisionBoxPositions[numCollisionBoxes] = glm::vec3(10.f, 1.3f, 0.f);
	collisionBoxSizes[numCollisionBoxes] = glm::vec3(1.5f, 1.0f, 1.5f);  // Adjusted size
	numCollisionBoxes++;

	// Door - matching your render position
	// Your render: Translate(8.f, 1.5f, 0.f), Rotate(90 on Y), Scale(0.02f)
	collisionBoxPositions[numCollisionBoxes] = glm::vec3(8.f, 1.5f, 0.f);
	collisionBoxSizes[numCollisionBoxes] = glm::vec3(0.3f, 3.0f, 1.5f);  // Thin door rotated
	numCollisionBoxes++;

	// Music Box - on the table
	collisionBoxPositions[numCollisionBoxes] = glm::vec3(10.f, 1.5f, 0.f);
	collisionBoxSizes[numCollisionBoxes] = glm::vec3(0.5f, 0.5f, 0.5f);
	numCollisionBoxes++;

	isDoorOpen = false;
	doorAngle = 0.0f;
	doorAnimationSpeed = 90.0f; // degrees per second

	// Setup tree positions
	treeRadius = 1.5f;
	treePositions[0] = glm::vec3(-5.f, 0.f, -5.f);
	treePositions[1] = glm::vec3(5.f, 0.f, -5.f);
	treePositions[2] = glm::vec3(0.f, 0.f, -8.f);
	treePositions[3] = glm::vec3(7.f, 0.f, -2.5f);
	treePositions[4] = glm::vec3(-5.f, 0.f, 5.f);
	treePositions[5] = glm::vec3(0.f, 0.f, 8.f);
	treePositions[6] = glm::vec3(6.f, 0.f, 3.f);

	// Setup wolf positions
	wolfRadius = 0.8f;
	wolfPositions[0] = glm::vec3(0.f, 0.f, 2.f);
	wolfPositions[1] = glm::vec3(-8.f, 0.f, -3.f);
	wolfPositions[2] = glm::vec3(3.f, 0.f, 6.f);
	wolfPositions[3] = glm::vec3(-3.f, 0.f, 7.f);
	wolfPositions[4] = glm::vec3(9.f, 0.f, -5.f);

	wolvesSpawned = false;
	wolfMoveSpeed = 2.0f; // units per second
	distanceToDoor = 0.0f;
	doorInteractionRadius = 0.0f;
	distanceToBox = 0.0f;
	musicBoxInteractionRadius = 0.0f;

	// Dialogue system initialization
	showDialogue = true;
	dialogueStage = 0;
	dialogueTimer = 0.0;
	wolvesKilled = 0;
	musicBoxActivated = false;
	allWolvesDefeated = false;

	// Jumpscare initialization
	jumpscareActive = false;
	jumpscareTimer = 0.0;
	cameraShakeIntensity = 0.0f;
	jumpscareWolfPosition = glm::vec3(0, 0, 0);
}

void SceneModel2::HandleMouseInput() {
	static bool isLeftUp = false;
	static bool isRightUp = false;

	// Process Left button
	if (!isLeftUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if (isLeftUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = false;
		std::cout << "LBUTTON UP" << std::endl;
	}

	// Process Right button
	if (!isRightUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		isRightUp = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (isRightUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_RIGHT))
	{
		isRightUp = false;
		std::cout << "RBUTTON UP" << std::endl;
	}
}

void SceneModel2::RenderText(Mesh* mesh, std::string text, glm::vec3 color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	// Enable blending  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable back face culling
	glDisable(GL_CULL_FACE);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		glm::mat4 characterSpacing = glm::translate(
			glm::mat4(1.f),
			glm::vec3(i * 1.0f, 0, 0));
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void SceneModel2::RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;

	// Enable blending  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	for (unsigned i = 0; i < text.length(); ++i)
	{
		glm::mat4 characterSpacing = glm::translate(
			glm::mat4(1.f),
			glm::vec3(0.5f + i * 1.0f, 0.5f, 0)
		);
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void SceneModel2::RenderSkybox() {

	// Front
	modelStack.PushMatrix();
	meshList[GEO_FRONT]->material.kAmbient = glm::vec3(0.2f, 0.2f, 0.3f);  // Dark blue tint
	meshList[GEO_FRONT]->material.kDiffuse = glm::vec3(0.2f, 0.2f, 0.3f);
	modelStack.Translate(0.f, 0.f, -25.f);
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_FRONT], true);  // Note: false means no lighting
	modelStack.PopMatrix();

	// Back
	modelStack.PushMatrix();
	meshList[GEO_BACK]->material.kAmbient = glm::vec3(0.2f, 0.2f, 0.3f);
	meshList[GEO_BACK]->material.kDiffuse = glm::vec3(0.2f, 0.2f, 0.3f);
	modelStack.Translate(0.f, 0.f, 25.f);
	modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_BACK], true);
	modelStack.PopMatrix();

	// Left
	modelStack.PushMatrix();
	meshList[GEO_LEFT]->material.kAmbient = glm::vec3(0.2f, 0.2f, 0.3f);
	meshList[GEO_LEFT]->material.kDiffuse = glm::vec3(0.2f, 0.2f, 0.3f);
	modelStack.Translate(-25.f, 0.f, 0.f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_LEFT], true);
	modelStack.PopMatrix();

	// Right
	modelStack.PushMatrix();
	meshList[GEO_RIGHT]->material.kAmbient = glm::vec3(0.2f, 0.2f, 0.3f);
	meshList[GEO_RIGHT]->material.kDiffuse = glm::vec3(0.2f, 0.2f, 0.3f);
	modelStack.Translate(25.f, 0.f, 0.f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_RIGHT], true);
	modelStack.PopMatrix();

	// Top
	modelStack.PushMatrix();
	meshList[GEO_TOP]->material.kAmbient = glm::vec3(0.15f, 0.15f, 0.25f);  // Even darker for sky
	meshList[GEO_TOP]->material.kDiffuse = glm::vec3(0.15f, 0.15f, 0.25f);
	modelStack.Translate(0.f, 25.f, 0.f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_TOP], true);
	modelStack.PopMatrix();

	// Bottom
	modelStack.PushMatrix();
	meshList[GEO_BOTTOM]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.15f);  // Very dark ground
	meshList[GEO_BOTTOM]->material.kDiffuse = glm::vec3(0.1f, 0.1f, 0.15f);
	modelStack.Translate(0.f, -25.f, 0.f);
	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(180.f, 0.f, 0.f, 1.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_BOTTOM], true);
	modelStack.PopMatrix();
}

void SceneModel2::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -1000.f, 1000.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	// Position and scale the GUI element
	modelStack.Translate(x, y, 0.f);
	modelStack.Scale(sizex, sizey, 1.f);

	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneModel2::Update(double dt)
{
	HandleKeyPress();

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

	camera.Update(dt);

	// DIALOGUE SYSTEM
	if (showDialogue)
	{
		dialogueTimer += dt;

		// INTRO DIALOGUES
		// Stage 0: "Where am I? I was just at home a moment ago..."
		if (dialogueStage == 0 && dialogueTimer > 3.0)
		{
			dialogueStage = 1;
			dialogueTimer = 0.0;
		}
		// Stage 1: "Suddenly I'm in the middle of a forest."
		else if (dialogueStage == 1 && dialogueTimer > 3.0)
		{
			dialogueStage = 2;
			dialogueTimer = 0.0;
		}
		// Stage 2: Final intro dialogue
		else if (dialogueStage == 2 && dialogueTimer > 4.0)
		{
			showDialogue = false;
		}

		// MUSIC BOX DIALOGUE
		// Stage 10: "Ominous music starts playing..."
		else if (dialogueStage == 10 && dialogueTimer > 3.0)
		{
			// Spawn wolves after dialogue
			wolvesSpawned = true;
			showDialogue = false;
			std::cout << "Wolves spawned!" << std::endl;
		}
	}

	// JUMPSCARE EFFECT
	if (jumpscareActive)
	{
		jumpscareTimer += dt;

		// Camera shake for 1.5 seconds
		if (jumpscareTimer < 1.5)
		{
			// Random shake offset
			float shakeX = ((rand() % 100 - 50) / 50.0f) * cameraShakeIntensity;
			float shakeY = ((rand() % 100 - 50) / 50.0f) * cameraShakeIntensity;
			float shakeZ = ((rand() % 100 - 50) / 50.0f) * cameraShakeIntensity;

			// Apply shake to camera position
			camera.position.x += shakeX;
			camera.position.y += shakeY;
			camera.position.z += shakeZ;

			// Decrease shake intensity over time
			cameraShakeIntensity *= 0.95f;
		}
		else
		{
			// End jumpscare after 1.5 seconds
			jumpscareActive = false;
			cameraShakeIntensity = 0.0f;

			// Spawn the rest of the wolves
			wolvesSpawned = true;
		}
	}

	// CHECK IF ALL WOLVES DEFEATED
	if (wolvesSpawned && !allWolvesDefeated && wolvesKilled >= 5)
	{
		allWolvesDefeated = true;
		showDialogue = true;
		dialogueStage = 20;  // Stage for after defeating wolves
		dialogueTimer = 0.0;
	}

	// WOLF DEFEAT DIALOGUE CONTINUATION
	if (dialogueStage == 20 && dialogueTimer > 3.0)
	{
		dialogueStage = 21;
		dialogueTimer = 0.0;
	}
	else if (dialogueStage == 21 && dialogueTimer > 4.0)
	{
		showDialogue = false;
	}

	// SKYBOX BOUNDARY COLLISIONS
	// Your skybox is at ±25 units, so we'll constrain the camera within those bounds
	float skyboxBoundary = 24.0f; // Slightly inside the skybox (25 - 1 for buffer)
	float groundLevel = 0.5f;     // Minimum Y position (above ground)
	float ceilingLevel = 23.0f;   // Maximum Y position (below ceiling)

	// Clamp X position
	if (camera.position.x < -skyboxBoundary)
		camera.position.x = -skyboxBoundary;
	if (camera.position.x > skyboxBoundary)
		camera.position.x = skyboxBoundary;

	// Clamp Z position
	if (camera.position.z < -skyboxBoundary)
		camera.position.z = -skyboxBoundary;
	if (camera.position.z > skyboxBoundary)
		camera.position.z = skyboxBoundary;

	// Clamp Y position (optional - prevents going through floor/ceiling)
	if (camera.position.y < groundLevel)
		camera.position.y = groundLevel;
	if (camera.position.y > ceilingLevel)
		camera.position.y = ceilingLevel;

	// DOOR INTERACTION
	glm::vec3 doorPosition = glm::vec3(8.f, 1.5f, 0.f);
	doorInteractionRadius = 2.0f;

	// Calculate distance to door
	float dxToDoor = camera.position.x - doorPosition.x;
	float dzToDoor = camera.position.z - doorPosition.z;
	distanceToDoor = sqrt(dxToDoor * dxToDoor + dzToDoor * dzToDoor);

	// Check if player is near door and presses E
	if (distanceToDoor < doorInteractionRadius)
	{
		if (KeyboardController::GetInstance()->IsKeyPressed('E'))
		{
			isDoorOpen = !isDoorOpen; // Toggle door state
			std::cout << (isDoorOpen ? "Door opened!" : "Door closed!") << std::endl;
		}
	}

	// Animate door opening/closing
	float targetAngle = isDoorOpen ? 90.0f : 0.0f;
	if (doorAngle < targetAngle)
	{
		doorAngle += doorAnimationSpeed * static_cast<float>(dt);
		if (doorAngle > targetAngle) doorAngle = targetAngle;
	}
	else if (doorAngle > targetAngle)
	{
		doorAngle -= doorAnimationSpeed * static_cast<float>(dt);
		if (doorAngle < targetAngle) doorAngle = targetAngle;
	}

	// MUSIC BOX INTERACTION
	glm::vec3 musicBoxPosition = glm::vec3(10.f, 1.3f, 0.f);
	musicBoxInteractionRadius = 2.0f;

	// Calculate distance to music box
	float dxToBox = camera.position.x - musicBoxPosition.x;
	float dzToBox = camera.position.z - musicBoxPosition.z;
	distanceToBox = sqrt(dxToBox * dxToBox + dzToBox * dzToBox);

	// Check if player is near music box and presses left mouse button
	if (distanceToBox < musicBoxInteractionRadius)
	{
		if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			if (!musicBoxActivated)
			{
				musicBoxActivated = true;

				// Turn camera 180 degrees IMMEDIATELY
				camera.RotateYaw(180.f);

				// TRIGGER JUMPSCARE
				jumpscareActive = true;
				jumpscareTimer = 0.0;
				cameraShakeIntensity = 0.3f;  // Shake intensity

				// Position jumpscare wolf right in front of camera
				jumpscareWolfPosition = glm::vec3(2.0f, -3.0f, 0.0f);  // Halfway, eye level

				// Show dialogue after jumpscare
				showDialogue = true;
				dialogueStage = 10;
				dialogueTimer = 0.0;

				std::cout << "Music box activated! JUMPSCARE!" << std::endl;
			}
		}
	}

	// ===== COLLISION DETECTION =====
	// tree collisions
	for (int i = 0; i < 7; i++)
	{
		float dx = camera.position.x - treePositions[i].x;
		float dz = camera.position.z - treePositions[i].z;
		float distanceSquared = dx * dx + dz * dz;
		float minDistance = treeRadius + cameraRadius;

		if (distanceSquared < minDistance * minDistance)
		{
			float distance = sqrt(distanceSquared);

			if (distance > 0.001f)
			{
				// Normalize and push
				float pushDirX = dx / distance;
				float pushDirZ = dz / distance;
				float pushAmount = minDistance - distance;

				camera.position.x += pushDirX * pushAmount;
				camera.position.z += pushDirZ * pushAmount;
			}
		}
	}

	// Check collision with each wolf - ONLY if spawned
	if (wolvesSpawned)
	{
		for (int i = 0; i < 5; i++)
		{
			// Move wolf towards player
			glm::vec3 directionToPlayer = camera.position - wolfPositions[i];
			directionToPlayer.y = 0;  // Keep movement horizontal
			float distanceToPlayer = glm::length(directionToPlayer);

			if (distanceToPlayer > 1.5f)  // Stop when close enough
			{
				directionToPlayer = glm::normalize(directionToPlayer);
				wolfPositions[i] += directionToPlayer * wolfMoveSpeed * static_cast<float>(dt);
			}

			// Check if player clicks on this wolf
			if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			{
				// Simple check: if looking at wolf and close enough
				glm::vec3 viewDirection = glm::normalize(camera.target - camera.position);
				glm::vec3 toWolf = glm::normalize(wolfPositions[i] - camera.position);

				float dotProduct = glm::dot(viewDirection, toWolf);
				float distanceToWolf = glm::distance(camera.position, wolfPositions[i]);

				// If looking at wolf (dot > 0.9) and within range (< 10 units)
				if (dotProduct > 0.9f && distanceToWolf < 10.0f)
				{
					// Check if wolf hasn't been killed yet (not already far away)
					if (wolfPositions[i].x < 999.f)
					{
						// "Despawn" this wolf by moving it far away
						wolfPositions[i] = glm::vec3(1000.f, -1000.f, 1000.f);
						wolvesKilled++;
						std::cout << "Wolf " << i << " despawned! Total killed: " << wolvesKilled << std::endl;
					}
				}
			}

			// Collision detection (push player away)
			float dx = camera.position.x - wolfPositions[i].x;
			float dz = camera.position.z - wolfPositions[i].z;
			float distanceSquared = dx * dx + dz * dz;
			float minDistance = wolfRadius + cameraRadius;

			if (distanceSquared < minDistance * minDistance)
			{
				float distance = sqrt(distanceSquared);

				if (distance > 0.001f)
				{
					float pushDirX = dx / distance;
					float pushDirZ = dz / distance;
					float pushAmount = minDistance - distance;

					camera.position.x += pushDirX * pushAmount;
					camera.position.z += pushDirZ * pushAmount;
				}
			}
		}
	}

	// BOX COLLISIONS FOR RECTANGULAR OBJECTS
	struct CollisionBox {
		glm::vec3 position;
		glm::vec3 halfSize;
	};

	CollisionBox boxes[5];
	int numBoxes = 0;

	// Table - rectangular box
	boxes[numBoxes].position = glm::vec3(10.f, 1.3f, 0.f);
	boxes[numBoxes].halfSize = glm::vec3(0.8f, 0.5f, 0.8f);
	numBoxes++;

	// Door - only add collision when closed
	if (!isDoorOpen)
	{
		boxes[numBoxes].position = glm::vec3(8.f, 1.5f, 0.f);
		boxes[numBoxes].halfSize = glm::vec3(0.3f, 1.5f, 1.0f);
		numBoxes++;
	}

	//// Pumpkin - round
	//boxes[numBoxes].position = glm::vec3(5.f, 1.5f, 0.f);
	//boxes[numBoxes].halfSize = glm::vec3(0.6f, 0.5f, 0.6f);
	//numBoxes++;

	// Check AABB collision
	for (int i = 0; i < numBoxes; i++)
	{
		// Find closest point on the box to the camera
		float closestX = camera.position.x;
		if (closestX < boxes[i].position.x - boxes[i].halfSize.x)
			closestX = boxes[i].position.x - boxes[i].halfSize.x;
		if (closestX > boxes[i].position.x + boxes[i].halfSize.x)
			closestX = boxes[i].position.x + boxes[i].halfSize.x;

		float closestZ = camera.position.z;
		if (closestZ < boxes[i].position.z - boxes[i].halfSize.z)
			closestZ = boxes[i].position.z - boxes[i].halfSize.z;
		if (closestZ > boxes[i].position.z + boxes[i].halfSize.z)
			closestZ = boxes[i].position.z + boxes[i].halfSize.z;

		// Calculate distance
		float dx = camera.position.x - closestX;
		float dz = camera.position.z - closestZ;
		float distanceSquared = dx * dx + dz * dz;

		if (distanceSquared < (cameraRadius * cameraRadius))
		{
			float distance = sqrt(distanceSquared);

			if (distance < 0.001f)
			{
				// Inside the box - push out in shortest direction
				float overlapX = boxes[i].halfSize.x + cameraRadius - fabs(camera.position.x - boxes[i].position.x);
				float overlapZ = boxes[i].halfSize.z + cameraRadius - fabs(camera.position.z - boxes[i].position.z);

				if (overlapX < overlapZ)
				{
					if (camera.position.x < boxes[i].position.x)
						camera.position.x -= overlapX;
					else
						camera.position.x += overlapX;
				}
				else
				{
					if (camera.position.z < boxes[i].position.z)
						camera.position.z -= overlapZ;
					else
						camera.position.z += overlapZ;
				}
			}
			else
			{
				// Push away from closest point
				float pushX = (dx / distance) * (cameraRadius - distance);
				float pushZ = (dz / distance) * (cameraRadius - distance);

				camera.position.x += pushX;
				camera.position.z += pushZ;
			}
		}
	}
}

void SceneModel2::Render()
{
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load view matrix stack and set it with camera position, target position and up direction
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);

	// Load identity matrix into the model stack
	modelStack.LoadIdentity();

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		glm::vec3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightDirection_cameraspace));
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
		glm::vec3 spotDirection_cameraspace = viewStack.Top() * glm::vec4(light[0].spotDirection, 0);
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, glm::value_ptr(spotDirection_cameraspace));
	}
	else {
		// Calculate the light position in camera space
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
	}

	//// Light 1 (Flashlight) - Update ALWAYS, not just when enabled
	//glm::vec3 light1Position_cameraspace = viewStack.Top() * glm::vec4(light[1].position, 1);
	//glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(light1Position_cameraspace));

	//if (light[1].type == Light::LIGHT_SPOT)
	//{
	//	glm::vec3 spot1Direction_cameraspace = viewStack.Top() * glm::vec4(light[1].spotDirection, 0);
	//	glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, glm::value_ptr(spot1Direction_cameraspace));
	//}

	modelStack.PushMatrix();
	// Render objects
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	RenderSkybox();

	//modelStack.PushMatrix();
	//// Render light
	//modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	//modelStack.Scale(0.1f, 0.1f, 0.1f);
	//RenderMesh(meshList[GEO_SPHERE], false);
	//modelStack.PopMatrix();

	// music box
	modelStack.PushMatrix();
	meshList[GEO_MODEL_MTL1]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_MODEL_MTL1]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_MODEL_MTL1]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_MTL1]->material.kShininess = 32.f;
	modelStack.Translate(10.f, 1.3f, 0.f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.01f, 0.01f, 0.01f);
	RenderMesh(meshList[GEO_MODEL_MTL1], true);
	modelStack.PopMatrix();

	// table
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ3]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ3]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ3]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ3]->material.kShininess = 32.f;
	modelStack.Translate(10.f, 1.3f, 0.f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_MODEL_OBJ3], true);
	modelStack.PopMatrix();
	//modelStack.PushMatrix();
	//meshList[GEO_MODEL_OBJ1]->material.kAmbient = glm::vec3(0.5, 0.5f, 0.5f);
	//meshList[GEO_MODEL_OBJ1]->material.kDiffuse = glm::vec3(0.5, 0.5f, 0.5f);
	//meshList[GEO_MODEL_OBJ1]->material.kSpecular = glm::vec3(0.5, 0.5f, 0.5f);
	//meshList[GEO_MODEL_OBJ1]->material.kShininess = 32.f;
	//RenderMesh(meshList[GEO_MODEL_OBJ1], true);
	//modelStack.PopMatrix();

	// Tree 1
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[0].x, treePositions[0].y, treePositions[0].z);
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Tree 2
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[1].x, treePositions[1].y, treePositions[1].z);
	modelStack.Scale(1.2f, 1.2f, 1.2f);
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Tree 3
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[2].x, treePositions[2].y, treePositions[2].z);
	modelStack.Rotate(45.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Tree 4
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[3].x, treePositions[3].y, treePositions[3].z);
	modelStack.Rotate(-30.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.8f, 0.8f, 0.8f);
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Tree 5
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[4].x, treePositions[4].y, treePositions[4].z);
	modelStack.Rotate(15.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1.5f, 1.5f, 1.5f);
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Tree 6
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[5].x, treePositions[5].y, treePositions[5].z);
	modelStack.Rotate(-15.f, 0.f, 1.f, 0.f);
	modelStack.Scale(1.1f, 1.1f, 1.1f);
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Tree 7
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ2]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ2]->material.kShininess = 32.f;
	modelStack.Translate(treePositions[6].x, treePositions[6].y, treePositions[6].z);
	modelStack.Rotate(60.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.9f, 0.9f, 0.9f); // Optional: make it smaller
	RenderMesh(meshList[GEO_MODEL_OBJ2], true);
	modelStack.PopMatrix();

	// Door
	modelStack.PushMatrix();
	meshList[GEO_MODEL_OBJ4]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ4]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ4]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_MODEL_OBJ4]->material.kShininess = 32.f;

	modelStack.Translate(8.f, 1.5f, 0.f);  // hinge position in world
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);  // base orientation
	modelStack.Scale(0.02f, 0.02f, 0.02f);  // scale first
	modelStack.Translate(50.f, 0.f, 0.f);  // moves hinge to other edge
	modelStack.Rotate(-doorAngle, 0.f, 1.f, 0.f);
	modelStack.Translate(-50.f, 0.f, 0.f);  // move back

	RenderMesh(meshList[GEO_MODEL_OBJ4], true);
	modelStack.PopMatrix();

	if (wolvesSpawned) 
	{
		// wolf 1
		modelStack.PushMatrix();
		meshList[GEO_MODEL_OBJ6]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kShininess = 32.f;
		modelStack.Translate(wolfPositions[0].x, wolfPositions[0].y, wolfPositions[0].z);

		float dx = camera.position.x - wolfPositions[0].x;
		float dz = camera.position.z - wolfPositions[0].z;
		float angleToPlayer = glm::degrees(atan2(dx, dz));

		modelStack.Rotate(angleToPlayer, 0, 1, 0);  // Rotate to face player
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_MODEL_OBJ6], true);
		modelStack.PopMatrix();

		// wolf 2
		modelStack.PushMatrix();
		meshList[GEO_MODEL_OBJ6]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kShininess = 32.f;
		modelStack.Translate(wolfPositions[1].x, wolfPositions[1].y, wolfPositions[1].z);

		dx = camera.position.x - wolfPositions[1].x;
		dz = camera.position.z - wolfPositions[1].z;
		angleToPlayer = glm::degrees(atan2(dx, dz));

		modelStack.Rotate(angleToPlayer, 0, 1, 0);
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_MODEL_OBJ6], true);
		modelStack.PopMatrix();

		// wolf 3
		modelStack.PushMatrix();
		meshList[GEO_MODEL_OBJ6]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kShininess = 32.f;
		modelStack.Translate(wolfPositions[2].x, wolfPositions[2].y, wolfPositions[2].z);

		dx = camera.position.x - wolfPositions[2].x;
		dz = camera.position.z - wolfPositions[2].z;
		angleToPlayer = glm::degrees(atan2(dx, dz));

		modelStack.Rotate(angleToPlayer, 0, 1, 0);
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_MODEL_OBJ6], true);
		modelStack.PopMatrix();

		// wolf 4
		modelStack.PushMatrix();
		meshList[GEO_MODEL_OBJ6]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kShininess = 32.f;
		modelStack.Translate(wolfPositions[3].x, wolfPositions[3].y, wolfPositions[3].z);

		dx = camera.position.x - wolfPositions[3].x;
		dz = camera.position.z - wolfPositions[3].z;
		angleToPlayer = glm::degrees(atan2(dx, dz));

		modelStack.Rotate(angleToPlayer, 0, 1, 0);
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_MODEL_OBJ6], true);
		modelStack.PopMatrix();

		// wolf 5
		modelStack.PushMatrix();
		meshList[GEO_MODEL_OBJ6]->material.kAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_MODEL_OBJ6]->material.kShininess = 32.f;
		modelStack.Translate(wolfPositions[4].x, wolfPositions[4].y, wolfPositions[4].z);

		dx = camera.position.x - wolfPositions[4].x;
		dz = camera.position.z - wolfPositions[4].z;
		angleToPlayer = glm::degrees(atan2(dx, dz));

		modelStack.Rotate(angleToPlayer, 0, 1, 0);
		modelStack.Scale(0.5f, 0.5f, 0.5f);
		RenderMesh(meshList[GEO_MODEL_OBJ6], true);
		modelStack.PopMatrix();
    }

	// JUMPSCARE WOLF
	if(jumpscareActive)
	{
		modelStack.PushMatrix();

		// Material settings
		meshList[GEO_MODEL_OBJ6]->material.kAmbient = glm::vec3(0.8, 0.5f, 0.5f);  // Slightly red tint
		meshList[GEO_MODEL_OBJ6]->material.kDiffuse = glm::vec3(0.9f, 0.6f, 0.6f);
		meshList[GEO_MODEL_OBJ6]->material.kSpecular = glm::vec3(1.0f, 0.7f, 0.7f);
		meshList[GEO_MODEL_OBJ6]->material.kShininess = 2.0f;

		modelStack.Translate(jumpscareWolfPosition.x, jumpscareWolfPosition.y, jumpscareWolfPosition.z);

		// Face the camera directly
		glm::vec3 toCamera = camera.position - jumpscareWolfPosition;
		float angleToCamera = glm::degrees(atan2(toCamera.x, toCamera.z));
		modelStack.Rotate(angleToCamera, 0, 1, 0);

		// BIGGER for jumpscare effect
		float scareScale = 1.5f + (jumpscareTimer * 0.5f);  // Grows slightly
		modelStack.Scale(scareScale, scareScale, scareScale);

		RenderMesh(meshList[GEO_MODEL_OBJ6], true);
		modelStack.PopMatrix();
	}

	//RenderMeshOnScreen(meshList[GEO_GUI], 100, 100, 2, 2);

	//modelStack.PushMatrix();
	////scale, translate, rotate
	//RenderText(meshList[GEO_TEXT], "Hello World", glm::vec3(0, 1, 0));
	//modelStack.PopMatrix();

	RenderTextOnScreen(meshList[GEO_TEXT], "CONTROLS:", glm::vec3(1, 1, 1), 20, 10, 550);
	RenderTextOnScreen(meshList[GEO_TEXT], "WASD - Move", glm::vec3(1, 1, 1), 20, 10, 520);
	RenderTextOnScreen(meshList[GEO_TEXT], "Mouse - Look Around", glm::vec3(1, 1, 1), 20, 10, 500);
	RenderTextOnScreen(meshList[GEO_TEXT], "Left click - Attack", glm::vec3(1, 1, 1), 20, 10, 480);

	// Render dialogue text
	if (showDialogue)
	{
		if (dialogueStage == 0)  // "Where am I? I was just at home a moment ago..."
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Where am I? I was just at home", glm::vec3(1, 1, 1), 15, 120, 100);
			RenderTextOnScreen(meshList[GEO_TEXT], "a moment ago...", glm::vec3(1, 1, 1), 15, 230, 75);
		}
		else if (dialogueStage == 1)  // "Suddenly I'm in the middle of a forest."
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Suddenly I'm in the middle", glm::vec3(1, 1, 1), 15, 140, 100);
			RenderTextOnScreen(meshList[GEO_TEXT], "of a forest.", glm::vec3(1, 1, 1), 15, 260, 75);
		}
		else if (dialogueStage == 2)  // "Wait, why is there a door there?"
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Wait, why is there a door there?", glm::vec3(1, 1, 1), 15, 110, 100);
			RenderTextOnScreen(meshList[GEO_TEXT], "Maybe I should check it out.", glm::vec3(1, 1, 1), 15, 140, 75);
		}
		else if (dialogueStage == 10)  // "Ominous music starts playing..."
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Ominous music starts playing...", glm::vec3(1, 1, 1), 15, 130, 90);
		}
		else if (dialogueStage == 20)  // "Those wolves scared me to death!"
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Those wolves scared me to death!", glm::vec3(1, 1, 1), 15, 110, 100);
			RenderTextOnScreen(meshList[GEO_TEXT], "Where did they come from?", glm::vec3(1, 1, 1), 15, 170, 75);
		}
		else if (dialogueStage == 21)  // "Something about this music box..."
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Something about this music box", glm::vec3(1, 1, 1), 15, 130, 100);
			RenderTextOnScreen(meshList[GEO_TEXT], "that summoned them...", glm::vec3(1, 1, 1), 15, 200, 75);
		}
	}

	// Show "Press E to open door" when near door
	if (distanceToDoor < doorInteractionRadius && !isDoorOpen)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press E to open door", glm::vec3(1, 1, 0), 20, 30, 30);
	}

	// Show "Press Left Click" when near music box
	if (distanceToBox < musicBoxInteractionRadius && !wolvesSpawned)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Press LEFT CLICK to activate", glm::vec3(1, 0, 0), 20, 25, 30);
	}

	//pumpkin
	//modelStack.PushMatrix();
	//meshList[GEO_MODEL_OBJ5]->material.kAmbient = glm::vec3(0.5, 0.5f, 0.5f);
	//meshList[GEO_MODEL_OBJ5]->material.kDiffuse = glm::vec3(0.5, 0.5f, 0.5f);
	//meshList[GEO_MODEL_OBJ5]->material.kSpecular = glm::vec3(0.5, 0.5f, 0.5f);
	//meshList[GEO_MODEL_OBJ5]->material.kShininess = 32.f;
	//modelStack.Translate(5.f, 1.5f, 0.f); // Position for table
	//modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	//modelStack.Scale(1.0f, 1.0f, 1.0f);
	//RenderMesh(meshList[GEO_MODEL_OBJ5], true);
	//modelStack.PopMatrix();
}

void SceneModel2::RenderMesh(Mesh* mesh, bool enableLight)
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
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, glm::value_ptr(modelView_inverse_transpose));

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}

	mesh->Render();

	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}


void SceneModel2::Exit()
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

void SceneModel2::HandleKeyPress()
{
	if (KeyboardController::GetInstance()->IsKeyPressed(0x31))
	{
		// Key press to enable culling
		glEnable(GL_CULL_FACE);
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(0x32))
	{
		// Key press to disable culling
		glDisable(GL_CULL_FACE);
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(0x33))
	{
		// Key press to enable fill mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(0x34))
	{
		// Key press to enable wireframe mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_SPACE))
	{
		// Change to black background
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		// Toggle light on or off
	/*	enableLight = !enableLight;*/

		if (light[0].power <= 0.1f)
			light[0].power = 1.f;
		else
			light[0].power = 0.1f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	}

	/*if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_TAB))
	{
		if (light[0].type == Light::LIGHT_POINT) {
			light[0].type = Light::LIGHT_DIRECTIONAL;
		}
		else if (light[0].type == Light::LIGHT_DIRECTIONAL) {
			light[0].type = Light::LIGHT_SPOT;
		}
		else {
			light[0].type = Light::LIGHT_POINT;
		}

		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}*/

	//if (KeyboardController::GetInstance()->IsKeyPressed('F'))
	//{
	//	// Toggle flashlight on/off
	//	flashLightEnabled = !flashLightEnabled;

	//	if (flashLightEnabled)
	//	{
	//		light[1].power = 50.0f;
	//		std::cout << "Flashlight ON" << std::endl;
	//	}
	//	else
	//	{
	//		light[1].power = 0.0f;
	//		std::cout << "Flashlight OFF" << std::endl;
	//	}
	//	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	//}

}
