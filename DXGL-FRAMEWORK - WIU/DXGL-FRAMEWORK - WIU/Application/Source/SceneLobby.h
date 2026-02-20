#ifndef SCENE_LOBBY_H
#define SCENE_LOBBY_H

#include "Scene.h"
#include "Mesh.h"
//#include "AltAzCamera.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "SceneManager.h"
#include <iostream>

struct Door {
	glm::vec3 position;
	float width, height;
	SceneManager::SCENE_TYPE leadsTo;// Which scene this door leads to
};

class SceneLobby : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_PLANE,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		GEO_DOOR_HOLE,
		GEO_DOOR,

		GEO_LIGHT_SWITCH,        // Switch plate
		GEO_LIGHT_SWITCH_LEVER,  // Toggle lever

		GEO_GUI,

		GEO_TEXT,

		NUM_GEOMETRY,
	};

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_LIGHTENABLED,


		U_TEXT_ENABLED,
		U_TEXT_COLOR,

		U_TOTAL,
	};


	enum GameState
	{
		GAME_NOT_STARTED = 0,
		GAME_PLAYING,
		GAME_WON,
		GAME_LOST
	};

	enum CustomerState
	{
		CUSTOMER_NONE = 0,
		CUSTOMER_WAITING,
		CUSTOMER_ORDERED,
		CUSTOMER_IS_MONSTER
	};


	SceneLobby();
	~SceneLobby();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void HandleKeyPress();
	void RenderMesh(Mesh* mesh, bool enableLight);

	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	//AltAzCamera camera;
	int projType = 1; // fix to 0 for orthographic, 1 for projection
	FPCamera camera;


	MatrixStack modelStack, viewStack, projectionStack;

	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool enableLight;


	// ANIMATIONS/INTERACTIONS
	
	// door
	float doorRotation;  // 0 = closed, 90 = open
	bool isDoorOpen;
	glm::vec3 doorPosition; // Store door position
	// Helper function to check if player is near door
	bool IsPlayerNearDoor(float radius);

	Door doors[4]; // Assuming 4 doors in the lobby
	int activeDoorIndex;
	bool showInteractPrompt;



	// light 
	/*glm::vec3 lightSwitchPosition;
	bool isLightSwitchOn;
	float leverRotation;

	bool IsPlayerNearLightSwitch(float radius);*/


	// shutter
	glm::vec3 shutterButtonPosition;
	bool isShutterOpen;
	float shutterHeight;
	float buttonPressDepth;

	bool IsPlayerNearShutterButton(float radius);



	// Game state
	GameState gameState;
	//CustomerState customerState;

	//int playerLives;
	//int customersServed;  // Total correct serves to win

	//glm::vec3 npcPosition;
	//bool isMonster;        // Is current customer a monster?
	//bool hasTaco;         // Is player holding taco?
	//float customerTimer;  // Timer for customer events

	//bool showNPC;         // Simple flag to show/hide NPC



	// Collision detection
	glm::vec3 playerSize;
	bool CheckWallCollision(const glm::vec3& pos);

	void RenderSkybox();
	void RenderMeshOnScreen(Mesh* mesh, float x, float y,
		float sizex, float sizey);

	void HandleMouseInput();

	void RenderText(Mesh* mesh, std::string text, glm::vec3
		color);
	void RenderTextOnScreen(Mesh* mesh, std::string text,
		glm::vec3 color, float size, float x, float y);

	float fps = 0;
};

#endif