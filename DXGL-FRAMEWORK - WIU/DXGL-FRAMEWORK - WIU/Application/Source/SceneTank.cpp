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
		glm::vec3(0, 2.1, 8),    // position — slightly inside the room
		glm::vec3(0, 2, 0),      // target — looking forward into the room
		glm::vec3(0, 1.0f, 0)    // up
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
	meshList[GEO_BALL2] = MeshBuilder::GenerateSphere("Ball2", glm::vec3(1.f, 1.f, 1.f), 0.5f, 16, 16);
	meshList[GEO_BALL3] = MeshBuilder::GenerateSphere("Ball3", glm::vec3(1.f, 1.f, 1.f), 0.5f, 16, 16);
	meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism("Counter", glm::vec3(0.55f, 0.35f, 0.15f), 15.f, 2.0f, 1.0f);
	meshList[GEO_BOMB] = MeshBuilder::GenerateSphere("Bomb", glm::vec3(0.05f, 0.05f, 0.05f), 0.35f, 16, 16);
	meshList[GEO_PILLAR] = MeshBuilder::GenerateRectangularPrism("Pillar", glm::vec3(0.55f, 0.35f, 0.15f), 1.f, 5.f, 1.f);
	meshList[GEO_DOOR] = MeshBuilder::GenerateRectangularPrism("Door", glm::vec3(0.4f, 0.25f, 0.15f), 1.f, 1.f, 1.f); // dark wood color
	//meshList[GEO_PLANE]->textureID = LoadTGA("Images//met4.tga");

	// OBJ Models
	meshList[GEO_TARGET] = MeshBuilder::GenerateOBJMTL("Target", "Models//target.obj", "Models//target.mtl");
	meshList[GEO_TARGET]->textureID = LoadTGA("Images//target_baseColor.tga");
	meshList[GEO_DUMMY] = MeshBuilder::GenerateOBJ("Dummy", "Models//doorman.obj");
	meshList[GEO_DUMMY]->textureID = LoadTGA("Images//doorman.tga");
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

	// text
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

	// Environment
	meshList[GEO_FLOOR] = MeshBuilder::GenerateRectangularPrism("Floor", glm::vec3(0.45f, 0.32f, 0.18f), 20.f, 0.2f, 20.f);  // dark wood brown

	meshList[GEO_CEILING] = MeshBuilder::GenerateRectangularPrism("Ceiling", glm::vec3(0.85f, 0.75f, 0.55f), 20.f, 0.2f, 15.f); // light tan canvas

	meshList[GEO_WALL] = MeshBuilder::GenerateRectangularPrism("Wall", glm::vec3(0.9f, 0.85f, 0.6f), 1.f, 1.f, 1.f);   // carnival cream
																													   // scaled per-wall in Render()


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

	//initialise door 
	door = { glm::vec3(1.f, 2.f, 10.f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };

	// ball physics properties
	ballPhys.pos = Vector3(ballRestPos.x, ballRestPos.y, ballRestPos.z);
	ballPhys.mass = 0.5f;
	ballPhys.bounciness = 0.3f;

	//// collision objects (position, mass)
	//// Walls
	//wallBack.pos = Vector3(0.f, 4.f, -7.5f);  wallBack.mass = 0.f;
	//wallLeft.pos = Vector3(-10.f, 4.f, 0.f);  wallLeft.mass = 0.f;
	//wallRight.pos = Vector3(10.f, 4.f, 0.f);   wallRight.mass = 0.f;
	//wallCeiling.pos = Vector3(0.f, 8.f, 0.f);    wallCeiling.mass = 0.f;

	//// Objects
	//objCounter.pos = Vector3(0.f, 0.5f, 3.f);    objCounter.mass = 0.f;
	//objPillar.pos = Vector3(2.5f, 1.f, 0.f);    objPillar.mass = 0.f;
	//objTank.pos = Vector3(-5.5f, 2.f, 0.f);   objTank.mass = 0.f;
	//objCabinet.pos = Vector3(8.9f, 0.f, -5.f);   objCabinet.mass = 0.f;
	//objBox1.pos = Vector3(-8.9f, 0.5f, -3.f); objBox1.mass = 0.f;
	//objBox2.pos = Vector3(-8.9f, 1.56f, -3.f); objBox2.mass = 0.f;
	//objBox3.pos = Vector3(-7.9f, 0.5f, -2.f); objBox3.mass = 0.f;
	//objBox4.pos = Vector3(-7.9f, 0.5f, -4.f); objBox4.mass = 0.f;

	// target phys — position matches render Translate exactly
	objTarget.pos = Vector3(3.5f, 3.f, 0.f);
	objTarget.mass = 0.f;

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

	BuildCollisionBoxes();

	gameState = STATE_FIND_BALLS;
}


bool SceneTank::CheckAABBCollision(const glm::vec3& pos, float radius, const AABB& box)
{
	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(pos.x, box.min.x, box.max.x);
	closestPoint.y = glm::clamp(pos.y, box.min.y, box.max.y);
	closestPoint.z = glm::clamp(pos.z, box.min.z, box.max.z);
	float distance = glm::distance(closestPoint, pos);
	return distance < radius;
}

void SceneTank::BuildCollisionBoxes()
{
	collisionBoxes.clear();

	// Back wall
	AABB backWall;
	backWall.min = glm::vec3(-10.f, 0.f, -7.65f);
	backWall.max = glm::vec3(10.f, 8.f, -7.35f);
	collisionBoxes.push_back(backWall);

	// Left wall
	AABB leftWall;
	leftWall.min = glm::vec3(-10.15f, 0.f, -7.5f);
	leftWall.max = glm::vec3(-9.85f, 8.f, 10.f);   // extend Z to 10 to match front wall
	collisionBoxes.push_back(leftWall);

	// Right wall
	AABB rightWall;
	rightWall.min = glm::vec3(9.85f, 0.f, -7.5f);
	rightWall.max = glm::vec3(10.15f, 8.f, 10.f);  // extend Z to 10 to match front wall
	collisionBoxes.push_back(rightWall);

	// Front wall left side
	AABB frontWallLeft;
	frontWallLeft.min = glm::vec3(-10.f, 0.f, 9.85f);
	frontWallLeft.max = glm::vec3(-1.f, 8.f, 10.15f);
	collisionBoxes.push_back(frontWallLeft);

	// Front wall right side
	AABB frontWallRight;
	frontWallRight.min = glm::vec3(1.f, 0.f, 9.85f);
	frontWallRight.max = glm::vec3(10.f, 8.f, 10.15f);
	collisionBoxes.push_back(frontWallRight);

	// floor
	AABB floor;
	floor.min = glm::vec3(-10.f, -0.5f, -7.5f);
	floor.max = glm::vec3(10.f, 0.1f, 10.f);
	collisionBoxes.push_back(floor);

	// door
	AABB doorBox;
	doorBox.min = glm::vec3(-1.f, 0.f, 9.85f);
	doorBox.max = glm::vec3(1.f, 4.f, 10.15f);
	doorBoxIndex = collisionBoxes.size();
	collisionBoxes.push_back(doorBox);

	// Ceiling
	AABB ceiling;
	ceiling.min = glm::vec3(-10.f, 7.9f, -7.5f);
	ceiling.max = glm::vec3(10.f, 8.1f, 10.f);  // extend to front wall
	collisionBoxes.push_back(ceiling);

	// Counter
	AABB counter;
	counter.min = glm::vec3(-7.5f, 0.f, 4.5f);
	counter.max = glm::vec3(7.5f, 2.0f, 5.5f);  // raised to match
	collisionBoxes.push_back(counter);

	// pillar
	AABB pillar;
	pillar.min = glm::vec3(2.f, 0.f, -0.5f);   // start from floor
	pillar.max = glm::vec3(3.f, 3.5f, 0.5f);   // lower from 6.f to 3.5f
	collisionBoxes.push_back(pillar);

	// Tank
	AABB tank;
	tank.min = glm::vec3(-1.65f, 0.f, -1.75f);
	tank.max = glm::vec3(1.85f, 4.f, 1.75f);
	collisionBoxes.push_back(tank);

	// Cabinet
	AABB cabinet;
	cabinet.min = glm::vec3(8.0f, 0.f, -7.f);
	cabinet.max = glm::vec3(9.5f, 4.5f, -3.f);
	collisionBoxes.push_back(cabinet);

	// Boxes
	AABB box1; box1.min = glm::vec3(-9.35f, 0.05f, -3.45f); box1.max = glm::vec3(-8.45f, 0.95f, -2.55f); collisionBoxes.push_back(box1);
	AABB box2; box2.min = glm::vec3(-9.35f, 1.11f, -3.45f); box2.max = glm::vec3(-8.45f, 2.01f, -2.55f); collisionBoxes.push_back(box2);
	AABB box3; box3.min = glm::vec3(-8.35f, 0.05f, -2.45f); box3.max = glm::vec3(-7.45f, 0.95f, -1.55f); collisionBoxes.push_back(box3);
	AABB box4; box4.min = glm::vec3(-8.35f, 0.05f, -4.45f); box4.max = glm::vec3(-7.45f, 0.95f, -3.55f); collisionBoxes.push_back(box4);
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
	glm::vec3 oldTarget = camera.target;

	// Update camera position based on input
	camera.Update(dt);

	bool nearBox = false;
	bool nearCabinet = false;

	glm::vec3 updatedPos = camera.position;

	// Vertical
	camera.position = glm::vec3(oldPos.x, updatedPos.y, oldPos.z);
	for (const AABB& box : collisionBoxes)
	{
		if (CheckAABBCollision(camera.position, 0.3f, box))
		{
			camera.position.y = oldPos.y;
			camera.target.y = oldTarget.y;
			break;
		}
	}

	// Horizontal
	float currentY = camera.position.y;
	camera.position = glm::vec3(updatedPos.x, currentY, updatedPos.z);
	for (const AABB& box : collisionBoxes)
	{
		if (CheckAABBCollision(camera.position, 0.3f, box))
		{
			camera.position.x = oldPos.x;
			camera.position.z = oldPos.z;
			camera.target.x = oldTarget.x;
			camera.target.z = oldTarget.z;
			break;
		}
	}

	showInteractPrompt = false;
	showLockedPrompt = false;  // add this

	glm::vec3 camForward = glm::normalize(camera.target - camera.position);

	// --- HOLDING ---
	if (ballState == HELD)
	{
		// Ball floats in front of camera
		glm::vec3 holdPos = camera.position + camForward * 1.2f + glm::vec3(0, -0.3f, 0);
		ballPhys.pos = Vector3(holdPos.x, holdPos.y, holdPos.z);
		ballPhys.vel = Vector3(0, 0, 0);
		ballPhys.accel = Vector3(0, 0, 0);

		// Charge power while holding left click
		if (MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
		{
			throwPower += static_cast<float>(dt) * 12.f;
			throwPower = glm::clamp(throwPower, 0.f, 20.f);
			isCharging = true;
		}
		else if (isCharging) // released
		{
			glm::vec3 throwDir = glm::normalize(camForward + glm::vec3(0, 0.1f, 0));
			ballPhys.vel = Vector3(throwDir.x * throwPower,
				throwDir.y * throwPower,
				throwDir.z * throwPower);
			ballPhys.accel = Vector3(0, -9.8f, 0); // gravity
			ballState = THROWN;
			isCharging = false;
			throwPower = 0.f;
		}

		//// Drop with E
		//if (KeyboardController::GetInstance()->IsKeyPressed('E'))
		//{
		//	ballPhys.pos = Vector3(ballRestPos.x, ballRestPos.y, ballRestPos.z);
		//	ballPhys.accel = Vector3(0, -9.8f, 0);
		//	ballState = THROWN;
		//}
	}

	// --- IN FLIGHT ---
	if (ballState == THROWN)
	{
		ballPhys.UpdatePhysics(static_cast<float>(dt));

		float r = 0.15f;

		// Floor
		if (ballPhys.pos.y < 0.25f)
		{
			ballPhys.pos.y = 0.25f;
			ballPhys.vel.y = -ballPhys.vel.y * ballPhys.bounciness;
			ballPhys.vel.x *= 0.85f;
			ballPhys.vel.z *= 0.85f;
			float speed = glm::length(glm::vec3(ballPhys.vel.x, ballPhys.vel.y, ballPhys.vel.z));
			if (speed < 0.3f)
			{
				ballPhys.vel = Vector3(0, 0, 0);
				ballPhys.accel = Vector3(0, 0, 0);
				ballState = ON_COUNTER;
			}
		}

		// define all AABBs
		for (const AABB& box : collisionBoxes)
		{
			Vector3 bMin(box.min.x, box.min.y, box.min.z);
			Vector3 bMax(box.max.x, box.max.y, box.max.z);

			if (!OverlapCircle2AABB(ballPhys.pos, r, bMin, bMax))
				continue;

			float bx = ballPhys.pos.x;
			float by = ballPhys.pos.y;
			float bz = ballPhys.pos.z;

			float penXR = bMax.x + r - bx;
			float penXL = bx - (bMin.x - r);
			float penYU = bMax.y + r - by;
			float penYD = by - (bMin.y - r);
			float penZF = bMax.z + r - bz;
			float penZB = bz - (bMin.z - r);

			float penX = (penXR < penXL) ? penXR : penXL;
			float penY = (penYU < penYD) ? penYU : penYD;
			float penZ = (penZF < penZB) ? penZF : penZB;

			if (penX < penY && penX < penZ)
			{
				if (penXR < penXL) { ballPhys.pos.x = bMax.x + r; ballPhys.vel.x = fabsf(ballPhys.vel.x) * ballPhys.bounciness; }
				else { ballPhys.pos.x = bMin.x - r; ballPhys.vel.x = -fabsf(ballPhys.vel.x) * ballPhys.bounciness; }
			}
			else if (penY < penZ)
			{
				if (penYU < penYD) { ballPhys.pos.y = bMax.y + r; ballPhys.vel.y = fabsf(ballPhys.vel.y) * ballPhys.bounciness; }
				else { ballPhys.pos.y = bMin.y - r; ballPhys.vel.y = -fabsf(ballPhys.vel.y) * ballPhys.bounciness; }
			}
			else
			{
				if (penZF < penZB) { ballPhys.pos.z = bMax.z + r; ballPhys.vel.z = fabsf(ballPhys.vel.z) * ballPhys.bounciness; }
				else { ballPhys.pos.z = bMin.z - r; ballPhys.vel.z = -fabsf(ballPhys.vel.z) * ballPhys.bounciness; }
			}
		}

		// Target AABB — tune these half-extents to match the visual size
		// Target is at (3.5, 3.0, 0), scaled 1.5x, so roughly 0.6 wide/tall
	
		float rad = glm::radians(targetRotation);

		// mirror exact same transforms as Render:
		float offsetX = 1.5f * cosf(rad);
		float offsetZ = -1.5f * sinf(rad);

		glm::vec3 targetCenter(2.5f + offsetX, 3.0f, 0.f + offsetZ);

		Vector3 targetMins(targetCenter.x - 0.7f, targetCenter.y - 0.6f, targetCenter.z - 0.1f);
		Vector3 targetMaxs(targetCenter.x + 0.7f, targetCenter.y + 0.6f, targetCenter.z + 0.1f);

		if (!targetHit && OverlapCircle2AABB(ballPhys.pos, r, targetMins, targetMaxs))
		{
			std::cout << "HIT! Ball at: "
				<< ballPhys.pos.x << " "
				<< ballPhys.pos.y << " "
				<< ballPhys.pos.z << std::endl;
			targetHit = true;
			targetRotation += 90.f;  // rotate backward, adjust to taste
			targetRotation = glm::clamp(targetRotation, 0.f, 90.f);  // stop at 90 degrees back

			// Same MTV bounce logic used for all other AABBs
			float bx = ballPhys.pos.x, by = ballPhys.pos.y, bz = ballPhys.pos.z;
			float penXR = targetMaxs.x + r - bx, penXL = bx - (targetMins.x - r);
			float penYU = targetMaxs.y + r - by, penYD = by - (targetMins.y - r);
			float penZF = targetMaxs.z + r - bz, penZB = bz - (targetMins.z - r);
			float penX = (penXR < penXL) ? penXR : penXL;
			float penY = (penYU < penYD) ? penYU : penYD;
			float penZ = (penZF < penZB) ? penZF : penZB;

			if (penX < penY && penX < penZ)
			{
				if (penXR < penXL) { ballPhys.pos.x = targetMaxs.x + r; ballPhys.vel.x = fabsf(ballPhys.vel.x) * ballPhys.bounciness; }
				else { ballPhys.pos.x = targetMins.x - r; ballPhys.vel.x = -fabsf(ballPhys.vel.x) * ballPhys.bounciness; }
			}
			else if (penY < penZ)
			{
				if (penYU < penYD) { ballPhys.pos.y = targetMaxs.y + r; ballPhys.vel.y = fabsf(ballPhys.vel.y) * ballPhys.bounciness; }
				else { ballPhys.pos.y = targetMins.y - r; ballPhys.vel.y = -fabsf(ballPhys.vel.y) * ballPhys.bounciness; }
			}
			else
			{
				if (penZF < penZB) { ballPhys.pos.z = targetMaxs.z + r; ballPhys.vel.z = fabsf(ballPhys.vel.z) * ballPhys.bounciness; }
				else { ballPhys.pos.z = targetMins.z - r; ballPhys.vel.z = -fabsf(ballPhys.vel.z) * ballPhys.bounciness; }
			}
		}
		// Clear hit flag once ball moves away
		if (targetHit && !OverlapCircle2AABB(ballPhys.pos, r + 0.3f, targetMins, targetMaxs))
			targetHit = false;
	}

	if (gameState == STATE_FIND_BALLS)
	{
		// hidden ball near boxes
		if (!hiddenBall1Found)
		{
			glm::vec3 boxPos(-8.9f, 0.5f, -3.f);
			float dist = glm::length(boxPos - camera.position);
			if (dist < 3.0f)
			{
				nearBox = true;
				hudMessage = "There seems to be something there...";
				if (KeyboardController::GetInstance()->IsKeyPressed('F'))
				{
					hiddenBall1Found = true;
					hudMessage = "Ball found! Go to the counter!";
					hudMessageTimer = 3.f;
				}
			}
		}

		// hidden ball near cabinet
		if (!hiddenBall2Found)
		{
			glm::vec3 cabinetPos(8.9f, 0.f, -5.f);
			float dist = glm::length(cabinetPos - camera.position);
			if (dist < 3.0f)
			{
				nearCabinet = true;
				hudMessage = "There seems to be something there...";
				if (KeyboardController::GetInstance()->IsKeyPressed('F'))
				{
					hiddenBall2Found = true;
					hudMessage = "Ball found! Go to the counter!";
					hudMessageTimer = 3.f;
				}
			}
		}

		if (hiddenBall1Found && hiddenBall2Found)
		{
			gameState = STATE_PLAYING;
			timerActive = true;  // start timer
		}
	}

	if (gameState == STATE_PLAYING)
	{
		// pickup
		if (ballState != HELD && !nearBox && !nearCabinet)
		{
			glm::vec3 ballPos(ballPhys.pos.x, ballPhys.pos.y, ballPhys.pos.z);
			float dist = glm::length(ballPos - camera.position);
			if (dist < 3.0f && KeyboardController::GetInstance()->IsKeyPressed('F'))
			{
				ballPhys.vel = Vector3(0, 0, 0);
				ballPhys.accel = Vector3(0, 0, 0);
				ballState = HELD;
			}
		}

		if (timerActive)
		{
			gameTimer -= static_cast<float>(dt);
			if (gameTimer <= 0.f)
			{
				gameTimer = 0.f;
				timerActive = false;
				gameState = STATE_LOST;  // add STATE_LOST to your enum
			}
		}

		// win condition
		if (targetRotation >= 90.f)
		{
			gameState = STATE_WON;
		}
	}

	if (targetRotation >= 90.f && !dummyFalling && !dummyInTank)
	{
		dummyFalling = true;
	}

	if (dummyFalling)
	{
		// Tip forward (rotate around X) and drop down
		//dummyFallAngle += static_cast<float>(dt) * 90.f;   // degrees per second
		dummyFallY -= static_cast<float>(dt) * 20.f;    // drop speed
		if (dummyFallY < -10.f)
			dummyFallY = -10.f;
		/*if (dummyFallAngle >= 90.f)
		{
			dummyFallAngle = 90.f;
			dummyFalling = false;
			dummyInTank = true;
		}*/
	}

	//Door interaction
	showInteractPrompt = false;
	showLockedPrompt = false;
	if (door.IsPlayerNear(camera.position, 2.5f))
	{
		if (gameState == STATE_WON)
			showInteractPrompt = true;
		else
			showLockedPrompt = true;
	}
	if (showInteractPrompt && KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		door.Open();
	}
	if (door.Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f))
	{
		SceneManager::GetInstance()->SwitchScene(door.leadsTo);
		door.Close();
	}

	// Remove door collision once it's open
	if (doorBoxIndex >= 0 && doorBoxIndex < (int)collisionBoxes.size())
	{
		if (door.rotation >= 85.f)
		{
			collisionBoxes[doorBoxIndex].min = glm::vec3(0);
			collisionBoxes[doorBoxIndex].max = glm::vec3(0);
		}
	}

	// only clear proximity message if not near anything AND no timed message running
	if (!nearBox && !nearCabinet && hudMessageTimer <= 0.f)
		hudMessage = "";

	// clear hud message after timer
	if (hudMessageTimer > 0.f)
	{
		hudMessageTimer -= static_cast<float>(dt);
		if (hudMessageTimer <= 0.f)
			hudMessage = "";
	}


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
	//modelStack.Translate(0.f, 0.f, 0.f);           // booth world origin

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
	modelStack.Scale(1.f, 0.3f, 1.35f);
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
	modelStack.Scale(0.3f, 8.f, 20.f);
	RenderMesh(meshList[GEO_WALL], true);       // reuses same mesh + material
	modelStack.PopMatrix();                     // <<< Left Wall
	// ---- RIGHT WALL ----
	modelStack.PushMatrix();                    // >>> Right Wall
	modelStack.Translate(10.f, 4.f, 0.f);
	modelStack.Scale(0.3f, 8.f, 20.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();                     // <<< Right Wall

	// front wall left side
	modelStack.PushMatrix();
	modelStack.Translate(-5.5f, 4.f, 10.f);
	modelStack.Scale(9.f, 8.f, 0.3f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// front wall right side
	modelStack.PushMatrix();
	modelStack.Translate(5.5f, 4.f, 10.f);
	modelStack.Scale(9.f, 8.f, 0.3f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// front wall above door
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 6.f, 10.f);
	modelStack.Scale(2.f, 4.0f, 0.3f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// door
	modelStack.PushMatrix();
	//modelStack.Translate(0.f, 2.f, 10.f);   // centered in door gap
	modelStack.Translate(door.position.x, door.position.y, door.position.z);
	modelStack.Rotate(door.rotation, 0.f, 1.f, 0.f);
	modelStack.Rotate(180.f, 0.f, 1.f, 0.f);   // rotate to face inward
	modelStack.Translate(door.width * 0.5f, 0.f, 0.f);   // pivot on left edge
	modelStack.Scale(2.f, 4.f, 0.3f);       // matches door gap width/height

	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.5f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_DOOR], true);
	modelStack.PopMatrix();

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

	modelStack.PushMatrix();
	modelStack.Translate(1.0f, 1.0f, 0.f - dummyFallY);  // drops as he falls
	modelStack.Rotate(-90.f, 1.f, 0.f, 0.f);  // tips forward

	meshList[GEO_DUMMY]->material.kAmbient = glm::vec3(0.3f, 0.2f, 0.1f);
	meshList[GEO_DUMMY]->material.kDiffuse = glm::vec3(0.8, 0.5, 0.3);
	meshList[GEO_DUMMY]->material.kSpecular = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DUMMY]->material.kShininess = 2.0f;

	RenderMesh(meshList[GEO_DUMMY], true);
	modelStack.PopMatrix();

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
	modelStack.Translate(2.5f, 3.5f, 0.f);        // pivot at pillar top (pillar x=2.5, top y=3.5)
	modelStack.Rotate(targetRotation, 0.f, 1.f, 0.f);  // Y axis spin like your screenshot
	modelStack.Translate(1.0f, -0.5f, 0.f);       // offset to where target hangs off pillar
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
	modelStack.Translate(0.f, 1.f, 5.f);  // raise Y from 0.5 to 1.f

	meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.05f);
	meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.55f, 0.35f, 0.15f);
	meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.15f, 0.1f);
	meshList[GEO_COUNTER]->material.kShininess = 8.f;

	RenderMesh(meshList[GEO_COUNTER], true);
	modelStack.PopMatrix();
	
	// bomb
	modelStack.PushMatrix();
	modelStack.Translate(2.5f, 2.3f, 5.f);
	meshList[GEO_BOMB]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BOMB]->material.kDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
	meshList[GEO_BOMB]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BOMB]->material.kShininess = 5.0f;

	RenderMesh(meshList[GEO_BOMB], true);
	modelStack.PopMatrix();

	// balls
	modelStack.PushMatrix();
	modelStack.Translate(ballPhys.pos.x, ballPhys.pos.y, ballPhys.pos.z);
	modelStack.Scale(0.3f, 0.3f, 0.3f);

	meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.25f, 0.22f, 0.18f);
	meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.9f, 0.85f, 0.7f);   // off-white leather
	meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.15f, 0.15f, 0.15f); // slightly shiny
	meshList[GEO_BALL]->material.kShininess = 8.0f;
	
	RenderMesh(meshList[GEO_BALL], true);
	modelStack.PopMatrix();

	if (hiddenBall1Found)
	{
		modelStack.PushMatrix();
		modelStack.Translate(1.5f, 2.15f, 5.f);  // on counter next to main ball
		modelStack.Scale(0.3f, 0.3f, 0.3f);
		meshList[GEO_BALL2]->material.kAmbient = glm::vec3(0.25f, 0.22f, 0.18f);
		meshList[GEO_BALL2]->material.kDiffuse = glm::vec3(0.9f, 0.85f, 0.7f);
		meshList[GEO_BALL2]->material.kSpecular = glm::vec3(0.15f, 0.15f, 0.15f);
		meshList[GEO_BALL2]->material.kShininess = 8.0f;
		RenderMesh(meshList[GEO_BALL2], true);
		modelStack.PopMatrix();
	}

	if (hiddenBall2Found)
	{
		modelStack.PushMatrix();
		modelStack.Translate(-1.5f, 2.15f, 5.f);  // on counter other side
		modelStack.Scale(0.3f, 0.3f, 0.3f);
		meshList[GEO_BALL3]->material.kAmbient = glm::vec3(0.25f, 0.22f, 0.18f);
		meshList[GEO_BALL3]->material.kDiffuse = glm::vec3(0.9f, 0.85f, 0.7f);
		meshList[GEO_BALL3]->material.kSpecular = glm::vec3(0.15f, 0.15f, 0.15f);
		meshList[GEO_BALL3]->material.kShininess = 8.0f;
		RenderMesh(meshList[GEO_BALL3], true);
		modelStack.PopMatrix();
	}


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

	// HUD text

	// permanent top instruction
	// HUD text — state based
	if (gameState == STATE_FIND_BALLS)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Find the hidden balls to start!",
			glm::vec3(1.f, 1.f, 0.f), 23, 40, 560);
	}

	if (gameState == STATE_PLAYING)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Hit the target to defuse bomb!",
			glm::vec3(0.3f, 1.f, 0.3f), 23, 40, 560);

		// timer
		char timerBuf[32];
		sprintf_s(timerBuf, "TIME: %.1f", gameTimer);
		glm::vec3 timerColor = (gameTimer <= 10.f) ? glm::vec3(1, 0, 0) : glm::vec3(1, 1, 1);
		RenderTextOnScreen(meshList[GEO_TEXT], timerBuf, timerColor, 30, 250, 530);

		if (ballState == ON_COUNTER)
		{
			glm::vec3 ballPos(ballPhys.pos.x, ballPhys.pos.y, ballPhys.pos.z);
			float dist = glm::length(ballPos - camera.position);
			if (dist < 3.0f)
				RenderTextOnScreen(meshList[GEO_TEXT], "[F] Pick up ball",
					glm::vec3(1, 1, 0), 30, 50, 70);
		}

		if (ballState == HELD)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "Hold LMB to charge throw",
				glm::vec3(1, 1, 1), 30, 50, 100);

			int bars = (int)(throwPower / 20.f * 10.f);
			std::string chargeBar = "Power: [";
			for (int i = 0; i < 10; i++)
				chargeBar += (i < bars) ? "#" : "-";
			chargeBar += "]";
			RenderTextOnScreen(meshList[GEO_TEXT], chargeBar,
				glm::vec3(1, 0.3f, 0.3f), 30, 120, 60);
		}

		if (ballState == THROWN)
			RenderTextOnScreen(meshList[GEO_TEXT], "Ball in flight!",
				glm::vec3(0.3f, 1.f, 0.3f), 30, 200, 100);
	
	}

	if (gameState == STATE_LOST)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Time's up! Bomb exploded!",
			glm::vec3(1.f, 0.f, 0.f), 25, 150, 300);
		RenderTextOnScreen(meshList[GEO_TEXT], "[R] Try Again",
			glm::vec3(1.f, 1.f, 0.f), 25, 250, 250);
	}

	if (gameState == STATE_WON)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "You win! Head to the exit!",
			glm::vec3(0.f, 1.f, 0.f), 25, 75, 300);
	}

	if (!hudMessage.empty())
		RenderTextOnScreen(meshList[GEO_TEXT], hudMessage,
			glm::vec3(1, 1, 0), 20, 50, 270);

	// Door interaction prompts
	if (showInteractPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "[F] Exit", glm::vec3(1.f, 1.f, 0.f), 30, 50, 30);
	else if (showLockedPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT], "Win the game first!", glm::vec3(1.f, 0.f, 0.f), 30, 50, 30);

	modelStack.PopMatrix();                         // <<< BOOTH ROOT

	for (const AABB& box : collisionBoxes)
	{
		glm::vec3 center = (box.min + box.max) * 0.5f;
		glm::vec3 size = box.max - box.min;
		modelStack.PushMatrix();
		modelStack.Translate(center.x, center.y, center.z);
		modelStack.Scale(size.x, size.y, size.z);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
		RenderMesh(meshList[GEO_CUBE], false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		modelStack.PopMatrix();
	}
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

	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		if (gameState == STATE_LOST)
		{
			// reset game state
			gameState = STATE_FIND_BALLS;
			gameTimer = 30.f;
			timerActive = false;
			hiddenBall1Found = false;
			hiddenBall2Found = false;
			targetRotation = 0.f;
			targetHit = false;
			dummyFalling = false;
			dummyInTank = false;
			dummyFallY = 0.f;
			ballState = ON_COUNTER;
			ballPhys.pos = Vector3(ballRestPos.x, ballRestPos.y, ballRestPos.z);
			ballPhys.vel = Vector3(0, 0, 0);
			ballPhys.accel = Vector3(0, 0, 0);
		}
	}
}

void SceneTank::HandleMouseInput() {
	//static bool isLeftUp = false;
	//static bool isRightUp = false;

	//// Process Left button
	//if (!isLeftUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	//{
	//	isLeftUp = true;
	//	std::cout << "LBUTTON DOWN" << std::endl;

	//	// transform into UI space
	//	double x = MouseController::GetInstance()->GetMousePositionX();
	//	double y = 1080 - MouseController::GetInstance()->GetMousePositionY();

	//	// Check if mouse click position is within the GUI box
	//	// Change the boundaries as necessary
	//	if (x > 0 && x < 100 && y > 0 && y < 100) {
	//		std::cout << "GUI IS CLICKED" << std::endl;
	//	}

	//}
	//else if (isLeftUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	//{
	//	isLeftUp = false;
	//	std::cout << "LBUTTON UP" << std::endl;
	//}

	//// Continue to do for right button
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