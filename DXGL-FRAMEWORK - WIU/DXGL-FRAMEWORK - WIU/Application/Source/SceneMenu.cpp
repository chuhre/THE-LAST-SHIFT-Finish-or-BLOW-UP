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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

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

    // ── Title ─────────────────────────────────────────────────────────────
    //   Large, centred near the top of the screen
    RenderTextOnScreen(meshList[GEO_TEXT],
        "THE LAST SHIFT:",
        glm::vec3(1.f, 0.85f, 0.f),   // golden yellow
        40.f,
        90.f, 420.f);

    // ── Sub-title ─────────────────────────────────────────────────────────
    RenderTextOnScreen(meshList[GEO_TEXT],
        "Finish or BlOW UP!",
        glm::vec3(1.f, 1.f, 1.f),
        25.f,
        190.f, 370.f);

    // ── Controls reminder ─────────────────────────────────────────────────
    RenderTextOnScreen(meshList[GEO_TEXT],
        "WASD  - Move",
        glm::vec3(0.8f, 0.8f, 0.8f),
        22.f,
        270.f, 260.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "MOUSE - Look",
        glm::vec3(0.8f, 0.8f, 0.8f),
        22.f,
        270.f, 230.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "F     - Interact",
        glm::vec3(0.8f, 0.8f, 0.8f),
        22.f,
        270.f, 200.f);

    RenderTextOnScreen(meshList[GEO_TEXT],
        "LMB   - Shoot",
        glm::vec3(0.8f, 0.8f, 0.8f),
        22.f,
        270.f, 170.f);

    // ── "Press ENTER" prompt (blinking) ───────────────────────────────────
    if (showBlink)
    {
        RenderTextOnScreen(meshList[GEO_TEXT],
            "PRESS SPACE TO START",
            glm::vec3(1.f, 1.f, 0.f),   // bright yellow
            28.f,
            165.f, 110.f);
    }
}


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