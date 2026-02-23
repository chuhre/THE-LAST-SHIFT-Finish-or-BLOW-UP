#include "SceneShooting.h"
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

SceneShooting::SceneShooting()
{
}

SceneShooting::~SceneShooting()
{
}

void SceneShooting::Init()
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
	//meshList[GEO_PLANE]->textureID = LoadTGA("Images//met4.tga");

	// OBJ Models

	// props
	//meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism("Counter", glm::vec3(1.f, 1.f, 1.f), 10.f, 1.f, 2.f);
	//meshList[GEO_TARGET_RAIL] = MeshBuilder::GenerateRectangularPrism("Target Rail", glm::vec3(1.f, 1.f, 1.f), 10.f, 0.5f, 0.5f);

	meshList[GEO_TARGET] = MeshBuilder::GenerateOBJMTL("Target", "Models//target.obj", "Models//target.mtl");
	meshList[GEO_TARGET]->textureID = LoadTGA("Images//target_baseColor.tga");
	
	//meshList[GEO_BOMB] = MeshBuilder::GenerateSphere("Bomb", glm::vec3(0.f, 0.f, 0.f), 0.5f, 16, 16);
	meshList[GEO_BOMB] = MeshBuilder::GenerateSphere(
		"Bomb", glm::vec3(0.05f, 0.05f, 0.05f), 0.35f, 16, 16);

	
	meshList[GEO_GUN] = MeshBuilder::GenerateOBJMTL("Gun", "Models//nerf_gun.obj", "Models//nerf_gun.mtl");
	meshList[GEO_GUN]->textureID = LoadTGA("Images//blinn1_baseColor.tga");
	






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

// Counter (barrier between player and targets)
meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism(
    "Counter", glm::vec3(0.55f, 0.35f, 0.15f),   // dark wood
    20.f, 1.0f, 0.4f);

// Target rail (thin bar along back wall)
meshList[GEO_TARGET_RAIL] = MeshBuilder::GenerateCylinder(
    "TargetRail", glm::vec3(0.6f, 0.6f, 0.65f),  // gunmetal grey
    12, 0.12f, 18.f);




	// In Init() — change 4.0f/3.0f -> 16.0f/9.0f (or 1920.0f/1080.0f)
	glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);



	
	
	// ANIMATIONS
	
	// ---------- Game state initialisation ----------
	gameState = STATE_FIND_GUN;
	bulletsLeft = MAX_BULLETS;   // 8
	targetsHit = 0;
	bombTimer = 120.0f;        // 2 minutes
	gunPickedUp = false;
	muzzleFlashTimer = 0.f;
	fps = 0.f;

	// Gun lying on the floor – to the right side, easy to spot
	gunWorldPos = glm::vec3(7.f, 0.3f, 7.f);

	// ---------- Target setup ----------
	// 5 targets on the rail, staggered starting positions, alternating directions
	//   Rail runs roughly X: -8 to +8, at Z = -5, Y = 3.5 (above counter)
	float railY = 3.5f;
	float railZ = -5.0f;

	targets[0] = { glm::vec3(-6.f, railY, railZ), 2.5f, +1.f, -8.f, 8.f, true };
	targets[1] = { glm::vec3(-2.f, railY, railZ), 3.5f, -1.f, -8.f, 8.f, true };
	targets[2] = { glm::vec3(2.f, railY, railZ), 2.0f, +1.f, -8.f, 8.f, true };
	targets[3] = { glm::vec3(5.f, railY, railZ), 4.0f, -1.f, -8.f, 8.f, true };
	targets[4] = { glm::vec3(-4.f, railY, railZ), 3.0f, +1.f, -8.f, 8.f, true };




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





void SceneShooting::Update(double dt)
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
	// --- Target movement (only while playing) ---
	if (gameState == STATE_PLAYING)
	{
		for (int i = 0; i < NUM_TARGETS; ++i)
		{
			if (!targets[i].isAlive) continue;

			targets[i].position.x += targets[i].speed * targets[i].direction * static_cast<float>(dt);

			// Bounce at patrol bounds
			if (targets[i].position.x >= targets[i].maxX)
			{
				targets[i].position.x = targets[i].maxX;
				targets[i].direction = -1.f;
			}
			else if (targets[i].position.x <= targets[i].minX)
			{
				targets[i].position.x = targets[i].minX;
				targets[i].direction = +1.f;
			}
		}

		// --- Bomb timer countdown ---
		bombTimer -= static_cast<float>(dt);
		if (bombTimer <= 0.f)
		{
			bombTimer = 0.f;
			gameState = STATE_LOST;
		}

		// --- Check win ---
		if (targetsHit >= NUM_TARGETS)
			gameState = STATE_WON;
	}

	// --- Muzzle flash decay ---
	if (muzzleFlashTimer > 0.f)
		muzzleFlashTimer -= static_cast<float>(dt);

	// --- FPS counter ---
	fps = static_cast<float>(1.0 / dt);


}

void SceneShooting::Render()
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

	/*modelStack.PushMatrix();
	modelStack.Translate(0.f, 5.f, 0.f);
	modelStack.Scale(10.f, 10.f, 1.f);

	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.0f, 0.0f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_WALL]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();*/



	/*modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 0.f);
	modelStack.Scale(10.f, 0.5f, 10.f);

	meshList[GEO_CUBE]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(0.0f, 0.0f, 0.5f);
	meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_CUBE]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();*/


	/*modelStack.PushMatrix();
	modelStack.Translate(0.f, 0.f, 0.f);
	modelStack.Scale(0.2f, 0.2f, 0.2f);

	meshList[GEO_OBJ]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_OBJ]->material.kDiffuse = glm::vec3(0.0f, 0.0f, 0.5f);
	meshList[GEO_OBJ]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_OBJ]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_OBJ], true);
	modelStack.PopMatrix();*/


	
	
	
	
	
	// gun obj
	modelStack.PushMatrix();
	modelStack.Translate(-15.f, 0.f, 0.f);
	modelStack.Scale(0.01f, 0.01f, 0.01f);

	meshList[GEO_GUN]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_GUN]->material.kDiffuse = glm::vec3(0.0f, 0.0f, 0.5f);
	meshList[GEO_GUN]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_GUN]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_GUN], true);
	modelStack.PopMatrix(); 


	// target obj
	modelStack.PushMatrix();
	modelStack.Translate(5.f, 0.f, 0.f);
	modelStack.Scale(1.5f, 1.5f, 1.5f);
	modelStack.Rotate(90.0f, 0, 1, 0);

	meshList[GEO_TARGET]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
	meshList[GEO_TARGET]->material.kDiffuse = glm::vec3(0.0f, 0.0f, 0.5f);
	meshList[GEO_TARGET]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_TARGET]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_TARGET], true);
	modelStack.PopMatrix();






	// BOOTH ROOT – all booth geometry lives inside this push/pop.
// Translate here if you ever want to move the whole booth at once.
// ------------------------------------------------------------------
	modelStack.PushMatrix();                        // >>> BOOTH ROOT
	modelStack.Translate(9.f, 0.f, 0.f);           // booth world origin

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


	// ------------------------------------------------------------------
	// COUNTER (PARENT)
	//   World position: centred X, Y=0.5, Z=1.5
	//   Children inherit this transform before applying their own offset.
	// ------------------------------------------------------------------
	modelStack.PushMatrix();                    // >>> COUNTER PARENT
	modelStack.Translate(0.f, 0.5f, 1.5f);     // counter world position

	// Render counter itself
	meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
	meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
	meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
	meshList[GEO_COUNTER]->material.kShininess = 8.f;
	RenderMesh(meshList[GEO_COUNTER], true);

	// ---- BOMB (CHILD of Counter) ----
	// Offset: +0.5 in Y (sits on top of counter surface),
	//         -7 in X (left end of counter)
	// No extra Z needed – inherits counter's Z=1.5
	modelStack.PushMatrix();                // >>> BOMB CHILD
	modelStack.Translate(-7.f, 0.85f, 0.f);
	meshList[GEO_BOMB]->material.kAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_BOMB]->material.kDiffuse = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BOMB]->material.kSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
	meshList[GEO_BOMB]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_BOMB], true);
	modelStack.PopMatrix();                 // <<< BOMB CHILD

	modelStack.PopMatrix();                     // <<< COUNTER PARENT


	// ------------------------------------------------------------------
	// TARGET RAIL (PARENT)
	//   World position: centred X, Y=3.5, Z=-5  (above counter, back wall)
	//   The cylinder's long axis is vertical by default so we rotate 90°
	//   around Z to lay it horizontally along X.
	//   All targets are children – their X offset is relative to the
	//   rail's centre, so moving the rail moves all targets with it.
	// ------------------------------------------------------------------
	modelStack.PushMatrix();                    // >>> RAIL PARENT
	modelStack.Translate(0.f, 3.5f, -5.0f);    // rail world position
	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);    // lay cylinder along X axis

	// Render rail itself
	meshList[GEO_TARGET_RAIL]->material.kAmbient = glm::vec3(0.2f, 0.2f, 0.22f);
	meshList[GEO_TARGET_RAIL]->material.kDiffuse = glm::vec3(0.55f, 0.55f, 0.6f);
	meshList[GEO_TARGET_RAIL]->material.kSpecular = glm::vec3(0.8f, 0.8f, 0.8f);
	meshList[GEO_TARGET_RAIL]->material.kShininess = 32.f;
	RenderMesh(meshList[GEO_TARGET_RAIL], true);

	// ---- TARGETS (CHILDREN of Rail) ----
	// Because the rail was rotated 90° around Z, the local axes
	// are swapped. We undo that rotation for each child so their
	// own translate/scale/rotate behave normally.
	for (int i = 0; i < NUM_TARGETS; ++i)
	{
		if (!targets[i].isAlive) continue;

		modelStack.PushMatrix();            // >>> TARGET[i] CHILD

		// Undo parent's 90° Z rotation so child X/Y/Z feel normal
		modelStack.Rotate(-90.f, 0.f, 0.f, 1.f);

		// targets[i].position is in world space;
		// subtract the rail's world origin to get local offset
		float localX = targets[i].position.x - 0.f;   // rail centred at X=0
		float localY = targets[i].position.y - 3.5f;   // rail at Y=3.5
		// Z is the same as rail so local Z offset = 0
		modelStack.Translate(localX, localY, 0.f);

		modelStack.Rotate(180.f, 0.f, 1.f, 0.f);       // face player
		modelStack.Scale(1.5f, 1.5f, 1.5f);

		meshList[GEO_TARGET]->material.kAmbient = glm::vec3(0.2f, 0.1f, 0.1f);
		meshList[GEO_TARGET]->material.kDiffuse = glm::vec3(0.9f, 0.3f, 0.3f);
		meshList[GEO_TARGET]->material.kSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
		meshList[GEO_TARGET]->material.kShininess = 10.f;
		RenderMesh(meshList[GEO_TARGET], true);

		modelStack.PopMatrix();             // <<< TARGET[i] CHILD
	}

	modelStack.PopMatrix();                     // <<< RAIL PARENT

	modelStack.PopMatrix();                         // <<< BOOTH ROOT


	// ------------------------------------------------------------------
	// GUN – independent (not part of booth hierarchy)
	// Only rendered while it hasn't been picked up yet.
	// ------------------------------------------------------------------
	if (!gunPickedUp)
	{
		modelStack.PushMatrix();                    // >>> GUN
		modelStack.Translate(gunWorldPos.x, gunWorldPos.y, gunWorldPos.z);
		modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
		modelStack.Scale(0.02f, 0.02f, 0.02f);
		meshList[GEO_GUN]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
		meshList[GEO_GUN]->material.kDiffuse = glm::vec3(0.6f, 0.55f, 0.5f);
		meshList[GEO_GUN]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_GUN]->material.kShininess = 20.f;
		RenderMesh(meshList[GEO_GUN], true);
		modelStack.PopMatrix();                     // <<< GUN
	}



}

void SceneShooting::RenderMesh(Mesh* mesh, bool enableLight)
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


//void SceneShooting::RenderSkybox() {
//	modelStack.PushMatrix();
//
//	// Offset in Z direction by -50 units
//	modelStack.Translate(0.f, 0.f, -50.f);
//
//	// Skybox should be rendered without light
//	RenderMesh(meshList[GEO_FRONT], false);
//	modelStack.PopMatrix();
//
//	// Do the rest of the quads with
//	// appropriate positions and rotations
//	// so that the camera is inside the skybox
//
//	modelStack.PushMatrix();
//	modelStack.Translate(0.f, 0.f, -50.f);
//	RenderMesh(meshList[GEO_FRONT], false);
//	modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	modelStack.Translate(0.f, 0.f, 50.f);
//	modelStack.Rotate(-180.f, 0.f, 1.f, 0.f);
//	RenderMesh(meshList[GEO_BACK], false);
//	modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	modelStack.Translate(-50.f, 0.f, 0.f);
//	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
//	RenderMesh(meshList[GEO_LEFT], false);
//	modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	modelStack.Translate(50.f, 0.f, 0.f);
//	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
//	RenderMesh(meshList[GEO_RIGHT], false);
//	modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	modelStack.Translate(0.f, 50.f, 0.f);
//	modelStack.Rotate(90.f, 1.f, 0.f, 0.f);
//	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);
//	RenderMesh(meshList[GEO_TOP], false);
//	modelStack.PopMatrix();
//
//	modelStack.PushMatrix();
//	modelStack.Translate(0.f, -50.f, 0.f);
//	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);
//	RenderMesh(meshList[GEO_BOTTOM], false);
//	modelStack.PopMatrix();
//
//}



void SceneShooting::RenderMeshOnScreen(Mesh* mesh, float x, float
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






void SceneShooting::Exit()
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

void SceneShooting::HandleKeyPress()
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

void SceneShooting::HandleMouseInput() {
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



void SceneShooting::RenderText(Mesh* mesh, std::string text, glm::vec3
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



void SceneShooting::RenderTextOnScreen(Mesh* mesh, std::string
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