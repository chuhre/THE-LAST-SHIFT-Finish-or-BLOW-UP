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

	// Environment
	meshList[GEO_FLOOR] = MeshBuilder::GenerateRectangularPrism("Floor", glm::vec3(0.45f, 0.32f, 0.18f), 20.f, 0.2f, 15.f);
	meshList[GEO_CEILING] = MeshBuilder::GenerateRectangularPrism("Ceiling", glm::vec3(0.85f, 0.75f, 0.55f), 20.f, 0.2f, 15.f);
	meshList[GEO_WALL] = MeshBuilder::GenerateRectangularPrism("Wall", glm::vec3(0.9f, 0.85f, 0.6f),1.f, 1.f, 1.f);   

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
		
	//GAME SETUP
	InitialiseCans();
	InitialiseBalls();
	BuildCollisionBoxes();
	gameState = GAME_NOT_STARTED;
	SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_CANS] = false;

}





void SceneCans::Update(double dt)
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
		if (m_balls->inAir)
		{
			UpdateBall(static_cast<float>(dt));
			CheckBallCanCollisions();
			CheckFloorCollisions();
		}

		UpdateCans(static_cast<float>(dt));
		CheckCanCanCollisions();

		//check if all cans knocked
		if (m_throwsLeft == 0)
		{
			int knocked = 0;
			for (int i = 0; i < NUM_CANS; ++i)
				if (m_cans[i].knocked) knocked++;

			if (knocked == NUM_CANS)
			{
				gameState = GAME_WON;
				SceneManager::GetInstance()->gameCompleted[SceneManager::SCENE_CANS] = true;
			}
			else if (!m_balls->inAir && m_throwsLeft <= 0)
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
			camera.up = glm::vec3(0.f, 1.f, 0.f);

			// Reset aim to point roughly at the cans
			m_aimPitch = 30.f;
			m_aimZOffset = 0.f;
			m_aimDir = glm::normalize(glm::vec3(
				cos(glm::radians(m_aimPitch)), sin(glm::radians(m_aimPitch)), 0.f
			));
			m_dynamicAimTarget = AIM_CAM_POS + glm::vec3(6.f, 1.5f, 0.f);
			camera.target = m_dynamicAimTarget;

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

		//render ball on counter
		modelStack.PushMatrix();
		modelStack.Translate(m_balls[0].ball.pos.x, m_balls[0].ball.pos.y, m_balls[0].ball.pos.z);
		modelStack.Rotate(0.f, 0.f, 1.f, 0.f);
		modelStack.Scale(15.f, 6.f, 6.f);
		modelStack.Rotate(0, renderBall, 0, 0);
		meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();

		//render 2nd ball
		modelStack.PushMatrix();
		modelStack.Translate(m_balls[1].ball.pos.x, m_balls[1].ball.pos.y, m_balls[1].ball.pos.z);
		modelStack.Scale(15.f, 6.f, 6.f);
		modelStack.Rotate(0, renderBall, 0, 0);
		meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}

	//render missing ball
	modelStack.PushMatrix();
	modelStack.Translate(m_balls[2].ball.pos.x, m_balls[2].ball.pos.y, m_balls[2].ball.pos.z);
	modelStack.Scale(17.f, 17.f, 17.f);
	modelStack.Rotate(0, renderBall, 0, 0);
	meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	RenderMesh(meshList[GEO_BALL], true);
	modelStack.PopMatrix();


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
	
	//draw outlines of collision area
	for (const AABB& box : collisionBoxes)
	{
		modelStack.PushMatrix();

		// Calculate center and size
		glm::vec3 center = (box.min + box.max) * 0.5f;
		glm::vec3 size = box.max - box.min;

		modelStack.Translate(center.x, center.y, center.z);
		modelStack.Scale(size.x, size.y, size.z);

		// Render as wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		RenderMesh(meshList[GEO_CUBE], false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		modelStack.PopMatrix();
	}

	// Only show aim line when game is active and ball not in air
    if (gameState == GAME_PLAYING && !m_balls->inAir)
        DrawAimLine();
	DrawRayCastLine();
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

	if (m_isAiming)
	{
		// RMB held = adjust aim
		if (MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
		{
			double dx = MouseController::GetInstance()->GetMouseDeltaX();
			double dy = MouseController::GetInstance()->GetMouseDeltaY();

			float sensitivity = 0.15f;

			// Mouse Y controls throw angle (pitch) — up = higher arc
			m_aimPitch += (float)dy * sensitivity;
			m_aimPitch = glm::clamp(m_aimPitch, -20.f, 20.f);  // keep arc reasonable

			// Mouse X shifts Z — lets player target different cans in the row
			m_aimZOffset -= (float)dx * sensitivity * 0.05f;
			m_aimZOffset = glm::clamp(m_aimZOffset, -5.f, -4.f);
		}

		// Recompute aim direction: launching rightward (+X), angled up by pitch
		// Z offset steers toward front/back cans
		m_aimDir = glm::normalize(glm::vec3(
			cos(glm::radians(m_aimPitch)),   // rightward component
			sin(glm::radians(m_aimPitch)),   // upward arc
			m_aimZOffset                     // side-to-side targeting
		));

		
		// LMB = throw
		static bool wasDown = false;
		bool isDown = MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT);
		if (isDown && !wasDown)
		{
			wasDown = true;
			if (gameState == GAME_PLAYING && m_throwsLeft > 0 && !m_balls->inAir)
			{
				LaunchBall();
				camera.position = m_savedCamPos;
				camera.target = m_savedCamTarget;
				camera.up = m_savedCamUp;
				m_isAiming = false;
			}
		}
		wasDown = isDown;
	}
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

bool SceneCans::RayHitsBall(int ballIndex, float maxDist)
{
	// Convert Vector3 ball pos to glm
	glm::vec3 ballPos(m_balls[2].ball.pos.x, m_balls[2].ball.pos.y, m_balls[2].ball.pos.z);

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

	////separator
	//AABB barrier;
	//barrier.min = glm::vec3(6.f, -0.75f, -7.5f);
	//barrier.max = glm::vec3(7.f, 8.f, 2.5f);
	//collisionBoxes.push_back(barrier);

	//AABB barrierL;
	//barrierL.min = glm::vec3(-7.f, -0.75f, -7.5f);
	//barrierL.max = glm::vec3(-6.f, 8.f, 2.5f);
	//collisionBoxes.push_back(barrierL);

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

	for (int i = 0; i < NUM_CANS; ++i)
	{
		m_cans[i].active = true;
		m_cans[i].knocked = false;
		m_cans[i].startPos = m_cans[i].can.pos;  
		m_cans[i].can.vel = Vector3(0.f);
	}

	//decorative cans
	m_staticCanPos[0] = glm::vec3(-canSpacing, counterTopY, 1.5f);
	m_staticCanPos[1] = glm::vec3(0.f, counterTopY, 1.5f);
	m_staticCanPos[2] = glm::vec3(canSpacing, counterTopY, 1.5f);
	m_staticCanPos[3] = glm::vec3(-canSpacing * 0.5f, counterTopY + canH, 1.5f);
	m_staticCanPos[4] = glm::vec3(canSpacing * 0.5f, counterTopY + canH, 1.5f);
	m_staticCanPos[5] = glm::vec3(0.f, counterTopY + canH * 2.f, 1.5f);
}

void SceneCans::InitialiseBalls()
{
	m_noOfBalls = 2;
	m_throwsLeft = 2;
	ballCollected = false;

	m_isAiming = false;

	//initialise balls pos
	m_balls[0].ball.pos = Vector3(-0.25f, 0.61f, 0.f);
	m_balls[1].ball.pos = Vector3(0.4f, 0.61f, 0.f);
	m_balls[2].ball.pos = Vector3(-15.75f, 0.75f, 10.f);

}

void SceneCans::ApplyGravity(PhysicsObject& obj, float dt)
{
	obj.vel.y += GRAVITY * dt;
	obj.pos += obj.vel * dt;
}

void SceneCans::UpdateBall(float dt)
{
	if (!m_balls->inAir) return;
	ApplyGravity(m_balls->ball, dt);
}

void SceneCans::UpdateCans(float dt)
{
	const float FRICTION = 0.85f;

	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].knocked) continue;

		m_cans[i].can.pos += m_cans[i].can.vel * dt;
		m_cans[i].can.vel = m_cans[i].can.vel * FRICTION;

		if (m_cans[i].can.vel.Length() < 0.01f)
			m_cans[i].can.vel.SetZero();
	}
}

void SceneCans::CheckBallCanCollisions()
{
	const float BALL_RADIUS = 0.15f;
	const float CAN_RADIUS = 0.18f;  // tune to match your can model

	Vector3 ballPos = m_balls->ball.pos;

	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].active || m_cans[i].knocked) continue;

		Vector3 canPos = m_cans[i].can.pos;
		Vector3 diff = ballPos - canPos;
		float dist = diff.Length();
		float minDist = BALL_RADIUS + CAN_RADIUS;

		if (dist < minDist && dist > 0.f)
		{
			// Knock the can over
			m_cans[i].knocked = true;

			// Give it a velocity in the direction of impact
			Vector3 knockDir = diff;
			knockDir.Normalize();
			float impactSpeed = m_balls->ball.vel.Length() * 0.6f;
			m_cans[i].can.vel = knockDir * impactSpeed;

			// Deflect the ball slightly
			m_balls->ball.vel.x *= 0.6f;
			m_balls->ball.vel.z *= 0.6f;
		}
	}
}

void SceneCans::CheckCanCanCollisions()
{
	const float CAN_RADIUS = 0.18f;

	for (int i = 0; i < NUM_CANS; ++i)
	{
		if (!m_cans[i].knocked) continue;  // only moving cans hit others

		for (int j = 0; j < NUM_CANS; ++j)
		{
			if (i == j || m_cans[j].knocked) continue;

			Vector3 diff = m_cans[j].can.pos - m_cans[i].can.pos;
			float dist = diff.Length();

			if (dist < CAN_RADIUS * 2.f && dist > 0.f)
			{
				m_cans[j].knocked = true;

				Vector3 pushDir = diff;
				pushDir.Normalize();
				float speed = m_cans[i].can.vel.Length() * 0.5f;
				m_cans[j].can.vel = pushDir * speed;
			}
		}
	}
}


void SceneCans::CheckFloorCollisions()
{
	const float FLOOR_Y = 0.3f;   // match your floor AABB top
	const float BALL_RADIUS = 0.15f;

	if (m_balls->ball.pos.y - BALL_RADIUS <= FLOOR_Y)
	{
		m_balls->ball.pos.y = FLOOR_Y + BALL_RADIUS;

		// small bounce, then kill velocity
		m_balls->ball.vel.y *= -0.2f;
		m_balls->ball.vel.x *= 0.7f;
		m_balls->ball.vel.z *= 0.7f;

		if (std::abs(m_balls->ball.vel.y) < 0.5f)
		{
			m_balls->ball.vel.SetZero();
			m_balls->inAir = false;
		}
	}
}

bool SceneCans::CheckSceneCollisions()
{
	return false;
}

void SceneCans::LaunchBall()
{
	m_throwsLeft--;
	m_noOfBalls--;

	if (m_noOfBalls < 0) m_noOfBalls == 0;

	float launchSpeed = 14.f;

	// Start ball at the launch point on the left side
	m_balls->ball.pos = Vector3(LAUNCH_POS.x, LAUNCH_POS.y, LAUNCH_POS.z + m_aimZOffset);

	// Velocity driven by pitch angle — flies rightward in an arc
	m_balls->ball.vel = Vector3(
		m_aimDir.x * launchSpeed,
		m_aimDir.y * launchSpeed,       // arc height set by pitch
		m_aimDir.z * launchSpeed * 0.5f // gentle Z steering
	);
	m_balls->inAir = true;
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
	InitialiseCans();
	InitialiseBalls();
}

void SceneCans::DrawAimLine()
{
	if (!m_isAiming) return;


	float launchSpeed = 14.f;

	glm::vec3 pos(LAUNCH_POS.x, LAUNCH_POS.y, LAUNCH_POS.z + m_aimZOffset);
	glm::vec3 vel(
		m_aimDir.x * launchSpeed,
		m_aimDir.y * launchSpeed,
		m_aimDir.z * launchSpeed * 0.5f
	);

	const float SIM_DT = 0.04f;
	const int MAX_STEPS = 60;
	const float FLOOR_Y = 0.3f;

	for (int i = 0; i < MAX_STEPS; ++i)
	{
		vel.y += GRAVITY * SIM_DT;
		pos += vel * SIM_DT;

		if (pos.y < FLOOR_Y) break;

		if (i % 2 != 0) continue;  // every other step = spaced dots

		float t = (float)i / MAX_STEPS;
		float size = glm::mix(0.1f, 0.03f, t);  // dots shrink along path

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

	}
	
	//ball
	if (showPickupPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to pick up ball",glm::vec3(1.f, 1.f, 0.f), 35, 220, 200);

	//booth
	if (showBoothPrompt && !m_isAiming)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to start aiming", glm::vec3(1, 1, 0), 35, 200, 200);

	if (m_isAiming)
		RenderTextOnScreen(meshList[GEO_TEXT], "RMB to aim, LMB to throw!", glm::vec3(1, 1, 0), 35, 20, 580);
		
	//crosshair
	if(gameState == GAME_NOT_STARTED || gameState == GAME_PLAYING && !m_isAiming)
		RenderTextOnScreen(meshList[GEO_TEXT], "+", glm::vec3(1, 1, 1), 40, crosshairPos.x, crosshairPos.y);

	
	//end state
	if (gameState == GAME_WON)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU WIN!", glm::vec3(0, 1, 0), 60, 280, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "Head back to the lobby.",
			glm::vec3(1, 1, 0), 28, 200, 240);
	}
	else if (gameState == GAME_LOST)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "OUT OF BALLS!", glm::vec3(1, 0, 0), 50, 240, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press R to retry",
			glm::vec3(1, 1, 1), 30, 310, 240);
	}
	
}
