#ifndef SCENE_MENU_H
#define SCENE_MENU_H

#include "Scene.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"

class SceneMenu : public Scene
{
public:
	SceneMenu();
	~SceneMenu();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	// ── Shader uniforms (mirrors SceneShooting pattern) ──────────────────
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
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};

	// ── Geometry slots ────────────────────────────────────────────────────
	enum GEOMETRY_TYPE
	{
		GEO_TEXT = 0,   // font quad for RenderTextOnScreen
		GEO_QUAD,       // background quad
		NUM_GEOMETRY,
	};

	// ── Menu state ────────────────────────────────────────────────────────
	// Which option the cursor is on (only 1 option for now: Start)
	int		selectedOption;

	// Simple blink timer for the "Press ENTER" prompt
	float	blinkTimer;
	bool	showBlink;

	// ── OpenGL handles ────────────────────────────────────────────────────
	unsigned int m_vertexArrayID;
	unsigned int m_programID;
	unsigned int m_parameters[U_TOTAL];

	Mesh* meshList[NUM_GEOMETRY];

	Light  light[1];

	MatrixStack projectionStack;
	MatrixStack viewStack;
	MatrixStack modelStack;

	// ── Helpers (same signatures as SceneShooting) ────────────────────────
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderTextOnScreen(Mesh* mesh, std::string text,
		glm::vec3 color, float size, float x, float y);
	void HandleKeyPress();
};

#endif