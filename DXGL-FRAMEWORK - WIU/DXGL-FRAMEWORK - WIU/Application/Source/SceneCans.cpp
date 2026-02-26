#include "SceneCans.h"
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

SceneCans::SceneCans()
{
}

SceneCans::~SceneCans()
{
}

void SceneCans::Init()
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

	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// Initialise camera properties
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

	meshList[GEO_DOOR] = MeshBuilder::GenerateCube("Door", glm::vec3(1.f, 1.f, 1.f), 1.f);

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

	// OBJ Models
	meshList[GEO_CAN] = MeshBuilder::GenerateOBJMTL("Can", "Models//can.obj", "Models//can.mtl");
	meshList[GEO_CAN]->textureID = LoadTGA("Images//can.tga");

	meshList[GEO_BALL] = MeshBuilder::GenerateOBJMTL("Ball", "Models//ball.obj", "Models//ball.mtl");
	meshList[GEO_BALL]->textureID = LoadTGA("Images//ball.tga");

	meshList[GEO_BOMB] = MeshBuilder::GenerateOBJMTL("Bomb", "Models//ball.obj", "Models//ball.mtl");
	meshList[GEO_BOMB]->textureID = LoadTGA("Images//bomb.tga");

	// Environment
	meshList[GEO_FLOOR] = MeshBuilder::GenerateRectangularPrism("Floor", glm::vec3(0.45f, 0.32f, 0.18f), 20.f, 0.2f, 15.f);
	meshList[GEO_CEILING] = MeshBuilder::GenerateRectangularPrism("Ceiling", glm::vec3(0.85f, 0.75f, 0.55f), 20.f, 0.2f, 15.f);
	meshList[GEO_CEILING]->textureID = LoadTGA("Images//carnivalwallpaper2.tga");

	meshList[GEO_WALL] = MeshBuilder::GenerateRectangularPrism("Wall", glm::vec3(0.9f, 0.85f, 0.6f),1.f, 1.f, 1.f);   
	meshList[GEO_WALL]->textureID = LoadTGA("Images//carnivalwallpaper.tga");

	meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism("Counter", glm::vec3(0.55f, 0.35f, 0.15f), 20.f, 1.0f, 0.4f);

	meshList[GEO_TABLE] = MeshBuilder::GenerateOBJMTL("Table", "Models//table.obj", "Models//table.mtl");
	meshList[GEO_TABLE]->textureID = LoadTGA("Images//table.tga");


	// In Init() — change 4.0f/3.0f -> 16.0f/9.0f (or 1920.0f/1080.0f)
	glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	// Player collision box size (width, height, depth)
	playerSize = glm::vec3(0.4f, 1.8f, 0.4f);


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

	//initialise door 
	door[0] = { glm::vec3(1.f, 2.f, 14.5f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };
	door[1] = { glm::vec3(-10.f, 2.f, 6.25f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };
	doorCollisionIdx[0] = -1;
	doorCollisionIdx[1] = -1;

	//GAME SETUP
	InitialiseCans();
	InitialiseBalls();
	BuildCollisionBoxes();
	gameState = GAME_NOT_STARTED;
	SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_CANS] = false;
	bombTimer = 60.0f;

	// Sync door collision boxes to open/closed state
	// Door is "open enough" to walk through when rotation exceeds ~70 degrees
	auto UpdateDoorAABB = [&](int doorIdx, int colIdx, bool isLeftWallDoor)
		{
			if (colIdx < 0 || colIdx >= (int)collisionBoxes.size()) return;

			bool isOpen = (door[doorIdx].rotation < 5.f);

			if (isOpen)
			{
				// Collapse the AABB so it blocks nothing
				collisionBoxes[colIdx].min = glm::vec3(0.f);
				collisionBoxes[colIdx].max = glm::vec3(0.f);
			}
			else
			{
				if (!isLeftWallDoor)
				{
					// Door[0]: back wall
					collisionBoxes[colIdx].min = glm::vec3(-0.1f, 0.f, 14.3f);
					collisionBoxes[colIdx].max = glm::vec3(2.1f, 4.f, 14.7f);
				}
				else
				{
					// Door[1]: left wall
					collisionBoxes[colIdx].min = glm::vec3(-10.2f, 0.f, 5.25f);
					collisionBoxes[colIdx].max = glm::vec3(-9.8f, 4.f, 7.25f);
				}
			}
		};

	UpdateDoorAABB(0, doorCollisionIdx[0], false);
	UpdateDoorAABB(1, doorCollisionIdx[1], true);
}





void SceneCans::Update(double dt)
{
	HandleKeyPress();
	HandleMouseInput();

	// Store position and target before camera update
	glm::vec3 oldPos = camera.position;
	glm::vec3 oldTarget = camera.target;
	
	// Only update camera movement when not aiming
	if (!m_isAiming)
		camera.Update(dt);

	// if collided, revert camera pos
	glm::vec3 updatedPos = camera.position;

	// Resolve Vertical Collision: floor/Ceiling
	camera.position = glm::vec3(oldPos.x, updatedPos.y, oldPos.z);
	for (const AABB& box : collisionBoxes)
	{
		if (CheckAABBCollision(camera.position, 0.3f, box))
		{
			//if hit smth vertically, go back to old Y
			camera.position.y = oldPos.y;
			camera.target.y = oldTarget.y;
			break;
		}
	}

	//Resolve Horizontal Collision 
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
		

	int flyingBall = m_currentBall - 1;
	bool ballInFlight = (flyingBall >= 0 && m_balls[flyingBall].inAir);

	// === ANIMATION/INTERACTIONS ====
	//Door interaction
	showDoorInteractPrompt = false;
	if (door[0].IsPlayerNear(camera.position, 2.5f))
	{
		if (SceneManager::GetInstance()->getIsGameCompleted(SceneManager::SCENE_CANS))
			showDoorInteractPrompt = true;
			
		else 
			RenderTextOnScreen(meshList[GEO_TEXT], "You need to win the game first!", glm::vec3(1.f, 0.f, 0.f), 40, 50, 50);
	}
	if (showDoorInteractPrompt && KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		door[0].Open();
	}

	if (door[0].Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f))
	{
		SceneManager::GetInstance()->SwitchScene(door[0].leadsTo);
		door[0].Close();
		showDoorInteractPrompt = false;
	}

	//side door
	if (door[1].IsPlayerNear(camera.position, 2.5f))
	{
		door[1].Open();
		door[1].Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f);
	}

	
	//check ball collection
	showPickupPrompt = false;
	if (!ballCollected && RayHitsBall(2, 3.f))
	{
		showPickupPrompt = true;

		if (KeyboardController::GetInstance()->IsKeyPressed('F'))
		{
			ballCollected = true;
			m_throwsLeft++;     
			m_noOfBalls++;
			gameState = GAME_PLAYING;
			showPickupPrompt = false;
		}
	}

	//game logic
	if (gameState == GAME_PLAYING)
	{

		//bombtimer
		bombTimer -= static_cast<float>(dt);
		if (bombTimer <= 0.f)
		{
			bombTimer = 0.f;
			gameState = GAME_LOST;
		}

		//ball vs can collision
		if (m_balls[0].inAir)
		{
			UpdateBall(static_cast<float>(dt));
			CheckBallCanCollisions(0);
			CheckFloorCollisions(0);
		}

		UpdateCans(static_cast<float>(dt));
		CheckCanCanCollisions();
		CheckCanSupports();

		//check if all cans knocked
		if (m_throwsLeft == 0 && !m_balls[0].inAir)
		{
			int knocked = 0;
			for (int i = 0; i < NUM_CANS; ++i)
				if (m_cans[i].knocked) knocked++;

			if (knocked == NUM_CANS)
			{
				gameState = GAME_WON;
				SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_CANS] = true;
			}
			else 
			{
				gameState = GAME_LOST;
			}
		}
		
	}

	// --- Booth interaction to enter aiming mode ---
	bool nearBooth = glm::length(camera.position - glm::vec3(0.f, 2.f, 5.f)) < 2.5f;

	if (nearBooth && ballCollected && gameState == GAME_PLAYING && !m_isAiming)
	{
		showBoothPrompt = true;
		if (KeyboardController::GetInstance()->IsKeyPressed('F'))
		{
			// Save current camera state
			m_savedCamPos = camera.position;
			m_savedCamTarget = camera.target;
			m_savedCamUp = camera.up;

			// Snap to aiming position
			camera.position = AIM_CAM_POS;
			camera.target = AIM_CAM_TARGET;
			camera.up = glm::vec3(0.f, 1.f, 0.f);

			// Reset aim to point roughly at the cans
			m_aimPitch = 75.f;
			m_aimDir = glm::normalize(glm::vec3(
				cos(glm::radians(m_aimPitch)), sin(glm::radians(m_aimPitch)), 0.f
			));

			// Switch to spotlight aimed at the can table
			light[0].type = Light::LIGHT_SPOT;
			light[0].position = glm::vec3(0.f, 8.f, -4.f);   // above and behind player
			light[0].spotDirection = glm::normalize(glm::vec3(0.f, -1.f, -1.f)); // aimed at cans
			light[0].cosCutoff = 25.f;   // tight cone
			light[0].cosInner = 15.f;
			light[0].power = 3.f;    // brighter for dramatic effect
			glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
			glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], cosf(glm::radians(light[0].cosCutoff)));
			glUniform1f(m_parameters[U_LIGHT0_COSINNER], cosf(glm::radians(light[0].cosInner)));

			m_isAiming = true;
		}
	}
	else
	{
		showBoothPrompt = false;
	}
	

	
}

void SceneCans::Render()
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

	// Render light
	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	modelStack.Scale(0.1f, 0.1f, 0.1f);
	RenderMesh(meshList[GEO_SPHERE], false);
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
	
	//side door
	modelStack.PushMatrix();
	modelStack.Translate(door[1].position.x, door[1].position.y, door[1].position.z);
	modelStack.Rotate(door[1].rotation + 90.f, 0, 1, 0);   
	modelStack.Translate(door[1].width * 0.5f, 0.f, 0.f);  //shift door right
	modelStack.Scale(door[1].width, door[1].height, 0.2f);
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.5f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	RenderMesh(meshList[GEO_DOOR], true);
	modelStack.PopMatrix();

	if (showDoorInteractPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to enter", glm::vec3(1.f, 1.f, 0.f), 40, 50, 50);


	//environment
	modelStack.PushMatrix();                     
	modelStack.Translate(0.f, 0.f, 0.f);          

	modelStack.PushMatrix();                  
	modelStack.Translate(-12.f, 0.f, 3.5f);
	modelStack.Scale(2.25f, 1.f, 1.5f);
	meshList[GEO_FLOOR]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_FLOOR]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.2f);
	meshList[GEO_FLOOR]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FLOOR]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_FLOOR], true);
	modelStack.PopMatrix();                     

	modelStack.PushMatrix();                   
	modelStack.Translate(0.f, 8.f, 3.25f);
	modelStack.Scale(1.f, 1.f, 1.5f);
	meshList[GEO_CEILING]->material.kAmbient = glm::vec3(0.4f, 0.35f, 0.25f);
	meshList[GEO_CEILING]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.55f);
	meshList[GEO_CEILING]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_CEILING]->material.kShininess = 1.f;
	RenderMesh(meshList[GEO_CEILING], true);
	modelStack.PopMatrix();                     

	//front wall
	modelStack.PushMatrix();                    
	modelStack.Translate(0.f, 4.f, -7.5f);
	modelStack.Scale(20.f, 8.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();                     

	//left wall L
	modelStack.PushMatrix();                    
	modelStack.Translate(-10.f, 4.f, 10.5f);
	modelStack.Scale(0.3f, 8.f, 8.5f);
	RenderMesh(meshList[GEO_WALL], true);      
	modelStack.PopMatrix();   

	//left wall R
	modelStack.PushMatrix();
	modelStack.Translate(-10.f, 4.f, -1.75f);
	modelStack.Scale(0.3f, 8.f, 12.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//door frame
	modelStack.PushMatrix();
	modelStack.Translate(-10.f, 5.85f, 5.f);
	modelStack.Scale(0.3f, 4.25f, 5.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//right
	modelStack.PushMatrix();                  
	modelStack.Translate(10.f, 4.f, 3.5f);
	modelStack.Scale(0.3f, 8.f, 22.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();      

	//back wall L
	modelStack.PushMatrix();
	modelStack.Translate(5.5f, 4.f, 14.5f);
	modelStack.Scale(9.f, 8.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//back wall R
	modelStack.PushMatrix();
	modelStack.Translate(-5.5f, 4.f, 14.5f);
	modelStack.Scale(9.f, 8.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//door frame
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 5.9f, 14.5f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.3f, 4.25f, 4.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	

	int renderBall;

	if (ballCollected) renderBall = 0;
	else renderBall = 1;

	//COUNTER
	{
		//render counter
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.5f, 1.5f);
		modelStack.Scale(1.f, 2.5f, 2.5f);
		meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
		meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
		meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
		meshList[GEO_COUNTER]->material.kShininess = 8.f;
		RenderMesh(meshList[GEO_COUNTER], true);

		int render = 1;
		if (m_isAiming) render = 0;

		//only render ball if NOT in air
		if (!m_balls[0].inAir)
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_balls[0].ball.pos.x, m_balls[0].ball.pos.y, m_balls[0].ball.pos.z);
			modelStack.Scale(15.f, 6.f, 6.f);
			modelStack.Rotate(0, render, 0, 0);
			meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_BALL], true);
			modelStack.PopMatrix();
		}

		if (!m_balls[1].inAir)
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_balls[1].ball.pos.x, m_balls[1].ball.pos.y, m_balls[1].ball.pos.z);
			modelStack.Scale(15.f, 6.f, 6.f);
			modelStack.Rotate(0, render, 0, 0);
			meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_BALL], true);
			modelStack.PopMatrix();
		}

		if (!m_balls[2].inAir)
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_balls[2].ball.pos.x, m_balls[2].ball.pos.y, m_balls[2].ball.pos.z);
			modelStack.Scale(15.f, 6.f, 6.f);
			modelStack.Rotate(0, render, 0, 0);
			meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_BALL], true);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
	}

	//render missing ball
	modelStack.PushMatrix();
	modelStack.Translate(m_balls[3].ball.pos.x, m_balls[3].ball.pos.y, m_balls[3].ball.pos.z);
	modelStack.Scale(17.f, 17.f, 17.f);
	modelStack.Rotate(0, renderBall, 0, 0);
	meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	RenderMesh(meshList[GEO_BALL], true);
	modelStack.PopMatrix();

	//flying ball
	if (m_balls[0].inAir)
	{
		modelStack.PushMatrix();
		modelStack.Translate(m_balls[0].ball.pos.x, m_balls[0].ball.pos.y, m_balls[0].ball.pos.z);
		modelStack.Scale(0.15f, 0.15f, 0.15f);
		meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
	}

	//render ball on screen, looks like player is holding it
	if (ballCollected && !m_isAiming)
	{
		int holdBall = 1;
		if (m_noOfBalls == 0) holdBall = 0;

		glm::vec3 view, right, up, ballPos;

		if (m_isAiming)
		{
			// Lock ball to bottom-left of aim camera view
			view = glm::normalize(AIM_CAM_TARGET - AIM_CAM_POS);
			right = glm::normalize(glm::cross(view, glm::vec3(0, 1, 0)));
			up = glm::normalize(glm::cross(right, view));
			ballPos = AIM_CAM_POS + view * 1.5f + right * (-0.5f) + up * (-0.5f); // left side
		}
		else
		{
			// Original walking hold position (right side)
			view = glm::normalize(camera.target - camera.position);
			right = glm::normalize(glm::cross(view, glm::vec3(0, 1, 0)));
			up = glm::normalize(glm::cross(right, view));
			ballPos = camera.position + view * 1.5f + right * 0.5f + up * (-0.5f);
		}

		glm::mat4 cameraBasis = glm::mat4(
			glm::vec4(right, 0.f),
			glm::vec4(up, 0.f),
			glm::vec4(-view, 0.f),
			glm::vec4(ballPos, 1.f)
		);

		modelStack.PushMatrix();
		modelStack.LoadIdentity();
		modelStack.LoadMatrix(cameraBasis);
		modelStack.Scale(16.f, 16.f, 16.f);
		modelStack.Rotate(0, holdBall, 0, 0);
		meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();
	}
	


	//MAIN TABLE 
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.f, -4.f);
		modelStack.Scale(1.15f, 1.15f, 1.15f);
		meshList[GEO_TABLE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
		meshList[GEO_TABLE]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
		meshList[GEO_TABLE]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
		meshList[GEO_TABLE]->material.kShininess = 8.f;
		RenderMesh(meshList[GEO_TABLE], true);

		//render cans
		for (int i = 0; i < NUM_CANS; ++i)
		{
			if (!m_cans[i].active) continue;
			modelStack.PushMatrix();
			modelStack.Translate(m_cans[i].can.pos.x, m_cans[i].can.pos.y + 0.75f, m_cans[i].can.pos.z - 1.5f);

			if (m_cans[i].knocked)
				modelStack.Rotate(90.f, 1.f, 0.f, 0.f);   // tip over

			modelStack.Scale(0.25f, 0.25f, 0.25f);
			meshList[GEO_CAN]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_CAN]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_CAN]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_CAN], true);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();
	}
	


	//side static tables
	{
		//left
		modelStack.PushMatrix();
		modelStack.Translate(-7.f, 0.f, -4.f);
		modelStack.Scale(1.f, 1.f, 1.f);
		meshList[GEO_TABLE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
		meshList[GEO_TABLE]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
		meshList[GEO_TABLE]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
		meshList[GEO_TABLE]->material.kShininess = 8.f;
		RenderMesh(meshList[GEO_TABLE], true);

		//render cans
		for (int i = 0; i < NUM_CANS; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_staticCanPos[i].x,m_staticCanPos[i].y + 0.75f,m_staticCanPos[i].z - 1.5f);
			modelStack.Scale(0.25f, 0.25f, 0.25f);
			meshList[GEO_CAN]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_CAN]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_CAN]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_CAN], true);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();


		//right
		modelStack.PushMatrix();
		modelStack.Translate(7.f, 0.f, -4.f);
		modelStack.Scale(1.f, 1.f, 1.f);
		meshList[GEO_TABLE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
		meshList[GEO_TABLE]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
		meshList[GEO_TABLE]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
		meshList[GEO_TABLE]->material.kShininess = 8.f;
		RenderMesh(meshList[GEO_TABLE], true);

		//render cans
		for (int i = 0; i < NUM_CANS; ++i)
		{
			modelStack.PushMatrix();
			modelStack.Translate(m_staticCanPos[i].x, m_staticCanPos[i].y + 0.75f, m_staticCanPos[i].z - 1.5f);
			modelStack.Scale(0.25f, 0.25f, 0.25f);
			meshList[GEO_CAN]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_CAN]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_CAN]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_CAN], true);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();

	}

	bool ballInFlight = m_balls[0].inAir;
	// Only show aim line when game is active and ball not in air
	if (gameState == GAME_PLAYING && !ballInFlight)
		DrawAimLine();

	/*if (!m_isAiming)
		DrawRayCastLine();*/

	//RenderColDebug(0);
	RenderHUD();
}

void SceneCans::RenderMesh(Mesh* mesh, bool enableLight)
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


void SceneCans::RenderSkybox() {
	modelStack.PushMatrix();

	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, -50.f);

	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

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



void SceneCans::RenderMeshOnScreen(Mesh* mesh, float x, float
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






void SceneCans::Exit()
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

void SceneCans::HandleKeyPress()
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

	// --- R key: restart (only on win or lose screen) ---
	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		ResetGame();
	}
}

void SceneCans::HandleMouseInput() 
{

	if (!m_isAiming) return;

	static bool wasDown = false;
	bool isDown = MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT);

	if (isDown && !wasDown)
	{
		wasDown = true;
		if (gameState == GAME_PLAYING && m_throwsLeft > 0 && !m_balls[0].inAir)
		{
			LaunchBall();
			camera.position = m_savedCamPos;
			camera.target = m_savedCamTarget;
			camera.up = m_savedCamUp;

			// Restore point light
			light[0].type = Light::LIGHT_POINT;
			light[0].position = glm::vec3(0.f, 5.f, 0.f);
			light[0].power = 1.f;
			glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
			glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);

			m_isAiming = false;
		}
	}
	wasDown = isDown;

}



void SceneCans::RenderText(Mesh* mesh, std::string text, glm::vec3
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
			glm::vec3(0.2f + i * 0.6f, 0.f, 0)
		);
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



void SceneCans::RenderTextOnScreen(Mesh* mesh, std::string
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
			glm::vec3(0.2f + i * 0.6f, 0.f, 0)
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

void SceneCans::RenderColDebug(int ballIdx)
{
	glm::vec3 canHalfExtents(0.3f, 0.4f, 0.3f);  // match your CheckBallCanCollisions values
	float ballRadius = 0.05f;                          // match your CheckBallCanCollisions value

	// Draw each can's AABB as a wireframe box
	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].active) continue;

		glm::vec3 canPos(m_cans[i].colPos.x, m_cans[i].colPos.y, m_cans[i].colPos.z);

		modelStack.PushMatrix();
		modelStack.Translate(canPos.x, canPos.y, canPos.z);
		modelStack.Scale(canHalfExtents.x * 2.f,   // full width  = halfExtent * 2
			canHalfExtents.y * 2.f,   // full height
			canHalfExtents.z * 2.f);  // full depth

		// Green if upright, red if knocked
		glm::vec3 boxColor = m_cans[i].knocked ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
		meshList[GEO_CUBE]->material.kAmbient = boxColor;
		meshList[GEO_CUBE]->material.kDiffuse = boxColor;
		meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.f);
		meshList[GEO_CUBE]->material.kShininess = 1.f;

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderMesh(meshList[GEO_CUBE], false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		modelStack.PopMatrix();
	}

	// Draw ball collision sphere as a wireframe sphere
	if (ballIdx >= 0)
	{
		glm::vec3 ballPos(m_balls[ballIdx].ball.pos.x,
			m_balls[ballIdx].ball.pos.y,
			m_balls[ballIdx].ball.pos.z);

		modelStack.PushMatrix();
		modelStack.Translate(ballPos.x, ballPos.y, ballPos.z);
		modelStack.Scale(ballRadius * 2.f, ballRadius * 2.f, ballRadius * 2.f);

		meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(0.f, 0.f, 1.f);  // blue
		meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(0.f, 0.f, 1.f);
		meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.f);
		meshList[GEO_SPHERE]->material.kShininess = 1.f;

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderMesh(meshList[GEO_SPHERE], false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		modelStack.PopMatrix();
	}
}

void SceneCans::CheckCanSupports()
{
	bool anyChanged = true;
	int counter = 0;

	// Keep looping until no new cans fall this frame
	// (handles chain: bottom falls -> middle loses support -> top loses support)
	while (anyChanged && counter < NUM_CANS)
	{		
			anyChanged = false;
			counter++;

			for (int i = 0; i < NUM_CANS; ++i)
			{
				if (m_cans[i].knocked)           continue;
				if (m_cans[i].supportCount == 0) continue;

				bool hasSupport = false;
				for (int s = 0; s < m_cans[i].supportCount; ++s)
				{
					int supportIdx = m_cans[i].supportedBy[s];

					// Safety check — never access invalid index
					if (supportIdx < 0 || supportIdx >= NUM_CANS) continue;

					if (!m_cans[supportIdx].knocked)
					{
						hasSupport = true;
						break;
					}
				}

				if (!hasSupport)
				{
					m_cans[i].knocked = true;
					m_cans[i].can.vel.y = -1.5f;
					anyChanged = true;

					for (int s = 0; s < m_cans[i].supportCount; ++s)
					{
						int supportIdx = m_cans[i].supportedBy[s];
						if (supportIdx < 0 || supportIdx >= NUM_CANS) continue;

						if (m_cans[supportIdx].knocked)
						{
							m_cans[i].can.vel.x += m_cans[supportIdx].can.vel.x * 0.3f;
							m_cans[i].can.vel.z += m_cans[supportIdx].can.vel.z * 0.3f;
						}
					}
				}
			}
	}
}

bool SceneCans::RayHitsBall(int ballIndex, float maxDist)
{
	// Convert Vector3 ball pos to glm
	glm::vec3 ballPos(m_balls[3].ball.pos.x, m_balls[3].ball.pos.y, m_balls[3].ball.pos.z);

	// Ray from camera eye along look direction
	glm::vec3 rayOrigin = camera.position;
	glm::vec3 rayDir = glm::normalize(camera.target - camera.position);


	//ball must be within maxDist of the player at all
	float distToBall = glm::length(ballPos - rayOrigin);
	if (distToBall > maxDist) return false;

	//ball must be roughly infront of player -> dot = 1 means looking directly at it, 0 means 90 degrees away
	glm::vec3 dirToBall = glm::normalize(ballPos - rayOrigin);
	float dot = glm::dot(rayDir, dirToBall);
	if (dot < 0.85f) return false;


	// Ray-sphere intersection
	//from ball center to ray start
	glm::vec3 oc = rayOrigin - ballPos; 
	float b = glm::dot(oc, rayDir);
	float c = glm::dot(oc, oc) - (0.75f * 0.5f);  //pick-up sphere radius
	float discriminant = (b * b) - c;

	if (discriminant < 0.f) return false;  // ray misses sphere

	// Check the hit is within maxDist in front of camera
	float t = -b - sqrtf(discriminant);
	return (t > 0.f && t < maxDist);
}

bool SceneCans::CheckAABBCollision(const glm::vec3& pos, float radius, const AABB& box)
{
	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(pos.x, box.min.x, box.max.x);
	closestPoint.y = glm::clamp(pos.y, box.min.y, box.max.y);
	closestPoint.z = glm::clamp(pos.z, box.min.z, box.max.z);
	float distance = glm::distance(closestPoint, pos);
	return distance < radius;
}

void SceneCans::BuildCollisionBoxes()
{
	collisionBoxes.clear();

	// Floor
	AABB floor;
	floor.min = glm::vec3(-35.f, 0.f, -7.5f);
	floor.max = glm::vec3(10.f, 1.5f, 14.5f);
	collisionBoxes.push_back(floor);

	// Front wall
	AABB frontWall;
	frontWall.min = glm::vec3(-10.f, -0.75f, -7.6f);
	frontWall.max = glm::vec3(10.f, 8.f, -7.4f);
	collisionBoxes.push_back(frontWall);

	// Right wall 
	AABB rightWall;
	rightWall.min = glm::vec3(9.5f, -0.75f, -7.5f);
	rightWall.max = glm::vec3(10.f, 8.f, 14.5f);
	collisionBoxes.push_back(rightWall);

	// Left wall 
	AABB leftWallFront;
	leftWallFront.min = glm::vec3(-10.f, -0.75f, -7.5f);
	leftWallFront.max = glm::vec3(-9.5f, 8.f, 4.25f);  
	collisionBoxes.push_back(leftWallFront);


	AABB leftWallBack;
	leftWallBack.min = glm::vec3(-10.f, -0.75f, 6.25f);   
	leftWallBack.max = glm::vec3(-9.5f, 8.f, 14.75f);
	collisionBoxes.push_back(leftWallBack);

	// Back wall 
	AABB backWallLeft;
	backWallLeft.min = glm::vec3(1.f, -0.75f, 14.3f);
	backWallLeft.max = glm::vec3(10.f, 8.f, 14.6f);
	collisionBoxes.push_back(backWallLeft);

	AABB backWallRight;
	backWallRight.min = glm::vec3(-10.f, -0.75f, 14.3f);
	backWallRight.max = glm::vec3(-1.f, 8.f, 14.8f);
	collisionBoxes.push_back(backWallRight);

	// Counter 
	AABB counter;
	counter.min = glm::vec3(-10.f, -2.f, 1.0f);
	counter.max = glm::vec3(10.f, 2.f, 2.5f);
	collisionBoxes.push_back(counter);

	// Door[0] — back wall, spans X axis
	AABB door0;
	door0.min = glm::vec3(-0.1f, 0.f, 14.3f);
	door0.max = glm::vec3(2.1f, 4.f, 14.7f);
	doorCollisionIdx[0] = collisionBoxes.size();
	collisionBoxes.push_back(door0);

	// Door[1] — left wall, spans Z axis
	AABB door1;
	door1.min = glm::vec3(-10.2f, 0.f, 5.25f);
	door1.max = glm::vec3(-9.8f, 4.f, 7.25f);
	doorCollisionIdx[1] = collisionBoxes.size();
	collisionBoxes.push_back(door1);

}

void SceneCans::InitialiseCans()
{
	const float counterTopY = 1.1f;
	const float canH = 0.65f;   // visual height of one can (scaled)
	const float canSpacing = 0.65f;

	//interactive cans
	// Bottom row — 3 cans
	m_cans[0].can.pos = Vector3(-canSpacing, counterTopY, 1.5f);
	m_cans[1].can.pos = Vector3(0.f, counterTopY, 1.5f);
	m_cans[2].can.pos = Vector3(canSpacing, counterTopY, 1.5f);

	// Middle row — 2 cans (offset half a spacing, raised one can height)
	m_cans[3].can.pos = Vector3(-canSpacing * 0.5f, counterTopY + canH, 1.5f);
	m_cans[4].can.pos = Vector3(canSpacing * 0.5f, counterTopY + canH, 1.5f);

	// Top row — 1 can
	m_cans[5].can.pos = Vector3(0.f, counterTopY + canH * 2.f, 1.5f);

	//decorative cans
	m_staticCanPos[0] = glm::vec3(-canSpacing, counterTopY, 1.5f);
	m_staticCanPos[1] = glm::vec3(0.f, counterTopY, 1.5f);
	m_staticCanPos[2] = glm::vec3(canSpacing, counterTopY, 1.5f);
	m_staticCanPos[3] = glm::vec3(-canSpacing * 0.5f, counterTopY + canH, 1.5f);
	m_staticCanPos[4] = glm::vec3(canSpacing * 0.5f, counterTopY + canH, 1.5f);
	m_staticCanPos[5] = glm::vec3(0.f, counterTopY + canH * 2.f, 1.5f);

	//world space col pos
	m_cans[0].colPos = glm::vec3(-0.75f, 2.5f, -3.9f);
	m_cans[1].colPos = glm::vec3(0.0f, 2.5f, -3.9f);
	m_cans[2].colPos = glm::vec3(0.75f, 2.5f, -3.9f);
	m_cans[3].colPos = glm::vec3(-0.37f, 3.2f, -3.9f);
	m_cans[4].colPos = glm::vec3(0.37f, 3.2f, -3.9f);
	m_cans[5].colPos = glm::vec3(0.0f, 3.9f, -3.9f);


	for (int i = 0; i < NUM_CANS; ++i)
	{
		m_cans[i].active = true;
		m_cans[i].knocked = false;
		m_cans[i].startPos = m_cans[i].can.pos;
		m_cans[i].can.vel = Vector3(0.f);
		m_cans[i].can.mass = 1.f;
		m_cans[i].can.bounciness = 0.1f;
		m_cans[i].supportedBy[0] = -1;
		m_cans[i].supportedBy[1] = -1;
		m_cans[i].supportCount = 0;
	}

	// Middle row
	m_cans[3].supportedBy[0] = 0;  m_cans[3].supportedBy[1] = 1;  m_cans[3].supportCount = 2;
	m_cans[4].supportedBy[0] = 1;  m_cans[4].supportedBy[1] = 2;  m_cans[4].supportCount = 2;

	// Top
	m_cans[5].supportedBy[0] = 3;  m_cans[5].supportedBy[1] = 4;  m_cans[5].supportCount = 2;
}

void SceneCans::InitialiseBalls()
{
	m_noOfBalls = 3;
	m_throwsLeft = 3;
	ballCollected = false;
	m_isAiming = false;
	m_currentBall = 0;

	//initialise balls pos
	m_balls[0].ball.pos = Vector3(-0.25f, 0.61f, 0.f);
	m_balls[1].ball.pos = Vector3(0.4f, 0.61f, 0.f);
	m_balls[2].ball.pos = Vector3(0.95f, 0.61f, 0.f);

	// Missing ball on floor to find
	m_balls[3].ball.pos = Vector3(-15.75f, 0.75f, 10.f);

	for (int i = 0; i < 4; ++i)
	{
		m_balls[i].inAir = false;
		m_balls[i].ball.vel = Vector3(0, 0, 0);
		m_balls[i].ball.accel = Vector3(0, 0, 0);
	}
}

void SceneCans::ApplyGravity(PhysicsObject& obj, float dt)
{
	obj.vel.y += GRAVITY * dt;
	obj.pos += obj.vel * dt;
}

void SceneCans::UpdateBall(float dt)
{
	if (!m_balls[0].inAir) return;

	//add gravity as force each frame
	m_balls[0].ball.AddForce(Vector3(0.f, GRAVITY * m_balls[0].ball.mass, 0.f));
	m_balls[0].ball.UpdatePhysics(dt);

}

void SceneCans::UpdateCans(float dt)
{
	const float FRICTION = 2.5f;
	const float FLOOR_Y = 1.1f;  

	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].knocked) continue;

		// Apply gravity
		ApplyGravity(m_cans[i].can, dt); 

		// Update position
		m_cans[i].can.pos += m_cans[i].can.vel * dt;

		// Sync world-space colPos to follow the can
		m_cans[i].colPos.x = m_cans[i].can.pos.x; // sync X/Z drift
		m_cans[i].colPos.z = m_cans[i].can.pos.z;
		// Y offset: colPos was ~1.0 above can.pos originally, maintain that
		m_cans[i].colPos.y = m_cans[i].can.pos.y + 1.0f;

		// Floor collision — stop them sinking through the table/floor
		if (m_cans[i].can.pos.y <= FLOOR_Y)
		{
			m_cans[i].can.pos.y = FLOOR_Y;
			m_cans[i].can.vel.y = 0.f;         // kill vertical velocity on landing

			// Apply horizontal friction only once grounded
			m_cans[i].can.vel.x *= (1.f - FRICTION * dt);
			m_cans[i].can.vel.z *= (1.f - FRICTION * dt);

			if (m_cans[i].can.vel.Length() < 0.01f)
				m_cans[i].can.vel.SetZero();
		}
	}
}

void SceneCans::CheckBallCanCollisions(int ballIdx)
{
	glm::vec3 canHalfExtents(0.3f, 0.4f, 0.3f);  

	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].active || m_cans[i].knocked) continue;

		// Use world-space collisionPos instead of can.pos
		PhysicsObject canCollider;
		canCollider.pos = Vector3(m_cans[i].colPos.x, m_cans[i].colPos.y, m_cans[i].colPos.z);
		canCollider.vel = m_cans[i].can.vel;
		canCollider.mass = m_cans[i].can.mass;

		CollisionData3D cd;
		if (OverlapSphere2AABB(m_balls[ballIdx].ball, 0.1f,canCollider, canHalfExtents,cd))
		{
			ResolveCollision3D(cd);
			m_cans[i].can.vel = canCollider.vel;
			m_cans[i].knocked = true;

			m_balls[ballIdx].ball.vel.x *= 0.3f;
			m_balls[ballIdx].ball.vel.z *= 0.3f;
		}
	}
}

void SceneCans::CheckCanCanCollisions()
{
	glm::vec3 canHalfExtents(0.1f, 0.2f, 0.1f);

	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].knocked) continue;

		for (int j = i + 1; j < NUM_CANS; ++j)
		{
			if (m_cans[j].knocked) continue;

			// Use collisionPos for accurate chain detection too
			PhysicsObject a, b;
			a.pos = Vector3(m_cans[i].colPos.x,
				m_cans[i].colPos.y,
				m_cans[i].colPos.z);
			b.pos = Vector3(m_cans[j].colPos.x,
				m_cans[j].colPos.y,
				m_cans[j].colPos.z);
			a.mass = b.mass = 1.f;

			CollisionData3D cd;
			if (OverlapSphere2AABB(a, 0.1f, b, canHalfExtents, cd))
			{
				m_cans[j].knocked = true;

				// Very weak chain — adjacent cans just tip, not launch
				Vector3 pushDir = b.pos - a.pos;
				pushDir.y = 0.f;
				if (pushDir.Length() > 0.001f)
				{
					pushDir.Normalize();
					float chainSpeed = m_cans[i].can.vel.Length() * 0.2f; // 80% energy loss
					m_cans[j].can.vel = pushDir * chainSpeed;
					m_cans[j].can.vel.y = 0.3f;
				}
			}
		}
	}
}


void SceneCans::CheckFloorCollisions(int ballIdx)
{
	const float FLOOR_Y = 0.3f;
	const float BALL_RADIUS = 0.15f;

	if (m_balls[ballIdx].ball.pos.y - BALL_RADIUS <= FLOOR_Y)
	{
		m_balls[ballIdx].ball.pos.y = FLOOR_Y + BALL_RADIUS;
		m_balls[ballIdx].ball.vel.y *= -0.2f;
		m_balls[ballIdx].ball.vel.x *= 0.7f;
		m_balls[ballIdx].ball.vel.z *= 0.7f;

		if (std::abs(m_balls[ballIdx].ball.vel.y) < 0.5f)
		{
			m_balls[ballIdx].ball.vel.SetZero();
			m_balls[ballIdx].inAir = false;
		}
	}
}


void SceneCans::LaunchBall()
{
	if (m_currentBall >= m_noOfBalls) return; // no balls left

	m_throwsLeft--;
	
	// Direction from launch point toward where the line ends
	glm::vec3 dir = glm::normalize(m_aimWorldTarget - glm::vec3(LAUNCH_POS));
	float launchSpeed = 15.f;

	m_balls[0].ball.pos = Vector3(LAUNCH_POS.x, LAUNCH_POS.y, LAUNCH_POS.z);
	m_balls[0].ball.vel = Vector3(dir.x * launchSpeed, dir.y * launchSpeed,dir.z * launchSpeed);
	m_balls[0].inAir = true;

	m_currentBall++;
}

void SceneCans::ResetGame()
{
	//reset camera
	camera.Init(
		glm::vec3(0, 2.1, 10),		// position
		glm::vec3(0, 2, 0),		// target
		glm::vec3(0, 1.0f, 0)		// up
	);
	gameState = GAME_NOT_STARTED;

	m_currentBall = 0;
	InitialiseCans();
	InitialiseBalls();
	BuildCollisionBoxes();
}

void SceneCans::DrawAimLine()
{
	if (!m_isAiming) return;


	// Get current mouse position on screen
	double mouseX = MouseController::GetInstance()->GetMousePositionX();
	double mouseY = MouseController::GetInstance()->GetMousePositionY();

	// Convert screen position to NDC (-1 to 1)
	float ndcX = (2.f * (float)mouseX / 1920.f) - 1.f;
	float ndcY = -(2.f * (float)mouseY / 1080.f) + 1.f;  // flip Y

	// Reconstruct the aim camera's projection and view matrices
	glm::mat4 proj = glm::perspective(45.f, 16.f / 9.f, 0.1f, 1000.f);
	glm::mat4 view = glm::lookAt(AIM_CAM_POS, AIM_CAM_TARGET, glm::vec3(0, 1, 0));

	// Unproject mouse into a world-space ray
	glm::vec4 rayClip(ndcX, ndcY, -1.f, 1.f);
	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.f, 0.f);
	glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

	// --- Step 3: Intersect ray with vertical plane at Z
	const float CAN_Z = -5.5f;
	if (std::abs(rayDir.z) < 0.0001f) return;  // ray is parallel to plane, skip
	float t = (CAN_Z - AIM_CAM_POS.z) / rayDir.z;
	if (t <= 0.f) return;

	glm::vec3 aimTarget = AIM_CAM_POS + t * rayDir;
	aimTarget.y = glm::clamp(aimTarget.y, 0.f, 8.f);
	aimTarget.x = glm::clamp(aimTarget.x, -5.f, 5.f);

	// Store for LaunchBall to use
	m_aimWorldTarget = aimTarget;

	// --- Step 4: Simulate parabolic arc with gravity toward aimTarget ---
	float launchSpeed = 15.f;
	glm::vec3 dir = glm::normalize(aimTarget - glm::vec3(LAUNCH_POS));
	dir.y += 0.45f;
	m_aimWorldTarget = aimTarget;

	glm::vec3 pos(LAUNCH_POS.x, LAUNCH_POS.y, LAUNCH_POS.z);
	glm::vec3 vel = dir * launchSpeed;  // initial velocity toward mouse target

	const float SIM_DT = 0.03f;
	const int MAX_STEPS = 100;
	const float FLOOR_Y = 0.3f;

	for (int i = 0; i < MAX_STEPS; ++i)
	{
		vel.y += GRAVITY * SIM_DT;  // gravity bends it into a parabola
		pos += vel * SIM_DT;

		if (pos.y < FLOOR_Y) break;  // stop drawing when it hits the floor

		if (i % 2 != 0) continue;   // skip every other step for spaced dots

		float t = (float)i / MAX_STEPS;
		float size = glm::mix(0.08f, 0.04f, t);  // dots shrink along path

		glm::vec3 color = glm::mix(
			glm::vec3(1.f, 1.f, 0.f),   // yellow at start
			glm::vec3(1.f, 0.3f, 0.f),  // orange at end
			t
		);

		modelStack.PushMatrix();
		modelStack.Translate(pos.x, pos.y, pos.z);
		modelStack.Scale(size, size, size);
		meshList[GEO_SPHERE]->material.kAmbient = color;
		meshList[GEO_SPHERE]->material.kDiffuse = color;
		meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.f);
		meshList[GEO_SPHERE]->material.kShininess = 1.f;
		RenderMesh(meshList[GEO_SPHERE], false);
		modelStack.PopMatrix();
	}
}

//draw raycast line for debugging
void SceneCans::DrawRayCastLine()
{
	glm::vec3 rayDir = glm::normalize(camera.target - camera.position);

	// Start the line just in front of the camera (0.5f offset)
	glm::vec3 lineStart = camera.position + rayDir * 0.5f;

	glm::vec3 lineEnd = camera.position + rayDir * 3.f;

	glm::vec3 midPoint = (lineStart + lineEnd) * 0.5f;
	float lineLength = glm::length(lineEnd - lineStart);

	// Rotate from Z-axis to rayDir
	glm::vec3 zAxis(0.f, 0.f, 1.f);
	glm::vec3 rotAxis = glm::cross(zAxis, rayDir);
	float rotAngle = glm::degrees(acosf(glm::clamp(glm::dot(zAxis, rayDir), -1.f, 1.f)));

	bool rayHit = RayHitsBall(2, 3.f);
	glm::vec3 lineColor = rayHit ? glm::vec3(0.f, 1.f, 0.f)
		: glm::vec3(1.f, 0.f, 0.f);

	modelStack.PushMatrix();
	modelStack.Translate(midPoint.x, midPoint.y, midPoint.z);

	if (glm::length(rotAxis) > 0.001f)
		modelStack.Rotate(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);

	modelStack.Scale(0.02f, 0.02f, lineLength);

	meshList[GEO_CUBE]->material.kAmbient = lineColor;
	meshList[GEO_CUBE]->material.kDiffuse = lineColor;
	meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.f);
	meshList[GEO_CUBE]->material.kShininess = 1.f;
	RenderMesh(meshList[GEO_CUBE], false);
	modelStack.PopMatrix();
}

void SceneCans::RenderHUD()
{

	//crosshair
	if (gameState == GAME_NOT_STARTED || gameState == GAME_PLAYING && !m_isAiming)
		RenderTextOnScreen(meshList[GEO_TEXT], "+", glm::vec3(1, 1, 1), 40, crosshairPos.x, crosshairPos.y);

	if (!ballCollected)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Find the missing ball to start!", glm::vec3(1, 1, 0.5), 35, 20, 575);
	}
	else if (gameState == GAME_PLAYING)
	{
		if (!m_isAiming)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Knock down all the cans to win!", glm::vec3(1, 1, 0.5), 35, 20, 575);
		}
			

		std::string throwsText = "Throws left: " + std::to_string(m_throwsLeft);
		RenderTextOnScreen(meshList[GEO_TEXT], throwsText, glm::vec3(1, 1, 1), 35, 20, 540);

		int knocked = 0;
		for (int i = 0; i < NUM_CANS; ++i)
			if (m_cans[i].knocked) knocked++;

		std::string cansText = "Cans down: " + std::to_string(knocked) + "/" + std::to_string(NUM_CANS);
		RenderTextOnScreen(meshList[GEO_TEXT], cansText, glm::vec3(1, 1, 1), 35, 20, 490);


		//bombtimer
		int sec = (int)(bombTimer);
		char  timerBuf[32];
		sprintf_s(timerBuf, "TIME: %ds", sec);

		// Turn red when under 10 seconds
		glm::vec3 timerColor = (bombTimer <= 10.f) ? glm::vec3(1, 0, 0) : glm::vec3(1, 1, 1);

		RenderTextOnScreen(meshList[GEO_TEXT],timerBuf, timerColor, 35, 20, 80);
	}
	
	//ball
	if (showPickupPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to pick up ball",glm::vec3(1.f, 1.f, 0.f), 35, 220, 200);

	//booth
	if (showBoothPrompt && !m_isAiming)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to start aiming", glm::vec3(1, 1, 0), 35, 200, 200);

	if (m_isAiming)
		RenderTextOnScreen(meshList[GEO_TEXT], "Use mouse to aim, LMB to throw!", glm::vec3(1, 1, 0), 35, 20, 580);

	//end state
	if (gameState == GAME_WON)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU WIN! BOMB DEFUSED", glm::vec3(0, 1, 0), 40, 150, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "Head back to the lobby.",glm::vec3(1, 1, 0), 28, 200, 240);
	}
	else if (gameState == GAME_LOST)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU LOSE..THE BOMB EXPLODED!", glm::vec3(1, 0, 0), 40, 125, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press [R] to retry", glm::vec3(1, 1, 1), 30, 200, 240);
	}
	
}
