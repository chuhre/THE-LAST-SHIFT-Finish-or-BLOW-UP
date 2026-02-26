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

struct AABB {
	glm::vec3 min;
	glm::vec3 max;

	AABB() : min(0.0f), max(0.0f) {}
	AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
};

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
	bool m_isAiming = false;
	glm::vec3 m_savedCamPos;
	glm::vec3 m_savedCamTarget;
	glm::vec3 m_savedCamUp;

	const glm::vec3 AIM_CAM_POS = glm::vec3(0.f, 4.1f, 2.9f);
	const glm::vec3 AIM_CAM_TARGET = glm::vec3(0.f, 3.9f, 1.9f);
	const glm::vec3 LAUNCH_POS = glm::vec3(0.1f, 3.2f, 2.5f);

	MatrixStack modelStack, viewStack, projectionStack;

	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool enableLight;


	// door
	static const int NUM_DOORS = 2;
	Door door[NUM_DOORS];
	bool showDoorInteractPrompt;

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
	bool showPickupPrompt = false;
	bool RayHitsBall(int ballIndex, float maxDist);

	//booth
	bool showBoothPrompt = false;

	//crosshair
	Vector3 crosshairPos = Vector3(405, 290, 0);

	// Collision detection
	std::vector<AABB> collisionBoxes;
	glm::vec3 playerSize;
	bool CheckAABBCollision(const glm::vec3& pos, float radius, const AABB& box);
	void BuildCollisionBoxes();

	//physics
	void InitialiseCans();
	void InitialiseBalls();
	void ApplyGravity(PhysicsObject& obj, float dt);
	void UpdateBall(float dt);
	void UpdateCans(float dt);
	void CheckBallCanCollisions();
	void CheckCanCanCollisions();
	void CheckFloorCollisions();
	bool CheckSceneCollisions();
	void LaunchBall();
	void ResetGame();

	//aim line
	float m_aimYaw = 0.f;   // horizontal aim angle (degrees)
	float m_aimPitch = 75.f;    // vertical aim angle (degrees)
	float m_aimZOffset = 0.f;   // which can to target along Z axis
	glm::vec3 m_aimDir;         // computed aim direction
	glm::vec3 m_dynamicAimTarget; // where the aim camera looks

	glm::vec3 m_aimWorldTarget;
	void DrawAimLine();



	//helpers
	void DrawRayCastLine();
	void RenderHUD();


	//gravity
	const float GRAVITY = -25.f;
};

#endif