#ifndef SCENE_MODEL_H
#define SCENE_MODEL_H

#include "Scene.h"
#include "Mesh.h"
#include "AltAzCamera.h"
#include "MatrixStack.h"
#include "Light.h"

class SceneModel : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_LIGHT_SPHERE,
		GEO_CUBE,
		GEO_CYLINDER,
		GEO_CONE,
		GEO_CUBOID,
		GEO_HAT_CONE,
		GEO_PELVIS_CUBOID,
		GEO_BODY_CUBE,
		GEO_SHOULDER_CUBOID,
		GEO_JOINT_SPHERE,
		GEO_UPPER_ARM_CYLINDER,
		GEO_LOWER_ARM_CYLINDER,
		GEO_HAND_CUBE,
		GEO_UPPER_LEG_CYLINDER,
		GEO_LOWER_LEG_CYLINDER,
		GEO_FOOT_CUBOID,
		GEO_NECK_HEMISPHERE,
		GEO_HEAD_SPHERE,
		GEO_EYE_SPHERE,
		NUM_GEOMETRY,
	};

	enum ANIMATION
	{
		ANIM_RESET,
		ANIM_IDLE,
		ANIM_WAVE,
		ANIM_ATTACK,
		ANIM_SOMERSAULT,
		NUM_ANIM
	};
	// Keep track of current animation
	ANIMATION currAnim;

	enum UNIFORM_TYPE
	{
		U_MVP = 0,

		// Add the following constants
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHTENABLED,

		U_TOTAL,
	};

	SceneModel();
	~SceneModel();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void HandleKeyPress();

	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	int projType = 1; // 1:perspective, 0:orthographic

	AltAzCamera camera;

	MatrixStack modelStack, viewStack, projectionStack;

	float waveRotation;
	float waveSpeed;
	bool waveDirection;

	float jumpHeight;
	float jumpSpeed;

	float headRotationY;
	float headRotationSpeed;
	bool headRotationDirection;
	float bodyBendZ;        // body leaning forward/back
	float armSwingRotation; // arms swinging slightly
	float kneeBend;         // knees bending
	float pelvisShiftX;

	bool isSpinAttacking;
	float spinTimer;
	float spinDuration;
	float spinRotation;
	float spinSpeed;

	float somersaultRotation;
	float somersaultSpeed;
	float somersaultTimer;
	float somersaultDuration;
	float somersaultPrepDuration;
	bool isSomersaulting;

	bool isTransitioning;
	ANIMATION targetAnim;
	float transitionTimer;
	float transitionDuration;

	void RenderMesh(Mesh* mesh, bool enableLight);
	Light light[1];
};

#endif