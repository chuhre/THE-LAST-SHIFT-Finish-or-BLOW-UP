#include "SceneMenu.h"

#include "GL\glew.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

#include "shader.hpp"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "LoadTGA.h"
#include "SceneManager.h"

// ─────────────────────────────────────────────────────────────────────────────
SceneMenu::SceneMenu()
    : selectedOption(0)
    , blinkTimer(0.f)
    , showBlink(true)
{
}

SceneMenu::~SceneMenu()
{
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::Init()
{
    // Black background – suits a title screen
    glClearColor(0.10f, 0.02f, 0.18f, 1.0f);  // dark purple background

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // VAO
    glGenVertexArrays(1, &m_vertexArrayID);
    glBindVertexArray(m_vertexArrayID);

    // ── Shaders (same pair used in SceneShooting) ─────────────────────────
    m_programID = LoadShaders("Shader//Texture.vertexshader",
        "Shader//Text.fragmentshader");
    glUseProgram(m_programID);

    // ── Uniform locations ─────────────────────────────────────────────────
    m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
    m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
    m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
    m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
    m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
    m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
    m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
    m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
    m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
    m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
    m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
    m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
    m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
    m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
    m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
    m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
    m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
    m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
    m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
    m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
    m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
    m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
    m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
    m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

    // ── Meshes ────────────────────────────────────────────────────────────
    for (int i = 0; i < NUM_GEOMETRY; ++i)
        meshList[i] = nullptr;

    // Font atlas – same calibri.tga used by SceneShooting
    meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
    meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

    // Full-screen background quad (rendered in ortho space)
    meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("MenuBG", glm::vec3(1, 1, 1), 1.f);

    // ── Light (minimal – menu doesn't really need it, but keeps shader happy) ──
    glUniform1i(m_parameters[U_NUMLIGHTS], 1);

    light[0].type = Light::LIGHT_DIRECTIONAL;
    light[0].position = glm::vec3(0.f, 10.f, 0.f);
    light[0].color = glm::vec3(1.f, 1.f, 1.f);
    light[0].power = 1.f;
    light[0].kC = 1.f;
    light[0].kL = 0.01f;
    light[0].kQ = 0.001f;
    light[0].cosCutoff = 45.f;
    light[0].cosInner = 30.f;
    light[0].exponent = 3.f;
    light[0].spotDirection = glm::vec3(0.f, -1.f, 0.f);

    glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
    glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
    glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
    glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
    glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
    glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
    glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF],
        cosf(glm::radians<float>(light[0].cosCutoff)));
    glUniform1f(m_parameters[U_LIGHT0_COSINNER],
        cosf(glm::radians<float>(light[0].cosInner)));
    glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

    // ── Projection (ortho-only menu – 16:9 matches SceneShooting) ────────
    glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    projectionStack.LoadMatrix(projection);

    // ── State ─────────────────────────────────────────────────────────────
    selectedOption = 0;
    blinkTimer = 0.f;
    showBlink = true;

    // Show and unlock the cursor so the player can see it on the menu
    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::Update(double dt)
{
    HandleKeyPress();

    // Blink "Press ENTER" every 0.5 s
    blinkTimer += static_cast<float>(dt);
    if (blinkTimer >= 0.5f)
    {
        blinkTimer = 0.f;
        showBlink = !showBlink;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::HandleKeyPress()
{
    // ENTER or SPACE → start game, switch to Lobby
    if (KeyboardController::GetInstance()->IsKeyPressed(VK_RETURN) ||
        KeyboardController::GetInstance()->IsKeyPressed(VK_SPACE))
    {
        SceneManager::GetInstance()->SwitchScene(SceneManager::SCENE_LOBBY);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ── Camera / view (identity – nothing 3D to show on menu) ────────────
    viewStack.LoadIdentity();
    viewStack.LookAt(
        0, 0, 1,   // eye
        0, 0, 0,   // centre
        0, 1, 0    // up
    );
    modelStack.LoadIdentity();

    // ── Pass light to shader (directional, camera-space) ─────────────────
    glm::vec3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
    glm::vec3 lightDir_cs = glm::vec3(viewStack.Top() * glm::vec4(lightDir, 0));
    glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightDir_cs));

    // ─────────────────────────────────────────────────────────────────────
    // All UI is drawn in 2-D ortho space (800 x 600), same as SceneShooting
    // ─────────────────────────────────────────────────────────────────────

    // ── Enable blending so quads layer correctly ─────────────────────────
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shared specular / shininess for all flat quads
    meshList[GEO_QUAD]->material.kSpecular = glm::vec3(0.f, 0.f, 0.f);
    meshList[GEO_QUAD]->material.kShininess = 1.f;



    // ── WHITE TITLE BACKGROUND RECTANGLE ─────────────────────────────────
    //    Sits behind the title and subtitle  (Y: 455–560)
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(1.f, 1.f, 1.f);
    meshList[GEO_QUAD]->material.kDiffuse = glm::vec3(1.f, 1.f, 1.f);
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 510.f, 680.f, 100.f);

    // ── CENTRE PANEL — deep warm red behind controls ──────────────────────
    //    Covers Y: 150–445
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(0.45f, 0.05f, 0.05f);
    meshList[GEO_QUAD]->material.kDiffuse = glm::vec3(0.45f, 0.05f, 0.05f);
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 297.f, 680.f, 295.f);

    // ── BOTTOM BAR — bright carnival yellow ──────────────────────────────
    //    Covers Y: 50–148
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(0.90f, 0.65f, 0.00f);
    meshList[GEO_QUAD]->material.kDiffuse = glm::vec3(0.90f, 0.65f, 0.00f);
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 99.f, 680.f, 100.f);

    // ── TOP DIVIDER — yellow stripe under the white box ───────────────────
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(0.95f, 0.75f, 0.00f);
    meshList[GEO_QUAD]->material.kDiffuse = glm::vec3(0.95f, 0.75f, 0.00f);
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 456.f, 680.f, 6.f);

    // ── BOTTOM DIVIDER — red stripe above the yellow bar ──────────────────
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(0.72f, 0.08f, 0.08f);
    meshList[GEO_QUAD]->material.kDiffuse = glm::vec3(0.72f, 0.08f, 0.08f);
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 149.f, 680.f, 6.f);

    glDisable(GL_BLEND);

    // ── Title — red text on white rectangle ───────────────────────────────
    RenderTextOnScreen(meshList[GEO_TEXT],
        "THE LAST SHIFT:",
        glm::vec3(0.72f, 0.05f, 0.05f),
        38.f,
        130.f, 510.f);

    // ── Sub-title — dark red below title, still on white box ─────────────
    RenderTextOnScreen(meshList[GEO_TEXT],
        "Finish or BLOW UP!",
        glm::vec3(0.50f, 0.04f, 0.04f),
        22.f,
        200.f, 468.f);

    // ── Controls header — yellow on red panel ─────────────────────────────
    RenderTextOnScreen(meshList[GEO_TEXT],
        "CONTROLS",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        318.f, 415.f);

    // ── Controls list — light yellow on red panel ─────────────────────────
    RenderTextOnScreen(meshList[GEO_TEXT],
        "WASD  - Move",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        270.f, 380.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "MOUSE - Look",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        270.f, 350.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "F     - Interact",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        270.f, 320.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "LMB   - Shoot",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        270.f, 290.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "P     - Pause",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        270.f, 260.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "ESC   - Quit",
        glm::vec3(0.f, 0.f, 0.f),
        22.f,
        270.f, 230.f);

    // ── "Press ENTER" prompt — red text on yellow bar ─────────────────────
    if (showBlink)
    {
        RenderTextOnScreen(meshList[GEO_TEXT],
            "PRESS [SPACE] TO START",
            glm::vec3(0.55f, 0.03f, 0.03f),
            26.f,
            120.f, 82.f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::Exit()
{
    for (int i = 0; i < NUM_GEOMETRY; ++i)
    {
        if (meshList[i])
        {
            delete meshList[i];
            meshList[i] = nullptr;
        }
    }
    glDeleteVertexArrays(1, &m_vertexArrayID);
    glDeleteProgram(m_programID);
}

// ─────────────────────────────────────────────────────────────────────────────
// RenderMesh – identical to SceneShooting's implementation
// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::RenderMesh(Mesh* mesh, bool enableLight)
{
    glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
    glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));

    glm::mat4 modelView = viewStack.Top() * modelStack.Top();
    glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, glm::value_ptr(modelView));

    if (enableLight)
    {
        glUniform1i(m_parameters[U_LIGHTENABLED], 1);
        glm::mat4 modelView_it = glm::inverseTranspose(modelView);
        glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE,
            glm::value_ptr(modelView_it));

        glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
        glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
        glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
        glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
    }
    else
    {
        glUniform1i(m_parameters[U_LIGHTENABLED], 0);
    }

    if (mesh->textureID > 0)
    {
        glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh->textureID);
        glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
    }
    else
    {
        glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
    }

    mesh->Render();

    if (mesh->textureID > 0)
        glBindTexture(GL_TEXTURE_2D, 0);
}

// ─────────────────────────────────────────────────────────────────────────────
// RenderMeshOnScreen — identical to SceneShooting's implementation
// Ortho space: 800 x 600. x/y = centre of quad, sizex/sizey = dimensions.
// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::RenderMeshOnScreen(Mesh* mesh, float x, float y,
    float sizex, float sizey)
{
    glDisable(GL_DEPTH_TEST);
    glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f);

    projectionStack.PushMatrix();
    projectionStack.LoadMatrix(ortho);
    viewStack.PushMatrix();
    viewStack.LoadIdentity();
    modelStack.PushMatrix();
    modelStack.LoadIdentity();

    modelStack.Translate(x, y, 0.f);
    modelStack.Scale(sizex, sizey, 1.f);

    RenderMesh(mesh, false);   // UI — no lighting

    projectionStack.PopMatrix();
    viewStack.PopMatrix();
    modelStack.PopMatrix();

    glEnable(GL_DEPTH_TEST);
}

// ─────────────────────────────────────────────────────────────────────────────
// RenderTextOnScreen – identical to SceneShooting's implementation
// ─────────────────────────────────────────────────────────────────────────────
void SceneMenu::RenderTextOnScreen(Mesh* mesh, std::string text,
    glm::vec3 color, float size,
    float x, float y)
{
    if (!mesh || mesh->textureID <= 0)
        return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Same ortho dimensions as SceneShooting (800 x 600)
    glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f);

    projectionStack.PushMatrix();
    projectionStack.LoadMatrix(ortho);
    viewStack.PushMatrix();
    viewStack.LoadIdentity();
    modelStack.PushMatrix();
    modelStack.LoadIdentity();
    modelStack.Translate(x, y, 0);
    modelStack.Scale(size, size, size);

    glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
    glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
    glUniform1i(m_parameters[U_LIGHTENABLED], 0);
    glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mesh->textureID);
    glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

    for (unsigned i = 0; i < text.length(); ++i)
    {
        glm::mat4 charSpacing = glm::translate(
            glm::mat4(1.f),
            glm::vec3(0.5f + i * 1.0f, 0.5f, 0));
        glm::mat4 MVP = projectionStack.Top() *
            viewStack.Top() *
            modelStack.Top() *
            charSpacing;
        glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
        mesh->Render((unsigned)text[i] * 6, 6);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
    projectionStack.PopMatrix();
    viewStack.PopMatrix();
    modelStack.PopMatrix();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}