#ifndef SCENE_MODEL2_H
#define SCENE_MODEL2_H

#include "Scene.h"
#include "Mesh.h"
#include "AltAzCamera.h"
#include "FPCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "MouseController.h"

class SceneModel2 : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_PLANE,
		GEO_MODEL1,
		GEO_MODEL_MTL1,
		GEO_MODEL_MTL2,
		GEO_MODEL_OBJ1,
		GEO_MODEL_OBJ2,
		GEO_MODEL_OBJ3,
		GEO_MODEL_OBJ4,
		GEO_MODEL_OBJ5,
		GEO_MODEL_OBJ6,
		GEO_GUI,
		GEO_TEXT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_FRONT,
		GEO_BACK,
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

		//U_LIGHT1_TYPE,
		//U_LIGHT1_POSITION,
		//U_LIGHT1_COLOR,
		//U_LIGHT1_POWER,
		//U_LIGHT1_KC,
		//U_LIGHT1_KL,
		//U_LIGHT1_KQ,
		//U_LIGHT1_SPOTDIRECTION,
		//U_LIGHT1_COSCUTOFF,
		//U_LIGHT1_COSINNER,
		//U_LIGHT1_EXPONENT,

		U_NUMLIGHTS,
		U_LIGHTENABLED,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	SceneModel2();
	~SceneModel2();

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
	FPCamera camera;
	int projType = 1; // fix to 0 for orthographic, 1 for projection

	MatrixStack modelStack, viewStack, projectionStack;

	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool enableLight;

	void RenderSkybox();
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void HandleMouseInput();
	void RenderText(Mesh* mesh, std::string text, glm::vec3 color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);

	// Add these member variables to your class
	glm::vec3 collisionBoxPositions[10];
	glm::vec3 collisionBoxSizes[10];
	int numCollisionBoxes;

	float cameraRadius;

	bool isDoorOpen;
	float doorAngle;
	float doorAnimationSpeed;

	static const int NUM_TREES = 7;
	glm::vec3 treePositions[NUM_TREES];
	float treeRadius;

	static const int NUM_WOLVES = 5;
	glm::vec3 wolfPositions[NUM_WOLVES];
	float wolfRadius;
	bool wolvesSpawned;
	float wolfMoveSpeed;

	float distanceToDoor;
	float doorInteractionRadius;
	float distanceToBox;
	float musicBoxInteractionRadius;
	
	bool showDialogue;
	int dialogueStage;
	double dialogueTimer;
	int wolvesKilled;  // Track how many wolves despawned
	bool musicBoxActivated;
	bool allWolvesDefeated;

	bool jumpscareActive;
	double jumpscareTimer;
	float cameraShakeIntensity;
	glm::vec3 jumpscareWolfPosition;
};

#endif