#ifndef SCENE_SHOOTING_H
#define SCENE_SHOOTING_H

#include "Scene.h"
#include "Mesh.h"
//#include "AltAzCamera.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include <string>

class SceneShooting : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_PLANE,

		/*GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,*/

		
		// Environment
		GEO_FLOOR,
		GEO_WALL,
		GEO_CEILING,

		// Props
		GEO_COUNTER,        // barrier between player and targets
		GEO_TARGET_RAIL,    // long bar that targets slide on
		GEO_TARGET,         // the moving duck / tin-can target
		GEO_BOMB,           // bomb sitting on the counter
		GEO_GUN,            // the MISSING item the player must find

		// HUD
		GEO_GUI,            // crosshair quad
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
		STATE_FIND_GUN = 0,  // player must locate and pick up the gun first
		STATE_PLAYING,       // bomb timer counting down, player can shoot
		STATE_WON,           // all 5 targets hit before time ran out
		STATE_LOST           // timer hit 0 OR ran out of bullets with < 5 hits
	};

	
	// =====================================================
	// TARGET DATA
	// =====================================================
	struct Target
	{
		glm::vec3 position;
		float speed;        // units per second along X
		float direction;    // +1.0 or -1.0
		float minX, maxX;   // patrol bounds on the rail
		bool  isAlive;
	};

	static const int NUM_TARGETS = 5;
	static const int MAX_BULLETS = 8;

	// =====================================================
	// LIFECYCLE
	// =====================================================
	SceneShooting();
	~SceneShooting();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	// ----- input helpers ----------------------------------
	void HandleKeyPress();
	void HandleMouseInput();

	// ----- rendering helpers (same signatures as SceneWIU) 
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, glm::vec3 color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);

	// ----- game logic helpers -----------------------------
	void Shoot();
	bool RayHitTarget(int index);       // hitscan from camera through crosshair
	void ResetGame();
	bool IsPlayerNearGun(float radius);

	// ----- GL handles -------------------------------------
	unsigned m_vertexArrayID;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	Mesh* meshList[NUM_GEOMETRY];

	// ----- camera & matrices (same as SceneWIU) ----------
	FPCamera    camera;
	int         projType = 1; // 0 = ortho, 1 = perspective
	MatrixStack modelStack, viewStack, projectionStack;

	// ----- lighting (same as SceneWIU) -------------------
	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool  enableLight;

	// ----- game state ------------------------------------
	GameState gameState;
	int       bulletsLeft;    // starts at MAX_BULLETS (8)
	int       targetsHit;     // need to hit all NUM_TARGETS (5) to win
	float     bombTimer;      // counts DOWN from 120.0 seconds (2 minutes)

	// ----- gun (the missing element) ---------------------
	glm::vec3 gunWorldPos;    // where the gun is lying on the floor
	bool      gunPickedUp;

	// ----- targets ----------------------------------------
	Target targets[NUM_TARGETS];

	// ----- muzzle flash -----------------------------------
	float muzzleFlashTimer;   // renders a white flash for ~0.05s on each shot

	// ----- misc -------------------------------------------
	float fps;
};

#endif