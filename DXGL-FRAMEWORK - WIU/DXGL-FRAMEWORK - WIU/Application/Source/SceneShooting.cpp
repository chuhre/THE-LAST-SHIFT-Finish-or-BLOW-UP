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
#include "SceneManager.h"

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
		glm::vec3(0, 2.1, 6),		// position
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


	// UI
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_GUI] = MeshBuilder::GenerateQuad("GUI", glm::vec3(1, 1, 1), 1.f);


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

// Door
//SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_SHOOTING] = false;
meshList[GEO_DOOR] = MeshBuilder::GenerateCube("Door", glm::vec3(1.f, 1.f, 1.f), 1.f);



	// In Init() — change 4.0f/3.0f -> 16.0f/9.0f (or 1920.0f/1080.0f)
	glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	// Player collision box size (width, height, depth)
	playerSize = glm::vec3(0.4f, 1.8f, 0.4f);

	
	
	// ANIMATIONS

	//initialise door 
	door[0] = { glm::vec3(1.f, 2.f, 7.5f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };
	
	// ---------- Game state ----------
	gameState = STATE_FIND_GUN;
	bulletsLeft = MAX_BULLETS;
	targetsHit = 0;
	bombTimer = 120.0f;
	gunPickedUp = false;
	muzzleFlashTimer = 0.f;
	fps = 0.f;
	playerLocked = false;
	atBooth = false;
	boothEntryPos = glm::vec3(0.f, 2.1f, 2.f);  // front of counter

	// Fixed shooting position – centred behind counter
	shootingPos = glm::vec3(0.f, 2.1f, 5.f);
	shootingTarget = glm::vec3(0.f, 3.5f, -5.f);  // looking at rail

	// Gun lying on the floor – to the right side, easy to spot
	gunWorldPos = glm::vec3(7.f, 0.6f, 5.f);

	// ---------- Target setup ----------
	// All positions are LOCAL to the rail (rail is at world Y=3.5, Z=-5)
	// X is the only axis that changes during looping movement
	// Evenly spaced 3 units apart
	//float startPositions[NUM_TARGETS] = { -6.f, -3.f, 0.f, 3.f, 6.f };
	float speed = 3.0f;  // same speed for all targets

	for (int i = 0; i < NUM_TARGETS; ++i)
	{
		// Evenly spaced 3 units apart across the rail
		float startX = -6.f + (i * 3.f);  // -6, -3, 0, 3, 6

		targets[i].physics.pos = Vector3(startX, 0.f, 0.f);
		targets[i].physics.vel = Vector3(0.f, 0.f, 0.f);
		targets[i].physics.accel = Vector3(0.f, 0.f, 0.f);  // no gravity until hit
		targets[i].physics.mass = 1.f;
		targets[i].physics.bounciness = 0.f;   // targets don't bounce when they fall
		targets[i].speed = speed;
		targets[i].minX = -8.f;
		targets[i].maxX = 8.f;
		targets[i].isAlive = true;
		targets[i].isFalling = false;
	}

	// ---------- Bullet pool ----------
	for (int i = 0; i < POOL_SIZE; ++i)
	{
		bulletPool[i].active = false;
		bulletPool[i].physics.pos = Vector3(0.f, 0.f, 0.f);
		bulletPool[i].physics.vel = Vector3(0.f, 0.f, 0.f);
		bulletPool[i].physics.accel = Vector3(0.f, -2.0f, 0.f);  // very slight gravity
		bulletPool[i].physics.mass = 0.1f;
		bulletPool[i].physics.bounciness = 0.f;
	}


	
	// --- Collision boxes for walls, floor, and counter
	BuildCollisionBoxes();



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
	// Store position before camera update
	glm::vec3 oldPos = camera.position;
	glm::vec3 oldTarget = camera.target;

	//// Update camera position based on input
	//camera.Update(dt);

	// --- Camera update ---
	if (playerLocked)
	{
		// Lock camera to fixed shooting position
		/*camera.position = shootingPos;
		camera.target = shootingTarget;*/

		camera.Update(dt);                  // allow mouse look
		camera.position = shootingPos;      // but snap position back every frame
	}
	else
	{
		camera.Update(dt);

		// --- Collision Resolution ---
		glm::vec3 updatedPos = camera.position;

		// Resolve Vertical Collision: floor/ceiling
		camera.position = glm::vec3(oldPos.x, updatedPos.y, oldPos.z);
		for (const AABB& box : collisionBoxes)
		{
			if (CheckAABBCollision(camera.position, 0.3f, box))
			{
				// If hit something vertically, go back to old Y
				camera.position.y = oldPos.y;
				camera.target.y = oldTarget.y;
				break;
			}
		}

		// Resolve Horizontal Collision
		float currentY = camera.position.y;
		camera.position = glm::vec3(updatedPos.x, currentY, updatedPos.z);

		for (const AABB& box : collisionBoxes)
		{
			if (CheckAABBCollision(camera.position, 0.3f, box))
			{
				// If hit a wall, revert X and Z
				camera.position.x = oldPos.x;
				camera.position.z = oldPos.z;
				camera.target.x = oldTarget.x;
				camera.target.z = oldTarget.z;
				break;
			}
		}
	}

	float fdt = static_cast<float>(dt);












	// === ANIMATION/INTERACTIONS ====
	// Door interaction
	showInteractPrompt = false;
	showLockedPrompt = false;

	if (door[0].IsPlayerNear(camera.position, 2.5f))
	{
		if (SceneManager::GetInstance()->getIsGameCompleted(SceneManager::SCENE_SHOOTING))
			showInteractPrompt = true;
		else
			showLockedPrompt = true;  // game not won yet
	}
	if (showInteractPrompt && KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		door[0].Open();
	}
	if (door[0].Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f))
	{
		SceneManager::GetInstance()->SwitchScene(door[0].leadsTo);
		door[0].Close();
		showInteractPrompt = true;
	}

	

	
	// --- STATE_FIND_GUN: just let player walk around ---
	if (gameState == STATE_FIND_GUN)
	{
		// Nothing extra needed, player walks freely
	}

	// --- STATE_PLAYING ---
	if (gameState == STATE_PLAYING)
	{
		// ---- Target movement ----
		for (int i = 0; i < NUM_TARGETS; ++i)
		{
			if (!targets[i].isAlive) continue;

			if (!targets[i].isFalling)
			{
				// Conveyor loop: all move right at same speed
				targets[i].physics.pos.x += targets[i].speed * fdt;

				if (targets[i].physics.pos.x > targets[i].maxX)
					targets[i].physics.pos.x = targets[i].minX;
			}
			else
			{
				// Falling: physics with gravity
				targets[i].physics.UpdatePhysics(fdt);

				// Floor in local rail space = -3.5 (rail is at world Y=3.5)
				if (targets[i].physics.pos.y <= -3.5f)
				{
					targets[i].physics.pos.y = -3.5f;
					targets[i].isAlive = false;
				}
			}
		}

		// ---- Bullet update ----
		for (int i = 0; i < POOL_SIZE; ++i)
		{
			if (!bulletPool[i].active) continue;

			bulletPool[i].physics.UpdatePhysics(fdt);

			// Deactivate if hits floor
			if (bulletPool[i].physics.pos.y <= 0.f)
			{
				bulletPool[i].active = false;
				continue;
			}

			// Deactivate if out of bounds
			if (bulletPool[i].physics.pos.z < -8.f ||
				bulletPool[i].physics.pos.x < -12.f ||
				bulletPool[i].physics.pos.x >  12.f)
			{
				bulletPool[i].active = false;
				continue;
			}

			// ---- Bullet vs Target collision ----
			for (int j = 0; j < NUM_TARGETS; ++j)
			{
				if (!targets[j].isAlive || targets[j].isFalling) continue;

				// Reconstruct target world position
				// Rail is at world (0, 3.5, -5), target local X offsets along world X
				Vector3 targetWorldPos(
					targets[j].physics.pos.x,
					targets[j].physics.pos.y + 3.5f,
					-5.0f
				);

				float bulletRadius = 0.15f;
				float targetRadius = 1.0f;

				if (OverlapCircle2Circle(
					bulletPool[i].physics.pos, bulletRadius,
					targetWorldPos, targetRadius))
				{
					// Target hit – start falling
					targets[j].isFalling = true;
					targets[j].physics.accel = Vector3(0.f, -9.8f, 0.f);
					targets[j].physics.vel = Vector3(0.f, -1.f, 0.f);
					targetsHit++;

					bulletPool[i].active = false;

					if (targetsHit >= NUM_TARGETS)
					{
						gameState = STATE_WON;
						SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_SHOOTING] = true;
						// Clear all remaining bullets
						for (int k = 0; k < POOL_SIZE; ++k)
							bulletPool[k].active = false;
						// Remove all remaining targets immediately
						for (int k = 0; k < NUM_TARGETS; ++k)
						{
							targets[k].isAlive = false;
							targets[k].isFalling = false;
						}
					}
					else
					{
						SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_SHOOTING] = false;
					}
				}
			}
		}

		// ---- Bomb timer ----
		bombTimer -= fdt;
		if (bombTimer <= 0.f)
		{
			bombTimer = 0.f;
			gameState = STATE_LOST;
		}

		// ---- Out of bullets lose check ----
		// Count active bullets still in air
		int activeBullets = 0;
		for (int i = 0; i < POOL_SIZE; ++i)
			if (bulletPool[i].active) activeBullets++;

		if (bulletsLeft <= 0 && activeBullets == 0 && targetsHit < NUM_TARGETS)
			gameState = STATE_LOST;
	}

	// --- STATE_WON: unlock camera so player can walk freely ---
	if (gameState == STATE_WON)
	{
		playerLocked = false;
		glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL); // show cursor again
		// camera.Update(dt) already called above since playerLocked = false
		return;
	}

	// --- STATE_LOST: camera stays locked (at booth) ---
	// Player presses R to restart in place (handled in HandleKeyPress)

	// Muzzle flash decay
	if (muzzleFlashTimer > 0.f)
		muzzleFlashTimer -= fdt;

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







	// BOOTH ROOT – all booth geometry lives inside this push/pop.
	// Translate here if you ever want to move the whole booth at once.
	// ------------------------------------------------------------------
	modelStack.PushMatrix();                        // >>> BOOTH ROOT
	modelStack.Translate(0.f, 0.f, 0.f);           // booth world origin

	// ---- FLOOR ----
	modelStack.PushMatrix();                    // >>> Floor
	modelStack.Translate(0.f, 0.f, 0.f);
	modelStack.Scale(20.f, 1.f, 15.f);

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

	// --- FRONT WALL ---
	//front wall L
	modelStack.PushMatrix();
	modelStack.Translate(5.5f, 4.f, 7.5f);
	modelStack.Scale(9.f, 8.f, 0.3f);

	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//front wall R
	modelStack.PushMatrix();
	modelStack.Translate(-5.5f, 4.f, 7.5f);
	modelStack.Scale(9.f, 8.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//door frame
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 5.9f, 7.5f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.3f, 4.25f, 4.f);

	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//render main door
	modelStack.PushMatrix();
	modelStack.Translate(door[0].position.x, door[0].position.y, door[0].position.z);
	modelStack.Rotate(door[0].rotation, 0, 1, 0);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Translate(door[0].width * 0.5f, 0.f, 0.f);
	modelStack.Scale(door[0].width, door[0].height, 0.2f);

	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.5f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	RenderMesh(meshList[GEO_DOOR], true);
	modelStack.PopMatrix();


	// ------------------------------------------------------------------
	// COUNTER (PARENT)
	//   World position: centred X, Y=0.5, Z=1.5
	//   Children inherit this transform before applying their own offset.
	// ------------------------------------------------------------------
	modelStack.PushMatrix();                    // >>> COUNTER PARENT
	modelStack.Translate(0.f, 0.5f, 1.5f);     // counter world position
	modelStack.Scale(1.f, 2.f, 2.5f);

	// Render counter itself
	meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
	meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
	meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
	meshList[GEO_COUNTER]->material.kShininess = 8.f;
	RenderMesh(meshList[GEO_COUNTER], true);

	modelStack.PopMatrix();                     // <<< COUNTER PARENT

	// ---- BOMB (CHILD of Counter) ----
	// Offset: +0.5 in Y (sits on top of counter surface),
	//         -7 in X (left end of counter)
	// No extra Z needed – inherits counter's Z=1.5
	modelStack.PushMatrix();                // >>> BOMB CHILD
	modelStack.Translate(-7.f, 1.8f, 1.5f);
	modelStack.Scale(1.f, 1.f, 1.f);

	meshList[GEO_BOMB]->material.kAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_BOMB]->material.kDiffuse = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BOMB]->material.kSpecular = glm::vec3(0.4f, 0.4f, 0.4f);
	meshList[GEO_BOMB]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_BOMB], true);
	modelStack.PopMatrix();                 // <<< BOMB CHILD



	// ------------------------------------------------------------------
	// TARGET RAIL (PARENT)
	//   World position: centred X, Y=3.5, Z=-5  (above counter, back wall)
	//   The cylinder's long axis is vertical by default so we rotate 90°
	//   around Z to lay it horizontally along X.
	//   All targets are children – their X offset is relative to the
	//   rail's centre, so moving the rail moves all targets with it.
	// ------------------------------------------------------------------
	modelStack.PushMatrix();                    // >>> RAIL PARENT
	modelStack.Translate(0.f, 4.3f, -5.0f);    // rail world position
	modelStack.Rotate(90.f, 0.f, 0.f, 1.f);    // lay cylinder along X axis
	modelStack.Scale(1.f, 1.1f, 1.f);

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

		modelStack.PushMatrix();
		modelStack.Translate(-1.1f, 0.f, 0.f);   
		modelStack.Rotate(-90.f, 0.f, 0.f, 1.f);   // undo rail rotation

		if (!targets[i].isFalling)
		{
			// Normal: local X offset on rail, Y=0 (on rail), Z=0
			modelStack.Translate(targets[i].physics.pos.x, 0.f, 0.f);
		}
		else
		{
			// Falling: use full local pos (Y drops via physics)
			modelStack.Translate(
				targets[i].physics.pos.x,
				targets[i].physics.pos.y,
				0.f
			);
		}

		modelStack.Rotate(90.0f, 0, 1, 0);
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
		modelStack.Rotate(-90.f, 0.f, 0.f, 1.f);	// lay flat
		modelStack.Rotate(-20.f, 0.f, 1.f, 0.f);   // fix tilt/spin to face correct direction
		modelStack.Scale(0.02f, 0.02f, 0.02f);

		meshList[GEO_GUN]->material.kAmbient = glm::vec3(0.15f, 0.1f, 0.1f);
		meshList[GEO_GUN]->material.kDiffuse = glm::vec3(0.6f, 0.55f, 0.5f);
		meshList[GEO_GUN]->material.kSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_GUN]->material.kShininess = 20.f;
		RenderMesh(meshList[GEO_GUN], true);
		modelStack.PopMatrix();                     // <<< GUN
	}

	// Gun held in hand (only after pickup) 
	if (gunPickedUp)
	{
		glm::vec3 view = glm::normalize(camera.target - camera.position);
		glm::vec3 right = glm::normalize(glm::cross(view, glm::vec3(0, 1, 0)));
		glm::vec3 up = glm::normalize(glm::cross(right, view));

		glm::vec3 gunPos = camera.position
			+ view * 1.1f    // how far in front of camera
			+ right * 0.9f    // offset to the right (like holding in right hand)
			+ up * (-0.2f); // offset downward

		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 cameraBasis = glm::mat4(
			glm::vec4(right, 0.f),
			glm::vec4(up, 0.f),
			glm::vec4(-view, 0.f),
			glm::vec4(gunPos, 1.f)
		);

		modelStack.PushMatrix();
		modelStack.LoadIdentity();
		modelStack.LoadMatrix(cameraBasis);
		modelStack.Scale(0.02f, 0.02f, 0.02f);
		modelStack.Rotate(180.f, 0.f, 1.f, 0.f); // flip to face forward
		modelStack.Rotate(-25.f, 0.f, 0.f, 1.f);	// tilt up slightly for better visibility
		modelStack.Rotate(-25.f, 0.f, 1.f, 0.f);	// fix spin to face correct direction

		meshList[GEO_GUN]->material.kAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
		meshList[GEO_GUN]->material.kDiffuse = glm::vec3(0.6f, 0.6f, 0.6f);
		meshList[GEO_GUN]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		meshList[GEO_GUN]->material.kShininess = 32.f;
		RenderMesh(meshList[GEO_GUN], true);
		modelStack.PopMatrix();
	}


	// ---- BULLETS (outside booth root, independent world objects) ----
	for (int i = 0; i < POOL_SIZE; ++i)
	{
		if (!bulletPool[i].active) continue;

		modelStack.PushMatrix();
		modelStack.Translate(
			bulletPool[i].physics.pos.x,
			bulletPool[i].physics.pos.y,
			bulletPool[i].physics.pos.z
		);
		modelStack.Scale(0.1f, 0.1f, 0.1f);  // small sphere

		meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(0.8f, 0.6f, 0.0f);  // gold-ish
		meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(1.0f, 0.8f, 0.0f);
		meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(1.0f, 1.0f, 0.5f);
		meshList[GEO_SPHERE]->material.kShininess = 32.f;
		RenderMesh(meshList[GEO_SPHERE], true);
		modelStack.PopMatrix();
	}

	
	// ------------------------------------------------------------------
	// HUD & SCREEN EFFECTS

	// ---- MUZZLE FLASH ----
	// Full-screen white quad flash on shoot
	if (muzzleFlashTimer > 0.f)
	{
		RenderMeshOnScreen(meshList[GEO_GUI], 960.f, 540.f, 1920.f, 1080.f);
	}

	// Door interaction prompts
	if (showInteractPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to exit", glm::vec3(1.f, 1.f, 1.f), 40, 0, 50);
	else if (showLockedPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "You need to win the game first!", glm::vec3(1.f, 0.f, 0.f), 40, 0, 50);

	// ---- HUD: FIND GUN STATE ----
	if (gameState == STATE_FIND_GUN)
	{
		if (!gunPickedUp)
		{
			RenderTextOnScreen(meshList[GEO_TEXT],
				"Find the gun!", glm::vec3(1, 1, 0), 30.f, 300.f, 540.f);

			if (IsPlayerNearGun(2.5f))
				RenderTextOnScreen(meshList[GEO_TEXT],
					"[F] Pick up Gun", glm::vec3(1, 1, 1), 30.f, 280.f, 490.f);
		}
		else
		{
			RenderTextOnScreen(meshList[GEO_TEXT],
				"Go to the booth!", glm::vec3(1, 1, 0), 30.f, 280.f, 540.f);

			if (IsPlayerNearBooth(2.5f))
				RenderTextOnScreen(meshList[GEO_TEXT],
					"[F] Start Game", glm::vec3(1, 1, 1), 30.f, 300.f, 490.f);
		}
	}

	// ---- HUD: PLAYING STATE ----
	if (gameState == STATE_PLAYING)
	{
		// Bomb timer – format as M:SS
		int   minutes = (int)(bombTimer / 60.f);
		int   seconds = (int)(bombTimer) % 60;
		char  timerBuf[32];
		sprintf_s(timerBuf, "TIME: %d:%02d", minutes, seconds);

		// Turn red when under 30 seconds
		glm::vec3 timerColor = (bombTimer <= 30.f)
			? glm::vec3(1, 0, 0)
			: glm::vec3(1, 1, 1);

		RenderTextOnScreen(meshList[GEO_TEXT],
			timerBuf, timerColor, 30.f, 30.f, 80.f);

		// Bullets
		char bulletBuf[32];
		sprintf_s(bulletBuf, "BULLETS: %d / %d", bulletsLeft, MAX_BULLETS);
		RenderTextOnScreen(meshList[GEO_TEXT], bulletBuf, glm::vec3(1, 1, 1), 30.f, 30.f, 520.f);

		// Targets hit
		char hitBuf[32];
		sprintf_s(hitBuf, "HITS: %d / %d", targetsHit, NUM_TARGETS);
		RenderTextOnScreen(meshList[GEO_TEXT], hitBuf, glm::vec3(1, 1, 1), 30.f, 30.f, 480.f);

		// Crosshair – centre of screen
		RenderTextOnScreen(meshList[GEO_TEXT], "+", glm::vec3(1, 1, 1), 40.f, 390.f, 285.f);
	}

	// ---- WIN SCREEN ----
	if (gameState == STATE_WON)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"BOMB DEFUSED!", glm::vec3(0, 1, 0), 60.f, 200.f, 340.f);
		RenderTextOnScreen(meshList[GEO_TEXT],
			"Head back to the lobby!", glm::vec3(1, 1, 1), 30.f, 200.f, 290.f);
	}

	// ---- LOSE SCREEN ----
	if (gameState == STATE_LOST)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"BOOM. YOU FAILED.", glm::vec3(1, 0, 0), 60.f, 180.f, 340.f);
		RenderTextOnScreen(meshList[GEO_TEXT],
			"The booth is gone.", glm::vec3(1, 1, 1), 30.f, 230.f, 290.f);
		RenderTextOnScreen(meshList[GEO_TEXT],
			"[R] Try Again", glm::vec3(1, 1, 0), 35.f, 300.f, 250.f);
	}



	// ---- DEBUG: Draw collision box outlines ----
	for (const AABB& box : collisionBoxes)
	{
		modelStack.PushMatrix();

		glm::vec3 center = (box.min + box.max) * 0.5f;
		glm::vec3 size = box.max - box.min;

		modelStack.Translate(center.x, center.y, center.z);
		modelStack.Scale(size.x, size.y, size.z);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderMesh(meshList[GEO_CUBE], false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		modelStack.PopMatrix();
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



// --------------------------------------------------------------
// IsPlayerNearGun()
// Simple distance check between camera and gun world position
// --------------------------------------------------------------
//bool SceneShooting::IsPlayerNearGun(float radius)
//{
//    glm::vec3 diff = camera.position - gunWorldPos;
//    float distSq   = glm::dot(diff, diff);
//    return distSq <= (radius * radius);
//}
bool SceneShooting::IsPlayerNearGun(float radius)
{
	float dx = camera.position.x - gunWorldPos.x;
	float dy = camera.position.y - gunWorldPos.y;
	float dz = camera.position.z - gunWorldPos.z;
	float distSq = dx * dx + dy * dy + dz * dz;
	return distSq <= (radius * radius);
}

// --------------------------------------------------------------
// IsPlayerNearBooth()
// Checks if player is within radius of booth entry point (in front of counter)
// --------------------------------------------------------------
bool SceneShooting::IsPlayerNearBooth(float radius)
{
	float dx = camera.position.x - boothEntryPos.x;
	float dy = camera.position.y - boothEntryPos.y;
	float dz = camera.position.z - boothEntryPos.z;
	return (dx * dx + dy * dy + dz * dz) <= (radius * radius);
}



// --------------------------------------------------------------
// Shoot()
// Fires one bullet. Checks all alive targets for a hit.
// Triggers muzzle flash. Checks lose condition (out of bullets).
// --------------------------------------------------------------
void SceneShooting::Shoot()
{
	if (bulletsLeft <= 0) return;

	// Find inactive bullet slot
	int slot = -1;
	for (int i = 0; i < POOL_SIZE; ++i)
	{
		if (!bulletPool[i].active) { slot = i; break; }
	}
	if (slot == -1) return;

	bulletsLeft--;
	muzzleFlashTimer = 0.08f;

	// Fire from fixed shooting position (camera is locked here)
	bulletPool[slot].active = true;
	bulletPool[slot].physics.pos = Vector3(
		shootingPos.x,
		shootingPos.y,
		shootingPos.z
	);

	// Direction: from shooting position toward fixed target point on rail
	// Use camera.target so the crosshair always matches where bullet goes
	//glm::vec3 forward = glm::normalize(shootingTarget - shootingPos);

	// Direction: fire toward where the player is actually looking
	glm::vec3 forward = glm::normalize(camera.target - camera.position);

	float bulletSpeed = 80.f;  // fast enough to travel straight
	bulletPool[slot].physics.vel = Vector3(
		forward.x * bulletSpeed,
		forward.y * bulletSpeed,
		forward.z * bulletSpeed
	);

	// Reset accel – low gravity (set in Init, restore here just in case)
	bulletPool[slot].physics.accel = Vector3(0.f, -2.0f, 0.f);
}

// --------------------------------------------------------------
// ResetGame()
// Restores everything to starting state.
// Called if you want an in-scene restart (optional).
// --------------------------------------------------------------
void SceneShooting::ResetGame()
{
	gameState = STATE_FIND_GUN;
	bulletsLeft = MAX_BULLETS;
	targetsHit = 0;
	bombTimer = 120.0f;
	gunPickedUp = false;
	fps = 0.f;
	muzzleFlashTimer = 0.f;
	playerLocked = false;
	glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	atBooth = false;
	boothEntryPos = glm::vec3(0.f, 2.1f, 2.f);  // front of counter

	// Fixed shooting position - centred behind counter (matches Init)
	shootingPos = glm::vec3(0.f, 2.1f, 5.f);
	shootingTarget = glm::vec3(0.f, 3.5f, -5.f);  // looking at rail

	// Reset gun (matches Init position)
	gunWorldPos = glm::vec3(7.f, 0.6f, 5.f);

	// Reset targets
	//float startPositions[NUM_TARGETS] = { -6.f, -3.f, 0.f, 3.f, 6.f };
	float speed = 3.0f;
	for (int i = 0; i < NUM_TARGETS; ++i)
	{
		float startX = -6.f + (i * 3.f);   // -6, -3, 0, 3, 6
		targets[i].physics.pos = Vector3(startX, 0.f, 0.f);
		targets[i].physics.vel = Vector3(0.f, 0.f, 0.f);
		targets[i].physics.accel = Vector3(0.f, 0.f, 0.f);
		targets[i].physics.mass = 1.f;
		targets[i].physics.bounciness = 0.f;
		targets[i].speed = speed;
		targets[i].minX = -8.f;
		targets[i].maxX = 8.f;
		targets[i].isAlive = true;
		targets[i].isFalling = false;
	}

	// Reset bullet pool
	for (int i = 0; i < POOL_SIZE; ++i)
	{
		bulletPool[i].active = false;
		bulletPool[i].physics.pos = Vector3(0.f, 0.f, 0.f);
		bulletPool[i].physics.vel = Vector3(0.f, 0.f, 0.f);
		bulletPool[i].physics.accel = Vector3(0.f, -2.0f, 0.f);
		bulletPool[i].physics.mass = 0.1f;
		bulletPool[i].physics.bounciness = 0.f;
	}

	// Unlock camera, send player back to start
	camera.Init(
		glm::vec3(0.f, 2.1f, 6.f),
		glm::vec3(0.f, 2.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f)
	);
}


// ---------------------------------------------------------------
// CheckAABBCollision
// Sphere-vs-AABB collision: clamps pos to nearest box point,
// returns true if that distance is less than radius.
// ---------------------------------------------------------------
bool SceneShooting::CheckAABBCollision(const glm::vec3& pos, float radius, const AABB& box)
{
	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(pos.x, box.min.x, box.max.x);
	closestPoint.y = glm::clamp(pos.y, box.min.y, box.max.y);
	closestPoint.z = glm::clamp(pos.z, box.min.z, box.max.z);
	float distance = glm::distance(closestPoint, pos);
	return distance < radius;
}

// ---------------------------------------------------------------
// BuildCollisionBoxes
// Populates collisionBoxes with AABB volumes matching the booth
// geometry (floor, walls, counter). Tweak min/max to fit your scene.
// ---------------------------------------------------------------
void SceneShooting::BuildCollisionBoxes()
{
	collisionBoxes.clear();

	// Floor
	AABB floor;
	floor.min = glm::vec3(-35.f, 0.f, -7.5f);
	floor.max = glm::vec3(10.f, 1.5f, 14.5f);
	collisionBoxes.push_back(floor);

	// Back-of-scene wall (behind targets)
	AABB backSceneWall;
	backSceneWall.min = glm::vec3(-10.f, -0.75f, -7.6f);
	backSceneWall.max = glm::vec3(10.f, 8.f, -7.4f);
	collisionBoxes.push_back(backSceneWall);

	// Right wall
	AABB rightWall;
	rightWall.min = glm::vec3(9.5f, -0.75f, -7.5f);
	rightWall.max = glm::vec3(10.f, 8.f, 14.5f);
	collisionBoxes.push_back(rightWall);

	// Left wall 
	AABB leftWallFront;
	leftWallFront.min = glm::vec3(-10.f, -0.75f, -7.5f);
	leftWallFront.max = glm::vec3(-9.5f, 8.f, 14.5f);
	collisionBoxes.push_back(leftWallFront);

	// Front Wall
	// Front wall L  (render: center (5.5, 4, 7.5), scale (9, 8, 0.3))
	// X: 1.0 to 10.0,  Y: 0 to 8,  Z: 7.35 to 7.65
	AABB frontWallL;
	frontWallL.min = glm::vec3(1.0f, 0.f, 7.35f);
	frontWallL.max = glm::vec3(10.0f, 8.f, 7.65f);
	collisionBoxes.push_back(frontWallL);

	// Front wall R  (render: center (-5.5, 4, 7.5), scale (9, 8, 0.3))
	// X: -10.0 to -1.0,  Y: 0 to 8,  Z: 7.35 to 7.65
	AABB frontWallR;
	frontWallR.min = glm::vec3(-10.0f, 0.f, 7.35f);
	frontWallR.max = glm::vec3(-1.0f, 8.f, 7.65f);
	collisionBoxes.push_back(frontWallR);

	// Counter
	AABB counter;
	counter.min = glm::vec3(-10.f, -2.f, 1.0f);
	counter.max = glm::vec3(10.f, 2.f, 2.5f);
	collisionBoxes.push_back(counter);

	
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









	// --- F key: step up to booth and start game ---
	if (KeyboardController::GetInstance()->IsKeyPressed('F'))
	{// Step 1: pick up gun
		if (!gunPickedUp && IsPlayerNearGun(2.5f))
		{
			gunPickedUp = true;
			// just picks up gun, player still walks freely
		}

		// Step 2: walk to counter and press F to start
		else if (gunPickedUp && !atBooth && IsPlayerNearBooth(2.5f))
		{
			atBooth = true;
			playerLocked = true;
			glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hide cursor
			camera.Init(shootingPos, shootingTarget, glm::vec3(0.f, 1.f, 0.f));
			gameState = STATE_PLAYING;
			bombTimer = 120.0f;
		}
	}

	// --- R key: restart at booth on lose ---
	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		if (gameState == STATE_LOST)
		{
			ResetGame();  // resets everything, stays in SceneShooting
		}

		// On win, R is not needed – player walks freely back to lobby
	}



}

void SceneShooting::HandleMouseInput() {
	static bool wasLeftDown = false;

	bool isLeftDown = MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT);

	// Shoot on left click PRESS (not hold)
	if (isLeftDown && !wasLeftDown)
	{
		if (gameState == STATE_PLAYING)
			Shoot();
	}

	wasLeftDown = isLeftDown;
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