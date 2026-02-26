#ifndef SCENE_DUCKS_H
#define SCENE_DUCKS_H

#include "Scene.h"
#include "Mesh.h"
//#include "AltAzCamera.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "Door.h"



struct Duck
{
	glm::vec3 pos;          // XZ used for movement; Y is kept flat (water surface)
	glm::vec2 velocity;     // movement direction * speed (XZ plane)
	float     facingAngle;  // degrees, Y-axis rotation for rendering
	bool      isCorrect;    // true = one of the 3 "right" ducks
	bool      caught;       // has the player caught this duck?
	float     bobOffset;    // per-duck phase offset for bobbing animation
	float     bobTimer;     // time accumulator for bobbing
};

struct DAABB {
	glm::vec3 min;
	glm::vec3 max;

	DAABB() : min(0.0f), max(0.0f) {}
	DAABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
};

class SceneDucks : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_PLANE,

		GEO_OBJ,

		GEO_FLOOR,
		GEO_WALL,
		GEO_CEILING,
		GEO_COUNTER,
		GEO_DOOR,

		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,

		GEO_BALLOON,
		GEO_CRATE,
		GEO_CRATE1,
		GEO_DUCKTABLE,
		GEO_TOYTRAIN,
		GEO_TOYPLANE,
		GEO_DUCKBASKETBALL,
		GEO_BOMB,

		GEO_LIGHT_SWITCH,        // Switch plate
		GEO_LIGHT_SWITCH_LEVER,  // Toggle lever

		GEO_GUI,

		GEO_TEXT,

		GEO_POOL,
		GEO_WATER,
		GEO_DUCK,
		GEO_DUCKLEYE,
		GEO_DUCKREYE,
		GEO_PEGHOOK,

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

	enum GameState { STATE_FIND_HOOK, STATE_PLAYING, STATE_WON, STATE_LOST};
	GameState gameState = STATE_FIND_HOOK;

	bool hookPickedUp = false;
	glm::vec3 hookWorldPos = glm::vec3(4.f, 0.5f, 5.f); // hook lying on floor

	// Scoring
	int ducksPickedUp = 0;
	float catchTimer = 0.f;   // flash "GOT ONE!"
	float wrongTimer = 0.f;   // flash "WRONG DUCK!"

	// Duck flock constants
	static const int NUM_DUCKS = 9;
	static const int MAX_DUCKS = 3;   // ducks the player must catch to win

	// Pool boundary: water sphere radius(1) * XZ scale(18) * pool root scale(0.2) = 3.6
	static constexpr float POOL_RADIUS = 3.35f;
	// Duck collision radius scaled proportionally to pool size
	static constexpr float DUCK_RADIUS = 0.26f;

	// Duck array
	Duck ducks[NUM_DUCKS];


	SceneDucks();
	~SceneDucks();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void HandleKeyPress();
	void HandleMouseInput();
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, glm::vec3 color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);
	void RenderSkybox();
	void RenderDucks();

	// Duck logic
	void InitDucks();
	void UpdateDucks(float dt);

	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	int projType = 1; // 0 = orthographic, 1 = perspective
	FPCamera camera;

	MatrixStack modelStack, viewStack, projectionStack;

	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool enableLight;

	// ANIMATIONS/INTERACTIONS
	// door
	float doorRotation;
	bool isDoorOpen;
	glm::vec3 doorPosition;
	bool IsPlayerNearDoor(float radius);
	bool showInteractPrompt;
	bool showLockedPrompt;

	// light switch
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

	// hook
	bool IsPlayerNearHook(float radius);

	// pool interaction
	bool IsPlayerNearPool(float radius);

	// top-down camera mode
	bool isTopDown = false;
	glm::vec3 savedCamPos;      // FP position saved before entering top-down
	glm::vec3 savedCamTarget;   // FP target saved before entering top-down
	glm::vec3 savedCamUp;       // FP up saved before entering top-down

	float bombTimer;
	float fps = 0;

	// door
	static const int NUM_DOORS = 2;
	Door door[NUM_DOORS];


	// Collision detection
	std::vector<DAABB> collisionBoxes;
	glm::vec3 playerSize;
	bool CheckDAABBCollision(const glm::vec3& pos, float radius, const DAABB& box);
	void BuildCollisionBoxes();
};

#endif