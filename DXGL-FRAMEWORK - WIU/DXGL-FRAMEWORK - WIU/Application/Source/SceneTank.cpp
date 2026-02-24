#include "SceneTank.h"
#include "GL\glew.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>

//Include GLFW
#include <GLFW/glfw3.h>

#include <iostream>

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "LoadTGA.h"

SceneTank::SceneTank()
{
}

SceneTank::~SceneTank()
{
}

void SceneTank::Init()
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

	// Initialise camera properties
	//camera.Init(45.f, 45.f, 10.f);
	camera.Init(
		glm::vec3(0, 2.1, 10),		// position
		glm::vec3(0, 2, 0),		// target
		glm::vec3(0, 1.0f, 0)		// up
	);

	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sun", glm::vec3(1.f, 1.f, 1.f), 1.f, 16, 16);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Arm", glm::vec3(0.5f, 0.5f, 0.5f), 1.f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_PLANK] = MeshBuilder::GenerateRectangularPrism("Plank", glm::vec3(1.f, 1.f, 1.f), 10.f, 5.f, 0.5f);
	meshList[GEO_BALL] = MeshBuilder::GenerateSphere("Ball", glm::vec3(1.f, 1.f, 1.f), 0.5f, 16, 16);
	meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism("Counter", glm::vec3(0.55f, 0.35f, 0.15f), 15.f, 1.0f, 1.0f);
	meshList[GEO_PILLAR] = MeshBuilder::GenerateRectangularPrism("Pillar", glm::vec3(0.55f, 0.35f, 0.15f), 1.f, 5.f, 1.f);
	//meshList[GEO_PLANE]->textureID = LoadTGA("Images//met4.tga");

	// OBJ Models
	meshList[GEO_TARGET] = MeshBuilder::GenerateOBJMTL("Target", "Models//target.obj", "Models//target.mtl");
	meshList[GEO_TARGET]->textureID = LoadTGA("Images//target_baseColor.tga");
	meshList[GEO_TANK] = MeshBuilder::GenerateOBJMTL("Tank", "Models//tank.obj", "Models//tank.mtl");
	meshList[GEO_LADDER] = MeshBuilder::GenerateOBJ("Ladder", "Models//ladder.obj");
	meshList[GEO_LADDER]->textureID = LoadTGA("Images//Ladder_BaseColor.tga");
	meshList[GEO_BOX] = MeshBuilder::GenerateOBJ("Box1", "Models//box.obj");
	meshList[GEO_BOX]->textureID = LoadTGA("Images//box.tga");
	meshList[GEO_BOX2] = MeshBuilder::GenerateOBJ("Box2", "Models//box.obj");
	meshList[GEO_BOX2]->textureID = LoadTGA("Images//box.tga");
	meshList[GEO_BOX3] = MeshBuilder::GenerateOBJ("Box3", "Models//box.obj");
	meshList[GEO_BOX3]->textureID = LoadTGA("Images//box.tga");
	meshList[GEO_BOX4] = MeshBuilder::GenerateOBJ("Box4", "Models//box.obj");
	meshList[GEO_BOX4]->textureID = LoadTGA("Images//box.tga");
	meshList[GEO_CABINET] = MeshBuilder::GenerateOBJ("Cabinet", "Models//iron_cabinet.obj");
	meshList[GEO_CABINET]->textureID = LoadTGA("Images//iron_cabinet_MT_BaseColor.1002.tga");

	// Environment
	meshList[GEO_FLOOR] = MeshBuilder::GenerateRectangularPrism(
		"Floor", glm::vec3(0.45f, 0.32f, 0.18f),   // dark wood brown
		20.f, 0.2f, 15.f);

	meshList[GEO_CEILING] = MeshBuilder::GenerateRectangularPrism(
		"Ceiling", glm::vec3(0.85f, 0.75f, 0.55f),  // light tan canvas
		20.f, 0.2f, 15.f);

	meshList[GEO_WALL] = MeshBuilder::GenerateRectangularPrism(
		"Wall", glm::vec3(0.9f, 0.85f, 0.6f),        // carnival cream
		1.f, 1.f, 1.f);   // scaled per-wall in Render()


	// Skybox NIGHT
	/*meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Images//nightsky_lf.tga");

	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Images//nightsky_rt.tga");

	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Images//nightsky_up.tga");

	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Images//nightsky_dn.tga");

	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Images//nightsky_bk.tga");

	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Images//nightsky_ft.tga ");*/




	// In Init() — change 4.0f/3.0f -> 16.0f/9.0f (or 1920.0f/1080.0f)
	glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	// Player collision box size (width, height, depth)
	playerSize = glm::vec3(0.4f, 1.8f, 0.4f);


	// ANIMATIONS




	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	light[0].position = glm::vec3(0, 5, 0);
	light[0].color = glm::vec3(1, 1, 1);
	light[0].type = Light::LIGHT_POINT;
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
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

	enableLight = true;


}





void SceneTank::Update(double dt)
{
	HandleKeyPress();
	HandleMouseInput();

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


	// Store position before camera update
	glm::vec3 oldPos = camera.position;

	// Update camera position based on input
	camera.Update(dt);













	// === ANIMATION/INTERACTIONS ====



}

void SceneTank::Render()
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

	modelStack.PushMatrix();
	// Render objects
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Render light
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	modelStack.Scale(0.1f, 0.1f, 0.1f);
	RenderMesh(meshList[GEO_SPHERE], false);
	modelStack.PopMatrix();


	// render tests

	modelStack.PushMatrix();                        // >>> BOOTH ROOT
	modelStack.Translate(0.f, 0.f, 0.f);           // booth world origin

	// ---- FLOOR ----
	modelStack.PushMatrix();                    // >>> Floor
	modelStack.Translate(0.f, 0.f, 0.f);
	meshList[GEO_FLOOR]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_FLOOR]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.2f);
	meshList[GEO_FLOOR]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FLOOR]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_FLOOR], true);
	modelStack.PopMatrix();                     // <<< Floor

	// ---- CEILING ----
	modelStack.PushMatrix();                    // >>> Ceiling
	modelStack.Translate(0.f, 8.f, 0.f);
	meshList[GEO_CEILING]->material.kAmbient = glm::vec3(0.4f, 0.35f, 0.25f);
	meshList[GEO_CEILING]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.55f);
	meshList[GEO_CEILING]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_CEILING]->material.kShininess = 1.f;
	RenderMesh(meshList[GEO_CEILING], true);
	modelStack.PopMatrix();                     // <<< Ceiling

	// ---- BACK WALL ----
	modelStack.PushMatrix();                    // >>> Back Wall
	modelStack.Translate(0.f, 4.f, -7.5f);
	modelStack.Scale(20.f, 8.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();                     // <<< Back Wall

	// ---- LEFT WALL ----
	modelStack.PushMatrix();                    // >>> Left Wall
	modelStack.Translate(-10.f, 4.f, 0.f);
	modelStack.Scale(0.3f, 8.f, 15.f);
	RenderMesh(meshList[GEO_WALL], true);       // reuses same mesh + material
	modelStack.PopMatrix();                     // <<< Left Wall

	// ---- RIGHT WALL ----
	modelStack.PushMatrix();                    // >>> Right Wall
	modelStack.Translate(10.f, 4.f, 0.f);
	modelStack.Scale(0.3f, 8.f, 15.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();                     // <<< Right Wall

	// dunk tank
	modelStack.PushMatrix();
	modelStack.Translate(-5.5f, 2.f, 0.f);
	modelStack.Scale(2.f, 2.f, 2.f);

	meshList[GEO_TANK]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_TANK]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_TANK]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_TANK]->material.kShininess = 5.0f;

	// plank
	modelStack.PushMatrix();
	modelStack.Translate(2.6f, 1.f, 0.f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.15f, 0.15f, 0.15f);
	
	meshList[GEO_PLANK]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_PLANK]->material.kDiffuse = glm::vec3(0.6f, 0.4f, 0.2f);
	meshList[GEO_PLANK]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_PLANK]->material.kShininess = 2.0f;

	RenderMesh(meshList[GEO_PLANK], true);
	modelStack.PopMatrix();
	RenderMesh(meshList[GEO_TANK], true);
	modelStack.PopMatrix();

	// target stand
	modelStack.PushMatrix();
	modelStack.Translate(2.5f, 1.f, 0.f);

	meshList[GEO_PILLAR]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_PILLAR]->material.kDiffuse = glm::vec3(0.6f, 0.4f, 0.2f);
	meshList[GEO_PILLAR]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_PILLAR]->material.kShininess = 5.0f;
	
	RenderMesh(meshList[GEO_PILLAR], true);
	modelStack.PopMatrix();

	// target
	modelStack.PushMatrix();
	modelStack.Translate(3.5f, 3.f, 0.f);
	modelStack.Rotate(90.0f, 0, 1, 0);
	modelStack.Rotate(270.0f, 1, 0, 0);
	modelStack.Scale(1.5f, 1.5f, 1.5f);

	meshList[GEO_TARGET]->material.kAmbient = glm::vec3(0.2f, 0.1f, 0.1f);
	meshList[GEO_TARGET]->material.kDiffuse = glm::vec3(0.9f, 0.3f, 0.3f);
	meshList[GEO_TARGET]->material.kSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
	meshList[GEO_TARGET]->material.kShininess = 2.0f;

	RenderMesh(meshList[GEO_TARGET], true);
	modelStack.PopMatrix();

	// ladder
	modelStack.PushMatrix();
	modelStack.Translate(-2.f, 0.f, 0.f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	modelStack.Rotate(2.f, 1.f, 0.f, 0.f);
	modelStack.Scale(0.01f, 0.01f, 0.01f);

	meshList[GEO_LADDER]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_LADDER]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_LADDER]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_LADDER]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_LADDER], true);
	modelStack.PopMatrix();

	// counter
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.5f, 3.f);

	meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
	meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
	meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
	meshList[GEO_COUNTER]->material.kShininess = 8.f;

	RenderMesh(meshList[GEO_COUNTER], true);
	modelStack.PopMatrix();

	// balls
	modelStack.PushMatrix();
	modelStack.Translate(1.f, 1.1f, 3.f);
	modelStack.Scale(0.3f, 0.3f, 0.3f);

	meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.25f, 0.22f, 0.18f);
	meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.9f, 0.85f, 0.7f);   // off-white leather
	meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.15f, 0.15f, 0.15f); // slightly shiny
	meshList[GEO_BALL]->material.kShininess = 8.0f;
	
	RenderMesh(meshList[GEO_BALL], true);
	modelStack.PopMatrix();

	// box
	modelStack.PushMatrix();
	modelStack.Translate(-8.9f, 0.5f, -3.f);
	modelStack.Scale(0.3f, 0.3f, 0.3f);

	meshList[GEO_BOX]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_BOX]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_BOX], true);
	modelStack.PopMatrix();

	// box 2
	modelStack.PushMatrix();
	modelStack.Translate(-8.9f, 1.56f, -3.f);
	modelStack.Rotate(45.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.3f, 0.3f, 0.3f);

	meshList[GEO_BOX2]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_BOX2]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX2]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX2]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_BOX2], true);
	modelStack.PopMatrix();

	// box 3
	modelStack.PushMatrix();
	modelStack.Translate(-7.9f, 0.5f, -2.f);
	modelStack.Rotate(-45.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.3f, 0.3f, 0.3f);

	meshList[GEO_BOX3]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_BOX3]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX3]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX3]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_BOX3], true);
	modelStack.PopMatrix();

	// box 4
	modelStack.PushMatrix();
	modelStack.Translate(-7.9f, 0.5f, -4.0f);
	modelStack.Rotate(50.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.3f, 0.3f, 0.3f);

	meshList[GEO_BOX4]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_BOX4]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX4]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOX4]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_BOX4], true);
	modelStack.PopMatrix();

	// cabinet
	modelStack.PushMatrix();
	modelStack.Translate(8.9f, 0.f, -5.f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(2.f, 2.f, 2.f);
	
	meshList[GEO_CABINET]->material.kAmbient = glm::vec3(0.25f, 0.25f, 0.25f);
	meshList[GEO_CABINET]->material.kDiffuse = glm::vec3(0.55f, 0.55f, 0.55f);
	meshList[GEO_CABINET]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_CABINET]->material.kShininess = 8.f;

	RenderMesh(meshList[GEO_CABINET], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix();
}

void SceneTank::RenderMesh(Mesh* mesh, bool enableLight)
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


void SceneTank::RenderSkybox() {
	modelStack.PushMatrix();

	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, -50.f);

	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	// Do the rest of the quads with
	// appropriate positions and rotations
	// so that the camera is inside the skybox

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, -50.f);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 50.f);
	modelStack.Rotate(-180.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-50.f, 0.f, 0.f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(50.f, 0.f, 0.f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, 50.f, 0.f);
	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(0.f, -50.f, 0.f);
	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();

}



void SceneTank::RenderMeshOnScreen(Mesh* mesh, float x, float
	y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 1920.f, 0.f, 1080.f, -1000.f, 1000.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);

	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode

	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	// To do: Use modelStack to position GUI on screen
	modelStack.Translate(x, y, 0);

	// To do: Use modelStack to scale the GUI
	modelStack.Scale(sizex, sizey, 1);

	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();

	glEnable(GL_DEPTH_TEST);
}






void SceneTank::Exit()
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

void SceneTank::HandleKeyPress()
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

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_TAB))
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
	}

}

void SceneTank::HandleMouseInput() {
	static bool isLeftUp = false;
	static bool isRightUp = false;

	// Process Left button
	if (!isLeftUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = true;
		std::cout << "LBUTTON DOWN" << std::endl;

		// transform into UI space
		double x = MouseController::GetInstance()->GetMousePositionX();
		double y = 1080 - MouseController::GetInstance()->GetMousePositionY();

		// Check if mouse click position is within the GUI box
		// Change the boundaries as necessary
		if (x > 0 && x < 100 && y > 0 && y < 100) {
			std::cout << "GUI IS CLICKED" << std::endl;
		}

	}
	else if (isLeftUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = false;
		std::cout << "LBUTTON UP" << std::endl;
	}

	// Continue to do for right button
}



void SceneTank::RenderText(Mesh* mesh, std::string text, glm::vec3
	color)
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
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() *
			modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE,
			glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}



void SceneTank::RenderTextOnScreen(Mesh* mesh, std::string
	text, glm::vec3 color, float size, float x, float y)
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
		glm::mat4 MVP = projectionStack.Top() *
			viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE,
			glm::value_ptr(MVP));
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