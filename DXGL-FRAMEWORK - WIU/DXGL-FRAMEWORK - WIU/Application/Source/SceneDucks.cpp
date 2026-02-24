#include "SceneDucks.h"
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

SceneDucks::SceneDucks()
{
}

SceneDucks::~SceneDucks()
{
}

void SceneDucks::Init()
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
	//meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Arm", glm::vec3(0.5f, 0.5f, 0.5f), 1.f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
	//meshList[GEO_PLANE]->textureID = LoadTGA("Images//met4.tga");

	// DUCKKKK
	meshList[GEO_POOL] = MeshBuilder::GenerateOBJ("Pool", "Models//pool.obj");
	meshList[GEO_POOL]->textureID = LoadTGA("Images//pool1.tga");
	meshList[GEO_WATER] = MeshBuilder::GenerateSphere("Water", glm::vec3(0.1f, 0.4f, 0.8f), 1.f, 32);
	meshList[GEO_WATER]->textureID = LoadTGA("Images//water.tga");
	meshList[GEO_DUCK] = MeshBuilder::GenerateOBJ("Duck", "Models//duck1.obj");
	meshList[GEO_DUCKLEYE] = MeshBuilder::GenerateSphere("LeftDuckEye", glm::vec3(0.0f, 0.0f, 0.0f), 1.f, 32);
	meshList[GEO_DUCKREYE] = MeshBuilder::GenerateSphere("RighttDuckEye", glm::vec3(0.0f, 0.0f, 0.0f), 1.f, 32);
	meshList[GEO_PEGHOOK] = MeshBuilder::GenerateOBJ("Peghook", "Models//peghook.obj");

	// UI
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_GUI] = MeshBuilder::GenerateQuad("GUI", glm::vec3(1, 1, 1), 1.f);

	// Environment (copy from SceneShooting)
	meshList[GEO_FLOOR] = MeshBuilder::GenerateRectangularPrism("Floor", glm::vec3(0.45f, 0.32f, 0.18f),20.f, 0.2f, 15.f);
	meshList[GEO_CEILING] = MeshBuilder::GenerateRectangularPrism("Ceiling", glm::vec3(0.85f, 0.75f, 0.55f),20.f, 0.2f, 15.f);
	meshList[GEO_WALL] = MeshBuilder::GenerateRectangularPrism("Wall", glm::vec3(0.9f, 0.85f, 0.6f),1.f, 1.f, 1.f);
	meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism("Counter", glm::vec3(0.55f, 0.35f, 0.15f),20.f, 1.0f, 0.4f);
	




	// In Init() — change 4.0f/3.0f -> 16.0f/9.0f (or 1920.0f/1080.0f)
	glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	// Player collision box size (width, height, depth)
	playerSize = glm::vec3(0.4f, 1.8f, 0.4f);


	// ANIMATIONS

	// ANIMATIONS
	gameState = STATE_FIND_HOOK;
	hookPickedUp = false;
	hookWorldPos = glm::vec3(4.f, 0.5f, 5.f);
	ducksPickedUp = 0;
	catchTimer = 0.f;
	duckAngle = 0.f;
	duckRadius = 12.f;
	duckSpeed = 1.f;


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

bool SceneDucks::IsPlayerNearHook(float radius)
{
	float dx = camera.position.x - hookWorldPos.x;
	float dy = camera.position.y - hookWorldPos.y;
	float dz = camera.position.z - hookWorldPos.z;
	float distSq = dx * dx + dy * dy + dz * dz;
	return distSq <= (radius * radius);
}



void SceneDucks::Update(double dt)
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


	//  ANIMATION/INTERACTIONS 


	duckAngle += duckSpeed * (float)dt;
	if (duckAngle > glm::two_pi<float>())
		duckAngle -= glm::two_pi<float>();

	if (catchTimer > 0.f)
		catchTimer -= (float)dt;

	// Check if hook catches a duck (only while playing)
	if (gameState == STATE_PLAYING && hookPickedUp)
	{
		float duckWorldX = duckRadius * glm::cos(duckAngle);
		float duckWorldZ = duckRadius * glm::sin(duckAngle);

		glm::vec3 view = glm::normalize(camera.target - camera.position);
		glm::vec3 right = glm::normalize(glm::cross(view, glm::vec3(0, 1, 0)));
		glm::vec3 hookTip = camera.position + view * 2.f + right * 0.5f;

		float dx = hookTip.x - duckWorldX;
		float dz = hookTip.z - duckWorldZ;
		float distSq = dx * dx + dz * dz;

		if (distSq < 1.5f && catchTimer <= 0.f)  // cooldown prevents multi-catch
		{
			ducksPickedUp++;
			catchTimer = 1.5f;
			if (ducksPickedUp >= MAX_DUCKS)
				gameState = STATE_WON;
		}
	}

	//// ANIMATIONS
	


}

void SceneDucks::Render()
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

	// ---- BOOTH ----
	modelStack.PushMatrix();                        // >>> BOOTH ROOT
	modelStack.Translate(0.f, 0.f, 0.f);           // adjust if needed

	// FLOOR
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -2.0f, 0.f);
	modelStack.Scale(5.f, 2.f, 5.0f);

	meshList[GEO_FLOOR]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_FLOOR]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.2f);
	meshList[GEO_FLOOR]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FLOOR]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_FLOOR], true);
	modelStack.PopMatrix();

	// CEILING
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 6.f, 0.f);
	meshList[GEO_CEILING]->material.kAmbient = glm::vec3(0.4f, 0.35f, 0.25f);
	meshList[GEO_CEILING]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.55f);
	meshList[GEO_CEILING]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_CEILING]->material.kShininess = 1.f;
	RenderMesh(meshList[GEO_CEILING], true);
	modelStack.PopMatrix();

	// BACK WALL
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 2.f, -7.5f);
	modelStack.Scale(20.f, 8.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// LEFT WALL
	modelStack.PushMatrix();
	modelStack.Translate(-10.f, 2.f, 0.f);
	modelStack.Scale(0.3f, 8.f, 15.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// RIGHT WALL
	modelStack.PushMatrix();
	modelStack.Translate(10.f, 2.f, 0.f);
	modelStack.Scale(0.3f, 8.f, 15.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	//// COUNTER
	//modelStack.PushMatrix();
	//modelStack.Translate(0.f, 0.5f, 1.5f);
	//meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
	//meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
	//meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
	//meshList[GEO_COUNTER]->material.kShininess = 8.f;
	//RenderMesh(meshList[GEO_COUNTER], true);
	//modelStack.PopMatrix();

	// POOL
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.5f, 0.f);
	modelStack.Scale(0.2f, 0.2f, 0.2f);         
	RenderMesh(meshList[GEO_POOL], true);
	meshList[GEO_POOL]->material.kAmbient = glm::vec3(0.1f, 0.2f, 0.3f);
	meshList[GEO_POOL]->material.kDiffuse = glm::vec3(0.2f, 0.5f, 0.8f);
	meshList[GEO_POOL]->material.kSpecular = glm::vec3(0.3f, 0.5f, 0.7f);
	meshList[GEO_POOL]->material.kShininess = 16.f;


	// In Render()
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 1.0f, 0.f);   // just above floor
	modelStack.Scale(18.f, 1.f, 18.f);          // stretch to pool size
	//modelStack.Rotate(0.f, 0.f, 90.f, 0.f);         
	meshList[GEO_WATER]->material.kAmbient = glm::vec3(0.0f, 0.2f, 0.5f);
	meshList[GEO_WATER]->material.kDiffuse = glm::vec3(0.1f, 0.4f, 0.8f);
	meshList[GEO_WATER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 1.0f);  // high specularity = shiny
	meshList[GEO_WATER]->material.kShininess = 64.f;                          // glossy look
	RenderMesh(meshList[GEO_WATER], true);
	modelStack.PopMatrix();

	// DUCK
	modelStack.PushMatrix();

	// Circle position using sin/cos
	float duckX = duckRadius * glm::cos(duckAngle);
	float duckZ = duckRadius * glm::sin(duckAngle);
	modelStack.Translate(duckX, 1.5f, duckZ);

	// Face the direction of movement (tangent to circle)
	float facingAngle = glm::degrees(duckAngle) + 90.f;
	modelStack.Rotate(facingAngle, 0.f, 1.f, 0.f);

	modelStack.Scale(0.07f, 0.07f, 0.07f);
	meshList[GEO_DUCK]->material.kAmbient = glm::vec3(1.0f, 0.8f, 0.0f);
	meshList[GEO_DUCK]->material.kDiffuse = glm::vec3(1.0f, 0.85f, 0.1f);
	meshList[GEO_DUCK]->material.kSpecular = glm::vec3(0.4f, 0.35f, 0.1f);
	meshList[GEO_DUCK]->material.kShininess = 8.f;
	RenderMesh(meshList[GEO_DUCK], true);

	modelStack.PushMatrix();
	modelStack.Translate(10.f, 28.f, 10.f);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[GEO_DUCKLEYE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(10.f, 28.f, -10.f);
	modelStack.Scale(2.0f, 2.0f, 2.0f);
	RenderMesh(meshList[GEO_DUCKREYE], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix(); //DUCK ROOT

	modelStack.PopMatrix(); // POOL ROOT

	modelStack.PopMatrix();                         // <<< BOOTH ROOT
	
	// Hook on floor (only before pickup)
	if (!hookPickedUp)
	{
		modelStack.PushMatrix();
		modelStack.Translate(hookWorldPos.x, hookWorldPos.y, hookWorldPos.z);
		modelStack.Scale(3.f, 3.f, 3.f);
		meshList[GEO_PEGHOOK]->material.kAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
		meshList[GEO_PEGHOOK]->material.kDiffuse = glm::vec3(0.6f, 0.6f, 0.6f);
		meshList[GEO_PEGHOOK]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		meshList[GEO_PEGHOOK]->material.kShininess = 64.f;
		RenderMesh(meshList[GEO_PEGHOOK], true);
		modelStack.PopMatrix();
	}

	// Hook held in hand (only after pickup) — camera basis method
	if (hookPickedUp)
	{
		glm::vec3 view = glm::normalize(camera.target - camera.position);
		glm::vec3 right = glm::normalize(glm::cross(view, glm::vec3(0, 1, 0)));
		glm::vec3 up = glm::normalize(glm::cross(right, view));
		glm::vec3 hookPos = camera.position
			+ view * 1.5f
			+ right * 0.5f
			+ up * (-0.5f);

		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 cameraBasis = glm::mat4(
			glm::vec4(right, 0.f),
			glm::vec4(up, 0.f),
			glm::vec4(-view, 0.f),
			glm::vec4(hookPos, 1.f)
		);

		modelStack.PushMatrix();
		modelStack.LoadIdentity();
		modelStack.LoadMatrix(cameraBasis);
		modelStack.Scale(2.6f, 2.6f, 2.6f);
		modelStack.Rotate(180.f, 0.f, 1.f, 0.f);
		meshList[GEO_PEGHOOK]->material.kAmbient = glm::vec3(0.3f, 0.3f, 0.3f);
		meshList[GEO_PEGHOOK]->material.kDiffuse = glm::vec3(0.6f, 0.6f, 0.6f);
		meshList[GEO_PEGHOOK]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
		meshList[GEO_PEGHOOK]->material.kShininess = 64.f;
		RenderMesh(meshList[GEO_PEGHOOK], true);
		modelStack.PopMatrix();
	}

	// HUD
	if (gameState == STATE_FIND_HOOK)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"Find the hook!", glm::vec3(1, 1, 0), 30.f, 90.f, 540.f);
		if (IsPlayerNearHook(2.5f))
			RenderTextOnScreen(meshList[GEO_TEXT],
				"[F] Pick up Hook", glm::vec3(1, 1, 1), 35.f, 300.f, 480.f);
	}
	if (gameState == STATE_PLAYING)
	{
		char buf[32];
		sprintf_s(buf, "Ducks: %d / %d", ducksPickedUp, MAX_DUCKS);
		RenderTextOnScreen(meshList[GEO_TEXT], buf, glm::vec3(1, 1, 1), 30.f, 30.f, 560.f);
		RenderTextOnScreen(meshList[GEO_TEXT], "+", glm::vec3(1, 1, 1), 40.f, 390.f, 285.f);

		if (catchTimer > 0.f)
			RenderTextOnScreen(meshList[GEO_TEXT],
				"GOT ONE!", glm::vec3(0, 1, 0), 50.f, 300.f, 400.f);
	}
	if (gameState == STATE_WON)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"You caught them all!", glm::vec3(0, 1, 0), 40.f, 200.f, 400.f);
		RenderTextOnScreen(meshList[GEO_TEXT],
			"[R] Return to Lobby", glm::vec3(1, 1, 0), 30.f, 220.f, 340.f);
	}

	// render tests


	// Skybox NIGHT
	//RenderSkybox();



}

void SceneDucks::RenderMesh(Mesh* mesh, bool enableLight)
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


void SceneDucks::RenderSkybox() {
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



void SceneDucks::RenderMeshOnScreen(Mesh* mesh, float x, float
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






void SceneDucks::Exit()
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

void SceneDucks::HandleKeyPress()
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

	if (KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		if (gameState == STATE_FIND_HOOK && IsPlayerNearHook(2.5f))
		{
			hookPickedUp = true;
			gameState = STATE_PLAYING;
		}
	}

	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		if (gameState == STATE_WON)
			SceneManager::GetInstance()->SwitchScene(SceneManager::SCENE_LOBBY);
	}

}

void SceneDucks::HandleMouseInput() {
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



void SceneDucks::RenderText(Mesh* mesh, std::string text, glm::vec3
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



void SceneDucks::RenderTextOnScreen(Mesh* mesh, std::string
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