#ifndef SCENE_CANS_H
#define SCENE_CANS_H

#include "Scene.h"
#include "Mesh.h"
//#include "AltAzCamera.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "SceneManager.h"
#include <iostream>
#include "Door.h"

class SceneCans : public Scene
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


	SceneCans();
	~SceneCans();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void HandleKeyPress();
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	void RenderMeshOnScreen(Mesh* mesh, float x, float y,float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, glm::vec3	color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);
	void HandleMouseInput();

	
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
	static const int NUM_DOORS = 1;
	Door door;
	bool showInteractPrompt;

	// light 
	glm::vec3 lightSwitchPosition;
	bool isLightSwitchOn;
	float leverRotation;

	bool IsPlayerNearLightSwitch(float radius);


	// shutter
	glm::vec3 shutterButtonPosition;
	bool isShutterOpen;
	float shutterHeight;
	float buttonPressDepth;

	bool IsPlayerNearShutterButton(float radius);



	// Game state
	GameState gameState;

	// Collision detection
	glm::vec3 playerSize;
	bool CheckWallCollision(const glm::vec3& pos);

	
	float fps = 0;
};

#endif