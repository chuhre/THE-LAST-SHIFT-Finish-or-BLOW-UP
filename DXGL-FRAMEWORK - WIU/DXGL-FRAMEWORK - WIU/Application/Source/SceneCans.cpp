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
	door[0] = { glm::vec3(1.f, -0.1f, 14.5f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };
	door[1] = { glm::vec3(-10.f, -0.1f, 6.25f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };
		
	//GAME SETUP
	SpawnCans();
	SpawnBalls();
	gameState = GAME_NOT_STARTED;
	
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


	// Store position before camera update
	glm::vec3 oldPos = camera.position;

	// Update camera position based on input
	camera.Update(dt);


	// === ANIMATION/INTERACTIONS ====
	//Door interaction
	showInteractPrompt = false;
	if (door[0].IsPlayerNear(camera.position, 2.5f))
	{
		if (SceneManager::GetInstance()->getIsGameCompleted(SceneManager::SCENE_CANS))
			showInteractPrompt = true;
			
		else 
			RenderTextOnScreen(meshList[GEO_TEXT], "You need to win the game first!", glm::vec3(1.f, 0.f, 0.f), 40, 50, 50);
	}
	if (showInteractPrompt && KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		door[0].Open();
	}

	if (door[0].Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f))
	{
		SceneManager::GetInstance()->SwitchScene(door[0].leadsTo);
		door[0].Close();
		showInteractPrompt = false;
	}

	//side door
	if (door[1].IsPlayerNear(camera.position, 2.5f))
	{
		door[1].Open();
		door[1].Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f);
	}

	

	//game logic
	if (ballCollected) 
	{
		gameState = GAME_PLAYING;
		m_noOfBalls++;
		m_throwsLeft++;

	}


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

	//check ball collection
	
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
	modelStack.Translate(door[0].width * 0.5f, 0.f, 0.f);  //shift door right
	modelStack.Scale(door[1].width, door[1].height, 0.2f);
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.5f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	RenderMesh(meshList[GEO_DOOR], true);
	modelStack.PopMatrix();

	if (showInteractPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F to enter", glm::vec3(1.f, 1.f, 0.f), 40, 50, 50);


	//environment
	modelStack.PushMatrix();                     
	modelStack.Translate(0.f, -2.f, 0.f);          

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
		modelStack.Scale(20.f, 8.f, 8.f);
		meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();

		//render 2nd ball
		modelStack.PushMatrix();
		modelStack.Translate(m_balls[1].ball.pos.x, m_balls[1].ball.pos.y, m_balls[1].ball.pos.z);
		modelStack.Scale(20.f, 8.f, 8.f);
		meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
		meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
		meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		RenderMesh(meshList[GEO_BALL], true);
		modelStack.PopMatrix();

		modelStack.PopMatrix();
	}
	
	//rernder missing ball
	modelStack.PushMatrix();
	modelStack.Translate(m_balls[2].ball.pos.x, m_balls[2].ball.pos.y, m_balls[2].ball.pos.z);
	modelStack.Scale(20.f, 20.f, 20.f);
	meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	RenderMesh(meshList[GEO_BALL], true);
	modelStack.PopMatrix();


	//MAIN TABLE 
	{
		modelStack.PushMatrix();
		modelStack.Translate(0.f, 0.f, -4.f);
		modelStack.Scale(1.25f, 1.25f, 1.25f);
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

			modelStack.Scale(0.3f, 0.3f, 0.3f);
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
		modelStack.Scale(1.25f, 1.25f, 1.25f);
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
			modelStack.Scale(0.3f, 0.3f, 0.3f);
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
		modelStack.Scale(1.25f, 1.25f, 1.25f);
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
			modelStack.Scale(0.3f, 0.3f, 0.3f);
			meshList[GEO_CAN]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			meshList[GEO_CAN]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			meshList[GEO_CAN]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			RenderMesh(meshList[GEO_CAN], true);
			modelStack.PopMatrix();
		}
		modelStack.PopMatrix();

	}
	
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
		if (gameState == GAME_WON || gameState == GAME_LOST)
		{
			// Mark game completed if won, then return to lobby
			if (gameState == GAME_WON)
				SceneManager::GetInstance()->gameCompleted[1] = true; // index 1 = shooting booth

			SceneManager::GetInstance()->SwitchScene(SceneManager::SCENE_LOBBY);
		}
	}
}

void SceneCans::HandleMouseInput() {
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

void SceneCans::SpawnCans()
{
	const float counterTopY = 1.1f;
	const float canH = 0.8f;   // visual height of one can (scaled)
	const float canSpacing = 0.7f;

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

void SceneCans::SpawnBalls()
{
	m_noOfBalls = 2;
	m_throwsLeft = 2;

	//initialise balls pos
	m_balls[0].ball.pos = Vector3(-0.25f, 0.7f, 0.f);
	m_balls[1].ball.pos = Vector3(0.5f, 0.7f, 0.f);
	m_balls[2].ball.pos = Vector3(-15.75f, 1.f, 10.f);

}

void SceneCans::ApplyGravity(PhysicsObject& obj, float dt)
{
	obj.vel.y += GRAVITY * dt;
	obj.pos += obj.vel * dt;
}

void SceneCans::UpdateBall(float dt)
{
	//add ball when player picks up ball


}

void SceneCans::UpdateCans(float dt)
{
}

void SceneCans::CheckBallCanCollisions()
{
}

void SceneCans::CheckCanCanCollisions()
{
}

void SceneCans::CheckFloorCollisions()
{
}

void SceneCans::LaunchBall()
{
}

void SceneCans::ResetGame()
{
	SpawnCans();
	SpawnBalls();
}

void SceneCans::DrawAimLine()
{
}

void SceneCans::RenderHUD()
{

	if (!ballCollected)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Find the mising ball to start!", glm::vec3(1, 1, 0.5), 35, 20, 575);
	}

	std::string throwsText = "Throws left: " + std::to_string(m_throwsLeft);
	RenderTextOnScreen(meshList[GEO_TEXT], throwsText, glm::vec3(1, 1, 1), 35, 20, 540);

	int knocked = 0;
	for (int i = 0; i < NUM_CANS; ++i) 
		if (m_cans[i].knocked) knocked++;

	std::string cansText = "Cans down: " + std::to_string(knocked) + "/" + std::to_string(NUM_CANS);
	RenderTextOnScreen(meshList[GEO_TEXT], cansText, glm::vec3(1, 1, 1), 35, 20, 490);

	//crosshair
	RenderTextOnScreen(meshList[GEO_TEXT], "+", glm::vec3(1, 1, 1), 40, 395, 290);

	//end state
	if (gameState == GAME_WON)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU WIN!", glm::vec3(0, 1, 0), 60, 280, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press F at the door to leave",
			glm::vec3(1, 1, 0), 28, 200, 240);
	}
	else if (gameState == GAME_LOST)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "OUT OF BALLS!", glm::vec3(1, 0, 0), 50, 240, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press R to retry",
			glm::vec3(1, 1, 1), 30, 310, 240);
	}
	
}
