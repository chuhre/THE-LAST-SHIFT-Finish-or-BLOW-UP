#ifndef SCENE_CANS_H
#define SCENE_CANS_H

#include "Scene.h"
#include "Mesh.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "SceneManager.h"
#include <iostream>
#include "Door.h"
#include "PhysicsObject.h"



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

		GEO_DOOR,

		GEO_FLOOR,
		GEO_WALL,
		GEO_CEILING,
		GEO_COUNTER,

		GEO_BALL,
		GEO_TABLE,
		GEO_CAN,

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

	struct Can
	{
		PhysicsObject can;
		float radius = 1.5f;
		float height = 4.f;
		bool active = true;
		bool knocked = false;
		Vector3 startPos;
	};

	struct Ball
	{
		PhysicsObject ball;
		float radius = 1.2f;
		Vector3 pos;
		bool launched = false;
		bool inAir = false;
	};

	static const int NUM_CANS = 6;
	static const int MAX_BALLS = 3;


	SceneCans();
	~SceneCans();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void HandleKeyPress();
	void HandleMouseInput();

	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderSkybox();
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, glm::vec3	color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);


	//gl handlers
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	FPCamera camera;

	MatrixStack modelStack, viewStack, projectionStack;

	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool enableLight;


	// door
	static const int NUM_DOORS = 2;
	Door door[NUM_DOORS];
	bool showInteractPrompt;

	// Game state
	GameState gameState;

	//cans
	Can m_cans[NUM_CANS];
	glm::vec3 m_staticCanPos[NUM_CANS];


	//balls
	Ball m_balls[3];
	int m_noOfBalls;
	int  m_throwsLeft;
	bool ballCollected;

	


	// Collision detection
	glm::vec3 playerSize;
	bool CheckWallCollision(const glm::vec3& pos);

	//physics
	void SpawnCans();
	void SpawnBalls();
	void ApplyGravity(PhysicsObject& obj, float dt);
	void UpdateBall(float dt);
	void UpdateCans(float dt);
	void CheckBallCanCollisions();
	void CheckCanCanCollisions();
	void CheckFloorCollisions();
	void LaunchBall();
	void ResetGame();

	//helpers
	void DrawAimLine();
	void RenderHUD();

	//getters

	//gravity
	const float GRAVITY = -25.f;
};

#endif