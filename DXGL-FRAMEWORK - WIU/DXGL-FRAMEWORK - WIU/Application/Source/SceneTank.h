#ifndef SCENE_TANK_H
#define SCENE_TANK_H

#include "Scene.h"
#include "Mesh.h"
//#include "AltAzCamera.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Vector3.h"
#include "PhysicsObject.h"
#include "CollisionDetection.h"
#include "Door.h"
#include "SceneManager.h"


class SceneTank : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_PLANE,

		GEO_WALL,
		GEO_FLOOR,
		GEO_CEILING,
		GEO_TARGET,
		GEO_TANK,
		GEO_LADDER,
		GEO_BOX,
		GEO_BOX2,
		GEO_BOX3,
		GEO_BOX4,
		GEO_BALL,
		GEO_BALL2,
		GEO_BALL3,
		GEO_PLANK,
		GEO_COUNTER,
		GEO_BOMB,
		GEO_PILLAR,
		GEO_CABINET,
		GEO_DOOR,
		GEO_DUMMY,
		GEO_BALLOON,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

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
		STATE_FIND_BALLS, 
		STATE_PLAYING, 
		STATE_WON,
		STATE_LOST,
	};

	GameState gameState;

	SceneTank();
	~SceneTank();

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

	//// collision objects
	//PhysicsObject wallBack, wallLeft, wallRight, wallCeiling;
	//PhysicsObject objCounter, objPillar, objTank, objCabinet;
	//PhysicsObject objBox1, objBox2, objBox3, objBox4;

	// game timer
	float gameTimer = 30.f;
	bool timerActive = false;

	struct AABB {
		glm::vec3 min, max;
	};

	std::vector<AABB> collisionBoxes;
	bool CheckAABBCollision(const glm::vec3& pos, float radius, const AABB& box);
	void BuildCollisionBoxes();
	int doorBoxIndex = -1;

	// ball phys
	PhysicsObject ballPhys;
	bool ballThrown = false;
	float ballRadius = 0.15f; // matches your Scale(0.3f)
	
	// Ball state machine
	enum BallState { ON_COUNTER, HELD, THROWN, RESET };
	BallState ballState = ON_COUNTER;

	// Throw charging
	float throwPower = 0.f;
	bool isCharging = false;
	 
	// Target object for throwing, can be static or dynamic
	PhysicsObject objTarget;
	float targetRotation = 0.f;

	// hidden balls
	bool hiddenBall1Found = false;  // box
	bool hiddenBall2Found = false;  // cabinet
	
	// text timer
	std::string hudMessage = "";
	float hudMessageTimer = 0.f;

	// door
	Door door;
	bool showInteractPrompt;
	bool showLockedPrompt;

	// door animation
	bool doorOpen = false;
	float doorAngle = 0.f;

	// dummy animation
	float dummyFallAngle = 0.f;      // rotation as he tips over
	float dummyFallY = 0.f;          // how far he's dropped
	bool dummyFalling = false;
	bool dummyInTank = false;

	// For camera-relative throwing
	glm::vec3 ballRestPos = glm::vec3(0.f, 2.15f, 5.f);

	// target as static AABB (world position)
	glm::vec3 targetPos = glm::vec3(3.5f, 3.f, 0.f);
	
	// for walls and boxes, we can define AABBs in world space for simple collision detection with the player
	/*struct AABB {
		glm::vec3 min;
		glm::vec3 max;
	};

	std::vector<AABB> collisionBoxes;
	const float PLAYER_RADIUS = 0.4f;

	bool CheckAABBCollision(const glm::vec3& pos, float radius, const AABB& box);
	void BuildCollisionBoxes();
	void ResolvePlayerCollisions(const glm::vec3& oldPos);*/

	// target
	bool targetHit = false;
	
	// Collision detection
	glm::vec3 playerSize;

	void RenderSkybox();
	void RenderMeshOnScreen(Mesh* mesh, float x, float y,
		float sizex, float sizey);

	//void HandleMouseInput();

	void RenderText(Mesh* mesh, std::string text, glm::vec3
		color);
	void RenderTextOnScreen(Mesh* mesh, std::string text,
		glm::vec3 color, float size, float x, float y);

};

#endif