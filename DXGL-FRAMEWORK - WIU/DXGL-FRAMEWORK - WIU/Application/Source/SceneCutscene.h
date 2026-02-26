#ifndef SCENE_CUTSCENE_H
#define SCENE_CUTSCENE_H

#include "Scene.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include <string>
#include <vector>



class SceneCutscene : public Scene
{
public:
    SceneCutscene();
    ~SceneCutscene();

    virtual void Init();
    virtual void Update(double dt);
    virtual void Render();
    virtual void Exit();

private:
    // ── Uniform enum (matches SceneShooting order) ────────────────────────
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

    enum GEOMETRY_TYPE
    {
        GEO_TEXT = 0,
        GEO_QUAD,
        NUM_GEOMETRY,
    };

    // ── A single story page ───────────────────────────────────────────────
    struct Page
    {
        std::string title;      // large line at top
        std::string body;       // smaller line below
        std::string prompt;     // bottom hint, e.g. "Press ENTER to continue"
    };

    // ── State ─────────────────────────────────────────────────────────────
    std::vector<Page> pages;
    int               currentPage;

    // Fade-in timer: each page fades in over FADE_DURATION seconds
    float             fadeTimer;
    static const float FADE_DURATION;  // defined in .cpp

    // Debounce ENTER so one press doesn't skip two pages
    bool              enterWasDown;

    // ── OpenGL handles ────────────────────────────────────────────────────
    unsigned int      m_vertexArrayID;
    unsigned int      m_programID;
    unsigned int      m_parameters[U_TOTAL];
    Mesh* meshList[NUM_GEOMETRY];

    Light             light[1];

    MatrixStack       projectionStack;
    MatrixStack       viewStack;
    MatrixStack       modelStack;

    // ── Helpers ───────────────────────────────────────────────────────────
    void RenderMesh(Mesh* mesh, bool enableLight);
    void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
    void RenderTextOnScreen(Mesh* mesh, std::string text,
        glm::vec3 color, float size, float x, float y);
    void HandleKeyPress();
};

#endif