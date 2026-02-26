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
#include <cstdlib>  
#include <ctime>     
#include <cmath>
#include <cstdio>    

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "LoadTGA.h"
#include "SceneManager.h"

// Forward declare the global GLFW window from Application.cpp
extern GLFWwindow* m_window;

// Random float in [lo, hi]
static float RandRange(float lo, float hi)
{
	return lo + (hi - lo) * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
}

SceneDucks::SceneDucks()
{
}
SceneDucks::~SceneDucks()
{
}

void SceneDucks::InitDucks()
{
	srand(static_cast<unsigned>(time(nullptr)));

	for (int i = 0; i < NUM_DUCKS; ++i)
	{
		Duck& d = ducks[i];

		float angle = glm::two_pi<float>() * i / NUM_DUCKS + RandRange(-0.3f, 0.3f);
		float radius = RandRange(0.3f, POOL_RADIUS * 0.7f);

		d.pos = glm::vec3(radius * cosf(angle), 1.5f, radius * sinf(angle));
		d.caught = false;
		d.bobOffset = RandRange(0.f, glm::two_pi<float>());
		d.bobTimer = 0.f;

		float vAngle = RandRange(0.f, glm::two_pi<float>());
		float speed = RandRange(0.5f, 1.2f);
		d.velocity = glm::vec2(cosf(vAngle) * speed, sinf(vAngle) * speed);
		d.facingAngle = glm::degrees(vAngle);

		d.isCorrect = false;
	}

	int indices[NUM_DUCKS];
	for (int i = 0; i < NUM_DUCKS; ++i) indices[i] = i;
	for (int i = 0; i < MAX_DUCKS; ++i)
	{
		int j = i + rand() % (NUM_DUCKS - i);
		std::swap(indices[i], indices[j]);
		ducks[indices[i]].isCorrect = true;
	}
}

void SceneDucks::Init()
{
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	glUseProgram(m_programID);

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

	camera.Init(
		glm::vec3(0, 1.8f, 10),
		glm::vec3(0, 1.7f, 0),
		glm::vec3(0, 1.0f, 0)
	);

	for (int i = 0; i < NUM_GEOMETRY; ++i)
		meshList[i] = nullptr;

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 0000.f, 0000.f, 0000.f);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sun", glm::vec3(1.f, 1.f, 1.f), 1.f, 16, 16);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Arm", glm::vec3(0.5f, 0.5f, 0.5f), 1.f);

	meshList[GEO_POOL] = MeshBuilder::GenerateOBJ("Pool", "Models//pool.obj");
	meshList[GEO_POOL]->textureID = LoadTGA("Images//pool1.tga");
	meshList[GEO_WATER] = MeshBuilder::GenerateSphere("Water", glm::vec3(0.1f, 0.4f, 0.8f), 1.f, 32);
	meshList[GEO_WATER]->textureID = LoadTGA("Images//water.tga");
	meshList[GEO_DUCK] = MeshBuilder::GenerateOBJ("Duck", "Models//duck1.obj");
	meshList[GEO_DUCKLEYE] = MeshBuilder::GenerateSphere("LeftDuckEye", glm::vec3(0.0f, 0.0f, 0.0f), 1.f, 32);
	meshList[GEO_DUCKREYE] = MeshBuilder::GenerateSphere("RightDuckEye", glm::vec3(0.0f, 0.0f, 0.0f), 1.f, 32);
	meshList[GEO_PEGHOOK] = MeshBuilder::GenerateOBJ("Peghook", "Models//peghook.obj");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_GUI] = MeshBuilder::GenerateQuad("GUI", glm::vec3(1, 1, 1), 1.f);

	meshList[GEO_FLOOR] = MeshBuilder::GenerateRectangularPrism("Floor", glm::vec3(0.45f, 0.32f, 0.18f), 20.f, 0.2f, 15.f);
	meshList[GEO_CEILING] = MeshBuilder::GenerateRectangularPrism("Ceiling", glm::vec3(0.85f, 0.75f, 0.55f), 20.f, 0.2f, 15.f);
	meshList[GEO_CEILING]->textureID = LoadTGA("Images//carnivalwallpaper2.tga");
	meshList[GEO_WALL] = MeshBuilder::GenerateRectangularPrism("Wall", glm::vec3(0.9f, 0.85f, 0.6f), 1.f, 1.f, 1.f);
	meshList[GEO_WALL]->textureID = LoadTGA("Images//carnivalwallpaper.tga");
	meshList[GEO_COUNTER] = MeshBuilder::GenerateRectangularPrism("Counter", glm::vec3(0.55f, 0.35f, 0.15f), 20.f, 1.0f, 0.4f);
	meshList[GEO_DOOR] = MeshBuilder::GenerateCube("Door", glm::vec3(1.f, 1.f, 1.f), 1.f);

	meshList[GEO_BALLOON] = MeshBuilder::GenerateOBJ("Balloon", "Models//balloon.obj");
	meshList[GEO_CRATE] = MeshBuilder::GenerateOBJ("Crate", "Models//cratebig.obj");
	meshList[GEO_CRATE1] = MeshBuilder::GenerateOBJ("Crate1", "Models//crate.obj");
	meshList[GEO_DUCKTABLE] = MeshBuilder::GenerateOBJ("Ducktable", "Models//ducktable.obj");
	meshList[GEO_DUCKTABLE]->textureID = LoadTGA("Images//ducktable.tga");
	meshList[GEO_TOYTRAIN] = MeshBuilder::GenerateOBJ("Toytrain", "Models//toytrain.obj");
	meshList[GEO_TOYTRAIN]->textureID = LoadTGA("Images//toytrain.tga");
	meshList[GEO_TOYPLANE] = MeshBuilder::GenerateOBJ("Toyplane", "Models//toyplane.obj");
	meshList[GEO_TOYPLANE]->textureID = LoadTGA("Images//toyplane.tga");
	meshList[GEO_DUCKBASKETBALL] = MeshBuilder::GenerateOBJ("Duckbasketball", "Models//duckbasketball.obj");

	glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	playerSize = glm::vec3(0.4f, 1.8f, 0.4f);

	// Game state
	gameState = STATE_FIND_HOOK;
	hookPickedUp = false;
	hookWorldPos = glm::vec3(9.5, -1.85f, 10.5f);
	ducksPickedUp = 0;
	catchTimer = 0.f;
	bombTimer = 30.0f;

	//1.f, 2.f, 17.5f), 2.f, 3.75f

	// Door setup — position at the back doorway (z = 17.5)
	door[0] = { glm::vec3(0.f, 1.f, 0.f), 2.f, 3.75f, SceneManager::SCENE_LOBBY };
	// Door prompt flags
	showInteractPrompt = false;
	showLockedPrompt = false;

	InitDucks();
	BuildCollisionBoxes();

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	light[0].position = glm::vec3(0, 5, 3);
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
	glm::vec3 diff = camera.position - hookWorldPos;
	return glm::dot(diff, diff) <= radius * radius;
}

bool SceneDucks::IsPlayerNearPool(float radius)
{
	glm::vec3 poolCentre(0.f, -1.5f, 0.f);
	glm::vec3 diff = camera.position - poolCentre;
	return glm::dot(diff, diff) <= radius * radius;
}

void SceneDucks::UpdateDucks(float dt)
{
	for (int i = 0; i < NUM_DUCKS; ++i)
	{
		Duck& d = ducks[i];
		if (d.caught) continue;

		d.bobTimer += dt;
		float bob = sinf(d.bobTimer * 2.f + d.bobOffset) * 0.08f;
		d.pos.y = 1.5f + bob;

		d.pos.x += d.velocity.x * dt;
		d.pos.z += d.velocity.y * dt;

		float distXZ = sqrtf(d.pos.x * d.pos.x + d.pos.z * d.pos.z);
		if (distXZ > POOL_RADIUS - DUCK_RADIUS)
		{
			glm::vec2 normal(-d.pos.x / distXZ, -d.pos.z / distXZ);

			float dot = d.velocity.x * normal.x + d.velocity.y * normal.y;
			d.velocity.x -= 2.f * dot * normal.x;
			d.velocity.y -= 2.f * dot * normal.y;

			float overlap = distXZ - (POOL_RADIUS - DUCK_RADIUS);
			d.pos.x += normal.x * overlap;
			d.pos.z += normal.y * overlap;

			float wobble = RandRange(-0.3f, 0.3f);
			float speed = sqrtf(d.velocity.x * d.velocity.x + d.velocity.y * d.velocity.y);
			float newAngle = atan2f(d.velocity.y, d.velocity.x) + wobble;
			d.velocity = glm::vec2(cosf(newAngle) * speed, sinf(newAngle) * speed);
		}

		if (glm::length(d.velocity) > 0.01f)
			d.facingAngle = glm::degrees(atan2f(d.velocity.x, d.velocity.y));
	}

	for (int i = 0; i < NUM_DUCKS; ++i)
	{
		if (ducks[i].caught) continue;
		for (int j = i + 1; j < NUM_DUCKS; ++j)
		{
			if (ducks[j].caught) continue;

			float dx = ducks[j].pos.x - ducks[i].pos.x;
			float dz = ducks[j].pos.z - ducks[i].pos.z;
			float dist = sqrtf(dx * dx + dz * dz);
			float minDist = DUCK_RADIUS * 2.f;

			if (dist < minDist && dist > 0.001f)
			{
				float nx = dx / dist;
				float nz = dz / dist;

				float overlap = (minDist - dist) * 0.5f;
				ducks[i].pos.x -= nx * overlap;
				ducks[i].pos.z -= nz * overlap;
				ducks[j].pos.x += nx * overlap;
				ducks[j].pos.z += nz * overlap;

				float vi_n = ducks[i].velocity.x * nx + ducks[i].velocity.y * nz;
				float vj_n = ducks[j].velocity.x * nx + ducks[j].velocity.y * nz;

				if (vi_n - vj_n > 0.f)
				{
					ducks[i].velocity.x += (vj_n - vi_n) * nx;
					ducks[i].velocity.y += (vj_n - vi_n) * nz;
					ducks[j].velocity.x += (vi_n - vj_n) * nx;
					ducks[j].velocity.y += (vi_n - vj_n) * nz;
				}
			}
		}
	}
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

	glm::vec3 oldPos = camera.position;
	glm::vec3 oldTarget = camera.target;

	if (!isTopDown)
		camera.Update(dt);

	glm::vec3 updatedPos = camera.position;

	// Vertical collision (floor/ceiling)
	camera.position = glm::vec3(oldPos.x, updatedPos.y, oldPos.z);
	for (const DAABB& box : collisionBoxes)
	{
		if (CheckDAABBCollision(camera.position, 0.3f, box))
		{
			camera.position.y = oldPos.y;
			camera.target.y = oldTarget.y;
			break;
		}
	}

	// Horizontal collision (walls)
	float currentY = camera.position.y;
	camera.position = glm::vec3(updatedPos.x, currentY, updatedPos.z);
	for (const DAABB& box : collisionBoxes)
	{
		if (CheckDAABBCollision(camera.position, 0.3f, box))
		{
			camera.position.x = oldPos.x;
			camera.position.z = oldPos.z;
			camera.target.x = oldTarget.x;
			camera.target.z = oldTarget.z;
			break;
		}
	}

	if (catchTimer > 0.f)
		catchTimer -= static_cast<float>(dt);
	if (wrongTimer > 0.f)
		wrongTimer -= static_cast<float>(dt);

	UpdateDucks(static_cast<float>(dt));

	// Bomb timer
	if (gameState == STATE_PLAYING)
	{
		bombTimer -= static_cast<float>(dt);
		if (bombTimer <= 0.f)
		{
			bombTimer = 0.f;
			gameState = STATE_LOST;
		}
	}

	// Door interaction
	showInteractPrompt = false;
	showLockedPrompt = false;

	glm::vec3 doorwayPos = glm::vec3(0.f, 2.f, 15.f); // adjust Z to match where the doorway opening is

	if (glm::distance(camera.position, doorwayPos) < 2.5f)
	{
		if (gameState == STATE_WON)
			showInteractPrompt = true;
		else
			showLockedPrompt = true;
	}
	if (showInteractPrompt && KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		SceneManager::GetInstance()->SwitchScene(door[0].leadsTo);
	}
	if (door[0].Update(dt, camera.position, playerSize.x * 0.5f, playerSize.z * 0.5f))
	{
		SceneManager::GetInstance()->SwitchScene(door[0].leadsTo);
		door[0].Close();
		showInteractPrompt = false;
	}
}

void SceneDucks::RenderDucks()
{
	for (int i = 0; i < NUM_DUCKS; ++i)
	{
		const Duck& d = ducks[i];
		if (d.caught) continue;

		modelStack.PushMatrix();
		modelStack.Translate(d.pos.x, -1.2, d.pos.z);
		modelStack.Rotate(d.facingAngle, 0.f, 1.f, 0.f);
		modelStack.Scale(0.01f, 0.01f, 0.01f);

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

		modelStack.PopMatrix();
	}
}

void SceneDucks::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);

	modelStack.LoadIdentity();

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
	{
		glm::vec3 ld(light[0].position.x, light[0].position.y, light[0].position.z);
		glm::vec3 ldc = viewStack.Top() * glm::vec4(ld, 0);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(ldc));
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		glm::vec3 lp = viewStack.Top() * glm::vec4(light[0].position, 1);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lp));
		glm::vec3 sd = viewStack.Top() * glm::vec4(light[0].spotDirection, 0);
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, glm::value_ptr(sd));
	}
	else
	{
		glm::vec3 lp = viewStack.Top() * glm::vec4(light[0].position, 1);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lp));
	}

	modelStack.PushMatrix();
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(light[0].position.x, light[0].position.y, light[0].position.z);
	modelStack.Scale(0.1f, 0.1f, 0.1f);
	RenderMesh(meshList[GEO_SPHERE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();

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
	modelStack.Translate(0.f, 6.f, 4.7f);
	modelStack.Scale(1.02f, 0.9f, 1.7f);
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
	modelStack.Translate(-10.f, 2.f, 5.f);
	modelStack.Scale(0.3f, 8.f, 25.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// RIGHT WALL
	modelStack.PushMatrix();
	modelStack.Translate(10.f, 2.f, 5.f);
	modelStack.Scale(0.3f, 8.f, 25.f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// back wall L
	modelStack.PushMatrix();
	modelStack.Translate(5.5f, 1.5f, 17.5f);
	modelStack.Scale(9.f, 9.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// back wall R
	modelStack.PushMatrix();
	modelStack.Translate(-5.5f, 1.5f, 17.5f);
	modelStack.Scale(9.f, 9.f, 0.3f);
	meshList[GEO_WALL]->material.kAmbient = glm::vec3(0.3f, 0.25f, 0.15f);
	meshList[GEO_WALL]->material.kDiffuse = glm::vec3(0.85f, 0.75f, 0.5f);
	meshList[GEO_WALL]->material.kSpecular = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_WALL]->material.kShininess = 2.f;
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// door frame
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 4.8f, 17.5f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	modelStack.Scale(0.29f, 4.24f, 3.9f);
	RenderMesh(meshList[GEO_WALL], true);
	modelStack.PopMatrix();

	// render main door
	modelStack.PushMatrix();
	modelStack.Translate(door[0].position.x, door[0].position.y, door[0].position.z);
	modelStack.Rotate(door[0].rotation, 0, 1, 0);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Translate(0, 0.5f, -17.5f);
	modelStack.Scale(door[0].width * 1.35, door[0].height * 1.85, 0.2f);
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.05f, 0.05f, 0.05f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.3f, 0.3f, 0.3f);
	RenderMesh(meshList[GEO_DOOR], true);
	modelStack.PopMatrix();

	// POOL
	modelStack.PushMatrix();
	modelStack.Translate(0.f, -1.5f, 0.f);
	modelStack.Scale(0.2f, 0.2f, 0.2f);
	meshList[GEO_POOL]->material.kAmbient = glm::vec3(0.1f, 0.2f, 0.3f);
	meshList[GEO_POOL]->material.kDiffuse = glm::vec3(0.2f, 0.5f, 0.8f);
	meshList[GEO_POOL]->material.kSpecular = glm::vec3(0.3f, 0.5f, 0.7f);
	meshList[GEO_POOL]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_POOL], true);

	// WATER SURFACE
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 1.0f, 0.f);
	modelStack.Scale(18.f, 1.f, 18.f);
	meshList[GEO_WATER]->material.kAmbient = glm::vec3(0.0f, 0.2f, 0.5f);
	meshList[GEO_WATER]->material.kDiffuse = glm::vec3(0.1f, 0.4f, 0.8f);
	meshList[GEO_WATER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 1.0f);
	meshList[GEO_WATER]->material.kShininess = 64.f;
	RenderMesh(meshList[GEO_WATER], true);
	modelStack.PopMatrix();

	modelStack.PopMatrix(); // POOL ROOT

	RenderDucks();

	modelStack.PopMatrix(); // BOOTH ROOT

	// BALLOON
	modelStack.PushMatrix();
	modelStack.Translate(7.f, 1.0f, -6.f);
	modelStack.Scale(0.4f, 0.4f, 0.4f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	meshList[GEO_BALLOON]->material.kAmbient = glm::vec3(0.0f, 0.2f, 0.5f);
	meshList[GEO_BALLOON]->material.kDiffuse = glm::vec3(0.1f, 0.4f, 0.8f);
	meshList[GEO_BALLOON]->material.kSpecular = glm::vec3(0.9f, 0.9f, 1.0f);
	meshList[GEO_BALLOON]->material.kShininess = 64.f;
	RenderMesh(meshList[GEO_BALLOON], true);
	modelStack.PopMatrix();

	// BALLOON
	modelStack.PushMatrix();
	modelStack.Translate(-7.f, 1.0f, -6.f);
	modelStack.Scale(0.4f, 0.4f, 0.4f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	meshList[GEO_BALLOON]->material.kAmbient = glm::vec3(0.0f, 0.2f, 0.5f);
	meshList[GEO_BALLOON]->material.kDiffuse = glm::vec3(0.1f, 0.4f, 0.8f);
	meshList[GEO_BALLOON]->material.kSpecular = glm::vec3(0.9f, 0.9f, 1.0f);
	meshList[GEO_BALLOON]->material.kShininess = 64.f;
	RenderMesh(meshList[GEO_BALLOON], true);
	modelStack.PopMatrix();

	// BALLOON
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 1.0f, -6.f);
	modelStack.Scale(0.4f, 0.4f, 0.4f);
	modelStack.Rotate(-90.f, 0.f, 1.f, 0.f);
	meshList[GEO_BALLOON]->material.kAmbient = glm::vec3(0.4f, 0.0f, 0.0f);
	meshList[GEO_BALLOON]->material.kDiffuse = glm::vec3(1.0f, 0.1f, 0.1f);
	meshList[GEO_BALLOON]->material.kSpecular = glm::vec3(1.0f, 0.8f, 0.8f);
	meshList[GEO_BALLOON]->material.kShininess = 64.f;
	RenderMesh(meshList[GEO_BALLOON], true);
	modelStack.PopMatrix();

	// CRATE
	modelStack.PushMatrix();
	modelStack.Translate(8.6f, -1.85f, 7.f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(0.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.07f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(0.55f, 0.27f, 0.07f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.2f, 0.1f, 0.05f);
	meshList[GEO_CRATE]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	// CRATE
	modelStack.PushMatrix();
	modelStack.Translate(7.5f, -1.85f, 10.6f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(-73.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.07f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(0.55f, 0.27f, 0.07f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.2f, 0.1f, 0.05f);
	meshList[GEO_CRATE]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	// CRATE
	modelStack.PushMatrix();
	modelStack.Translate(7.5f, 0.8f, 8.6f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(113.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.07f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(0.55f, 0.27f, 0.07f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.2f, 0.1f, 0.05f);
	meshList[GEO_CRATE]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	// CRATE1
	modelStack.PushMatrix();
	modelStack.Translate(6.5f, -1.8f, 8.2f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE1]->material.kAmbient = glm::vec3(0.35f, 0.22f, 0.1f);
	meshList[GEO_CRATE1]->material.kDiffuse = glm::vec3(0.7f, 0.45f, 0.2f);
	meshList[GEO_CRATE1]->material.kSpecular = glm::vec3(0.15f, 0.1f, 0.05f);
	meshList[GEO_CRATE1]->material.kShininess = 12.f;
	RenderMesh(meshList[GEO_CRATE1], true);
	modelStack.PopMatrix();

	// CRATE
	modelStack.PushMatrix();
	modelStack.Translate(-8.6f, -1.85f, 12.f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(0.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.07f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(0.55f, 0.27f, 0.07f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.2f, 0.1f, 0.05f);
	meshList[GEO_CRATE]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	// CRATE
	modelStack.PushMatrix();
	modelStack.Translate(-7.5f, -1.85f, 15.6f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(-73.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.07f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(0.55f, 0.27f, 0.07f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.2f, 0.1f, 0.05f);
	meshList[GEO_CRATE]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	// CRATE
	modelStack.PushMatrix();
	modelStack.Translate(-7.5f, 0.8f, 13.6f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(113.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.25f, 0.15f, 0.07f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(0.55f, 0.27f, 0.07f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.2f, 0.1f, 0.05f);
	meshList[GEO_CRATE]->material.kShininess = 16.f;
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	// CRATE1
	modelStack.PushMatrix();
	modelStack.Translate(-6.5f, -1.8f, 12.9f);
	modelStack.Scale(3.0f, 3.0f, 3.0f);
	modelStack.Rotate(90.f, 0.f, 1.f, 0.f);
	meshList[GEO_CRATE1]->material.kAmbient = glm::vec3(0.35f, 0.22f, 0.1f);
	meshList[GEO_CRATE1]->material.kDiffuse = glm::vec3(0.7f, 0.45f, 0.2f);
	meshList[GEO_CRATE1]->material.kSpecular = glm::vec3(0.15f, 0.1f, 0.05f);
	meshList[GEO_CRATE1]->material.kShininess = 12.f;
	RenderMesh(meshList[GEO_CRATE1], true);
	modelStack.PopMatrix();

	// TABLE
	modelStack.PushMatrix();
	modelStack.Translate(-8.2f, -0.5f, 2.2f);
	modelStack.Scale(0.025f, 0.025f, 0.025f);
	modelStack.Rotate(0.f, 0.f, 1.f, 0.f);
	meshList[GEO_DUCKTABLE]->material.kAmbient = glm::vec3(0.35f, 0.22f, 0.1f);
	meshList[GEO_DUCKTABLE]->material.kDiffuse = glm::vec3(0.7f, 0.45f, 0.2f);
	meshList[GEO_DUCKTABLE]->material.kSpecular = glm::vec3(0.15f, 0.1f, 0.05f);
	meshList[GEO_DUCKTABLE]->material.kShininess = 12.f;
	RenderMesh(meshList[GEO_DUCKTABLE], true);
	modelStack.PopMatrix();

	// Toy train
	modelStack.PushMatrix();
	modelStack.Translate(-8.2f, 0.75f, 4.2f);
	modelStack.Scale(0.04f, 0.04f, 0.04f);
	modelStack.Rotate(156.f, 0.f, 1.f, 0.f);
	meshList[GEO_TOYTRAIN]->material.kAmbient = glm::vec3(0.35f, 0.22f, 0.1f);
	meshList[GEO_TOYTRAIN]->material.kDiffuse = glm::vec3(0.7f, 0.45f, 0.2f);
	meshList[GEO_TOYTRAIN]->material.kSpecular = glm::vec3(0.15f, 0.1f, 0.05f);
	meshList[GEO_TOYTRAIN]->material.kShininess = 12.f;
	RenderMesh(meshList[GEO_TOYTRAIN], true);
	modelStack.PopMatrix();

	// Toy plane
	modelStack.PushMatrix();
	modelStack.Translate(-9.2f, 0.75f, 2.8f);
	modelStack.Scale(0.03f, 0.03f, 0.03f);
	modelStack.Rotate(230.f, 0.f, 1.f, 0.f);
	meshList[GEO_TOYPLANE]->material.kAmbient = glm::vec3(0.35f, 0.22f, 0.1f);
	meshList[GEO_TOYPLANE]->material.kDiffuse = glm::vec3(0.7f, 0.45f, 0.2f);
	meshList[GEO_TOYPLANE]->material.kSpecular = glm::vec3(0.15f, 0.1f, 0.05f);
	meshList[GEO_TOYPLANE]->material.kShininess = 12.f;
	RenderMesh(meshList[GEO_TOYPLANE], true);
	modelStack.PopMatrix();

	// Basketball
	modelStack.PushMatrix();
	modelStack.Translate(-7.2f, 1.1f, 0.8f);
	modelStack.Scale(3.03f, 3.03f, 3.03f);
	modelStack.Rotate(0.f, 0.f, 1.f, 0.f);
	meshList[GEO_DUCKBASKETBALL]->material.kAmbient = glm::vec3(0.35f, 0.22f, 0.1f);
	meshList[GEO_DUCKBASKETBALL]->material.kDiffuse = glm::vec3(0.7f, 0.45f, 0.2f);
	meshList[GEO_DUCKBASKETBALL]->material.kSpecular = glm::vec3(0.15f, 0.1f, 0.05f);
	meshList[GEO_DUCKBASKETBALL]->material.kShininess = 12.f;
	RenderMesh(meshList[GEO_DUCKBASKETBALL], true);
	modelStack.PopMatrix();

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

	// ==================== HUD ====================

	if (gameState == STATE_FIND_HOOK)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"Find the hook!", glm::vec3(1, 1, 0), 30.f, 90.f, 540.f);
		if (IsPlayerNearHook(5.0f))
			RenderTextOnScreen(meshList[GEO_TEXT],
				"[F] Pick up Hook", glm::vec3(1, 1, 1), 30.f, 180.f, 180.f);
	}

	if (gameState == STATE_PLAYING)
	{
		char buf[32];
		sprintf_s(buf, "Ducks: %d / %d", ducksPickedUp, MAX_DUCKS);
		RenderTextOnScreen(meshList[GEO_TEXT], buf, glm::vec3(1, 1, 1), 30.f, 30.f, 560.f);

		if (!isTopDown && IsPlayerNearPool(7.f))
			RenderTextOnScreen(meshList[GEO_TEXT],
				"[E] Play Duck Game", glm::vec3(1, 1, 0), 30.f, 130.f, 60.f);

		if (isTopDown)
			RenderTextOnScreen(meshList[GEO_TEXT],
				"[E] Exit Duck Game", glm::vec3(1, 1, 0), 30.f, 130.f, 60.f);

		if (catchTimer > 0.f)
			RenderTextOnScreen(meshList[GEO_TEXT],
				"GOT ONE!", glm::vec3(0, 1, 0), 50.f, 300.f, 400.f);

		if (wrongTimer > 0.f)
			RenderTextOnScreen(meshList[GEO_TEXT],
				"WRONG DUCK!", glm::vec3(1, 0, 0), 50.f, 260.f, 400.f);

		// Bomb timer
		int  minutes = (int)(bombTimer / 60.f);
		int  seconds = (int)(bombTimer) % 60;
		char timerBuf[32];
		sprintf_s(timerBuf, "TIME: %d:%02d", minutes, seconds);
		glm::vec3 timerColor = (bombTimer <= 10.f) ? glm::vec3(1, 0, 0) : glm::vec3(1, 1, 1);
		RenderTextOnScreen(meshList[GEO_TEXT], timerBuf, timerColor, 30.f, 30.f, 120.f);
	}

	if (gameState == STATE_WON && isTopDown)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"BOMB DEFUSED!!!", glm::vec3(0, 1, 0), 40.f, 100.f, 400.f);
		RenderTextOnScreen(meshList[GEO_TEXT],
			"Head to the door to leave!", glm::vec3(1, 1, 0), 30.f, 80.f, 340.f);
	}

	if (gameState == STATE_LOST)
	{
		RenderTextOnScreen(meshList[GEO_TEXT],
			"BOOM. YOU FAILED.", glm::vec3(1, 0, 0), 60.f, 100.f, 340.f);
		RenderTextOnScreen(meshList[GEO_TEXT],
			"[R] Try Again", glm::vec3(1, 1, 0), 35.f, 100.f, 250.f);
	}

	// Door prompts (shown regardless of game state)
	if (showInteractPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT],
			"[F] Exit to Lobby", glm::vec3(1, 1, 1), 30.f, 90.f, 50.f);
	else if (showLockedPrompt)
		RenderTextOnScreen(meshList[GEO_TEXT],
			"You need to win the game first!", glm::vec3(1, 0, 0), 30.f, 50.f, 50.f);
}

void SceneDucks::RenderMesh(Mesh* mesh, bool enableLight)
{
	glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));

	glm::mat4 modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, glm::value_ptr(modelView));

	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		glm::mat4 mit = glm::inverseTranspose(modelView);
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, glm::value_ptr(mit));
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
		glBindTexture(GL_TEXTURE_2D, 0);
}

void SceneDucks::RenderSkybox()
{
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

void SceneDucks::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sx, float sy)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 1920.f, 0.f, 1080.f, -1000.f, 1000.f);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sx, sy, 1);
	RenderMesh(mesh, false);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneDucks::RenderText(Mesh* mesh, std::string text, glm::vec3 color)
{
	if (!mesh || mesh->textureID <= 0) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
		glm::mat4 cs = glm::translate(glm::mat4(1.f), glm::vec3(i * 1.0f, 0, 0));
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * cs;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void SceneDucks::RenderTextOnScreen(Mesh* mesh, std::string text,
	glm::vec3 color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) return;
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
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
		glm::mat4 cs = glm::translate(glm::mat4(1.f), glm::vec3(0.5f + i * 1.0f, 0.5f, 0));
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * cs;
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

void SceneDucks::Exit()
{
	for (int i = 0; i < NUM_GEOMETRY; ++i)
		if (meshList[i]) delete meshList[i];
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void SceneDucks::HandleKeyPress()
{
	if (KeyboardController::GetInstance()->IsKeyPressed(0x31))
		glEnable(GL_CULL_FACE);
	if (KeyboardController::GetInstance()->IsKeyPressed(0x32))
		glDisable(GL_CULL_FACE);
	if (KeyboardController::GetInstance()->IsKeyPressed(0x33))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (KeyboardController::GetInstance()->IsKeyPressed(0x34))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (KeyboardController::GetInstance()->IsKeyPressed(VK_SPACE))
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		light[0].power = (light[0].power <= 0.1f) ? 1.f : 0.1f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_TAB))
	{
		if (light[0].type == Light::LIGHT_POINT)            light[0].type = Light::LIGHT_DIRECTIONAL;
		else if (light[0].type == Light::LIGHT_DIRECTIONAL) light[0].type = Light::LIGHT_SPOT;
		else                                                 light[0].type = Light::LIGHT_POINT;
		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}

	// F key: pick up hook (only in STATE_FIND_HOOK)
	// Door open (F near door in STATE_WON) is handled in Update()
	if (KeyboardController::GetInstance()->IsKeyPressed('F'))
	{
		if (gameState == STATE_FIND_HOOK && IsPlayerNearHook(5.0f))
		{
			hookPickedUp = true;
			gameState = STATE_PLAYING;
			bombTimer = 30.0f;
		}
	}

	// E: enter/exit top-down pool view
	if (KeyboardController::GetInstance()->IsKeyPressed('E'))
	{
		if (!isTopDown && hookPickedUp && IsPlayerNearPool(7.f))
		{
			savedCamPos = camera.position;
			savedCamTarget = camera.target;
			savedCamUp = camera.up;

			camera.position = glm::vec3(0.f, 6.0f, 0.f);
			camera.target = glm::vec3(0.f, 0.f, 0.f);
			camera.up = glm::vec3(0.f, 0.f, -1.f);

			isTopDown = true;
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else if (isTopDown)
		{
			camera.position = savedCamPos;
			camera.target = savedCamTarget;
			camera.up = savedCamUp;

			isTopDown = false;
			glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	// R: reset in place (only for non-won states; STATE_WON uses the door)
	if (KeyboardController::GetInstance()->IsKeyPressed('R'))
	{
		if (gameState == STATE_LOST || gameState == STATE_PLAYING || gameState == STATE_FIND_HOOK)
		{
			gameState = STATE_FIND_HOOK;
			hookPickedUp = false;
			ducksPickedUp = 0;
			catchTimer = 0.f;
			wrongTimer = 0.f;
			bombTimer = 30.0f;

			InitDucks();

			camera.Init(
				glm::vec3(0, 1.8f, 10),
				glm::vec3(0, 1.7f, 0),
				glm::vec3(0, 1.0f, 0)
			);

			if (isTopDown)
			{
				isTopDown = false;
				glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
		// STATE_WON: R intentionally does nothing — player must use the door
	}
}

void SceneDucks::HandleMouseInput()
{
	static bool wasLeftDown = false;
	bool isLeftDown = MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT);

	if (isLeftDown && !wasLeftDown)
	{
		if (isTopDown && gameState == STATE_PLAYING)
		{
			double mx = MouseController::GetInstance()->GetMousePositionX();
			double my = MouseController::GetInstance()->GetMousePositionY();

			float ndcX = (static_cast<float>(mx) / 1920.f) * 2.f - 1.f;
			float ndcY = 1.f - (static_cast<float>(my) / 1080.f) * 2.f;

			glm::mat4 proj = projectionStack.Top();
			glm::mat4 view = viewStack.Top();
			glm::mat4 invVP = glm::inverse(proj * view);

			glm::vec4 nearClip(ndcX, ndcY, -1.f, 1.f);
			glm::vec4 farClip(ndcX, ndcY, 1.f, 1.f);

			glm::vec4 nearWorld = invVP * nearClip;
			glm::vec4 farWorld = invVP * farClip;
			nearWorld /= nearWorld.w;
			farWorld /= farWorld.w;

			glm::vec3 rayOrigin(nearWorld);
			glm::vec3 rayDir = glm::normalize(glm::vec3(farWorld) - rayOrigin);

			float planeY = -1.2f;
			float hitX = 0.f, hitZ = 0.f;
			bool  validHit = false;

			if (fabsf(rayDir.y) > 0.0001f)
			{
				float t = (planeY - rayOrigin.y) / rayDir.y;
				if (t > 0.f)
				{
					hitX = rayOrigin.x + rayDir.x * t;
					hitZ = rayOrigin.z + rayDir.z * t;
					validHit = true;
				}
			}

			if (validHit)
			{
				const float CLICK_RADIUS = 0.4f;

				for (int i = 0; i < NUM_DUCKS; ++i)
				{
					Duck& d = ducks[i];
					if (d.caught) continue;

					float dx = hitX - d.pos.x;
					float dz = hitZ - d.pos.z;
					float distSq = dx * dx + dz * dz;

					if (distSq <= CLICK_RADIUS * CLICK_RADIUS)
					{
						if (d.isCorrect)
						{
							d.caught = true;
							ducksPickedUp++;
							catchTimer = 1.5f;
							wrongTimer = 0.f;

							if (ducksPickedUp >= MAX_DUCKS)
								gameState = STATE_WON;
						}
						else
						{
							wrongTimer = 1.5f;
							catchTimer = 0.f;
						}
						break;
					}
				}
			}
		}
	}

	wasLeftDown = isLeftDown;
}

bool SceneDucks::CheckDAABBCollision(const glm::vec3& pos, float radius, const DAABB& box)
{
	glm::vec3 closestPoint;
	closestPoint.x = glm::clamp(pos.x, box.min.x, box.max.x);
	closestPoint.y = glm::clamp(pos.y, box.min.y, box.max.y);
	closestPoint.z = glm::clamp(pos.z, box.min.z, box.max.z);
	float distance = glm::distance(closestPoint, pos);
	return distance < radius;
}

void SceneDucks::BuildCollisionBoxes()
{
	collisionBoxes.clear();

	// Floor
	DAABB floor;
	floor.min = glm::vec3(-10.f, -2.5f, -7.5f);
	floor.max = glm::vec3(10.f, 1.3f, 17.5f);
	collisionBoxes.push_back(floor);

	// Back wall (z = -7.5)
	DAABB backWall;
	backWall.min = glm::vec3(-10.f, -2.f, -7.6f);
	backWall.max = glm::vec3(10.f, 8.f, -7.4f);
	collisionBoxes.push_back(backWall);

	// Left wall (x = -10)
	DAABB leftWall;
	leftWall.min = glm::vec3(-10.2f, -2.f, -7.5f);
	leftWall.max = glm::vec3(-9.5f, 8.f, 17.5f);
	collisionBoxes.push_back(leftWall);

	// Right wall (x = 10)
	DAABB rightWall;
	rightWall.min = glm::vec3(9.5f, -2.f, -7.5f);
	rightWall.max = glm::vec3(10.2f, 8.f, 17.5f);
	collisionBoxes.push_back(rightWall);

	// Back wall L (z = 17.5, right side)
	DAABB backWallL;
	backWallL.min = glm::vec3(1.f, -2.f, 17.3f);
	backWallL.max = glm::vec3(10.f, 8.f, 17.6f);
	collisionBoxes.push_back(backWallL);

	// Back wall R (z = 17.5, left side)
	DAABB backWallR;
	backWallR.min = glm::vec3(-10.f, -2.f, 17.3f);
	backWallR.max = glm::vec3(-1.f, 8.f, 17.6f);
	collisionBoxes.push_back(backWallR);

	// Pool outer wall - left side
	DAABB poolLeft;
	poolLeft.min = glm::vec3(-3.8f, -2.5f, -3.8f);
	poolLeft.max = glm::vec3(-3.2f, 1.5f, 3.8f);
	collisionBoxes.push_back(poolLeft);

	// Pool outer wall - right side
	DAABB poolRight;
	poolRight.min = glm::vec3(3.2f, -2.5f, -3.8f);
	poolRight.max = glm::vec3(3.8f, 1.5f, 3.8f);
	collisionBoxes.push_back(poolRight);

	// Pool outer wall - front side
	DAABB poolFront;
	poolFront.min = glm::vec3(-3.8f, -2.5f, -3.8f);
	poolFront.max = glm::vec3(3.8f, 1.5f, -3.2f);
	collisionBoxes.push_back(poolFront);

	// Pool outer wall - back side
	DAABB poolBack;
	poolBack.min = glm::vec3(-3.8f, -2.5f, 3.2f);
	poolBack.max = glm::vec3(3.8f, 1.5f, 3.8f);
	collisionBoxes.push_back(poolBack);

	// Door (z = 17.5, center gap between back wall L and R)
	DAABB doorBox;
	doorBox.min = glm::vec3(-1.f, -2.f, 17.3f);
	doorBox.max = glm::vec3(1.f, 8.f, 17.6f);
	collisionBoxes.push_back(doorBox);
}