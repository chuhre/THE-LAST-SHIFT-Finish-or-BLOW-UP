#include "SceneCutscene.h"

#include "GL\glew.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <GLFW/glfw3.h>

#include <string>

#include "shader.hpp"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "LoadTGA.h"
#include "SceneManager.h"

// Fade-in duration per page (seconds)
const float SceneCutscene::FADE_DURATION = 2.f;

// ─────────────────────────────────────────────────────────────────────────────
SceneCutscene::SceneCutscene()
    : currentPage(0)
    , fadeTimer(0.f)
    , enterWasDown(false)
{
}

SceneCutscene::~SceneCutscene()
{
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneCutscene::Init()
{
    glClearColor(0.f, 0.f, 0.f, 1.f);  // black void between pages
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glGenVertexArrays(1, &m_vertexArrayID);
    glBindVertexArray(m_vertexArrayID);

    // ── Shaders ───────────────────────────────────────────────────────────
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

    meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
    meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

    meshList[GEO_QUAD] = MeshBuilder::GenerateQuad("CutsceneBG", glm::vec3(1, 1, 1), 1.f);

    // ── Minimal light to satisfy shader ───────────────────────────────────
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

    glm::mat4 projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 1000.0f);
    projectionStack.LoadMatrix(projection);

   
    
    
    
    
    // ── Story pages ───────────────────────────────────────────────────────
    pages.clear();

    Page p1;
    p1.title = "THE CARNIVAL IS DOOMED.";
    p1.body = "Someone planted bombs in the booths.";
    p1.prompt = "Press SPACE to continue...";
    pages.push_back(p1);

    Page p2;
    p2.title = "THE ONLY WAY TO DEFUSE IT?";
    p2.body = "Complete every game in the carnival.";
    p2.prompt = "Press SPACE to continue...";
    pages.push_back(p2);

    Page p3;
    p3.title = "YOUR CHALLENGES AWAIT.";
    p3.body = "FULFILL YOUR INSPECTOR DUTY.";
    p3.prompt = "Press SPACE to continue...";
    pages.push_back(p3);

    Page p4;
    p4.title = "FINISH YOUR SHIFT.";
    p4.body = "Save the carnival. Defuse the bomb.";
    p4.prompt = "Press SPACE to begin...";
    pages.push_back(p4);

    // ── Reset state ───────────────────────────────────────────────────────
    currentPage = 0;
    fadeTimer = 0.f;
    enterWasDown = true;  // ignore held SPACE carried over from SceneMenu

    glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneCutscene::Update(double dt)
{
    // Tick fade-in timer — clamp at FADE_DURATION so alpha reaches 1
    if (fadeTimer < FADE_DURATION)
        fadeTimer += static_cast<float>(dt);

    HandleKeyPress();
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneCutscene::HandleKeyPress()
{
    bool enterDown = KeyboardController::GetInstance()->IsKeyDown(VK_SPACE);

    if (enterDown && !enterWasDown)
    {
        currentPage++;

        if (currentPage >= static_cast<int>(pages.size()))
        {
            // All pages done — head to the lobby
            SceneManager::GetInstance()->SwitchScene(SceneManager::SCENE_LOBBY);
        }
        else
        {
            // Reset fade for the new page
            fadeTimer = 0.f;
        }
    }

    enterWasDown = enterDown;
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneCutscene::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    viewStack.LoadIdentity();
    viewStack.LookAt(0, 0, 1, 0, 0, 0, 0, 1, 0);
    modelStack.LoadIdentity();

    // Guard against out-of-range (should never happen, but just in case)
    if (currentPage >= static_cast<int>(pages.size()))
        return;

    const Page& page = pages[currentPage];

    // ── Fade-in alpha (0 → 1 over FADE_DURATION seconds) ─────────────────
    float alpha = fadeTimer / FADE_DURATION;
    if (alpha > 1.f) alpha = 1.f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    meshList[GEO_QUAD]->material.kSpecular = glm::vec3(0.f, 0.f, 0.f);
    meshList[GEO_QUAD]->material.kShininess = 1.f;

    // ── Dark background quad (always fully opaque) ────────────────────────
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(0.05f, 0.f, 0.10f);
    meshList[GEO_QUAD]->material.kDiffuse = glm::vec3(0.05f, 0.f, 0.10f);
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 300.f, 800.f, 600.f);

    // ── Horizontal accent bar behind the title ────────────────────────────
    //    Fades in with the rest of the page content
    meshList[GEO_QUAD]->material.kAmbient = glm::vec3(0.60f * alpha, 0.05f * alpha, 0.05f * alpha);
    meshList[GEO_QUAD]->material.kDiffuse = meshList[GEO_QUAD]->material.kAmbient;
    RenderMeshOnScreen(meshList[GEO_QUAD], 400.f, 340.f, 700.f, 6.f);

    // ── Page counter dots at the bottom ───────────────────────────────────
    //    Filled dot for current page, dim dot for others
    float dotSpacing = 24.f;
    int   totalPages = static_cast<int>(pages.size());
    float startX = 400.f - (totalPages - 1) * dotSpacing * 0.5f;

    for (int i = 0; i < totalPages; ++i)
    {
        bool isCurrent = (i == currentPage);
        float brightness = isCurrent ? 0.90f : 0.25f;

        meshList[GEO_QUAD]->material.kAmbient = glm::vec3(
            brightness * (isCurrent ? 1.f : 0.8f),
            brightness * (isCurrent ? 0.7f : 0.7f),
            brightness * (isCurrent ? 0.f : 0.7f)
        );
        meshList[GEO_QUAD]->material.kDiffuse = meshList[GEO_QUAD]->material.kAmbient;
        RenderMeshOnScreen(meshList[GEO_QUAD],
            startX + i * dotSpacing, 40.f, 12.f, 12.f);
    }

    glDisable(GL_BLEND);

    // ── Title (fades in) ──────────────────────────────────────────────────
    glm::vec3 titleColor(1.f * alpha, 0.85f * alpha, 0.f);
    RenderTextOnScreen(meshList[GEO_TEXT],
        page.title, titleColor,
        26.f,
        400.f - (page.title.length() * 26.f * 0.5f),   // rough centre
        370.f);

    // ── Body (fades in, slightly delayed — starts at 30% of fade) ─────────
    float bodyAlpha = (alpha - 0.3f) / 0.7f;
    if (bodyAlpha < 0.f) bodyAlpha = 0.f;
    if (bodyAlpha > 1.f) bodyAlpha = 1.f;

    glm::vec3 bodyColor(1.f * bodyAlpha, 0.75f * bodyAlpha, 0.50f * bodyAlpha);
    RenderTextOnScreen(meshList[GEO_TEXT],
        page.body, bodyColor,
        20.f,
        400.f - (page.body.length() * 20.f * 0.5f),
        305.f);

    // ── Prompt — only shown once fully faded in ────────────────────────────
    if (alpha >= 1.f)
    {
        RenderTextOnScreen(meshList[GEO_TEXT],
            page.prompt,
            glm::vec3(0.55f, 0.55f, 0.55f),
            16.f,
            400.f - (page.prompt.length() * 16.f * 0.5f),
            80.f);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneCutscene::Exit()
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
void SceneCutscene::RenderMesh(Mesh* mesh, bool enableLight)
{
    glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
    glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));

    glm::mat4 modelView = viewStack.Top() * modelStack.Top();
    glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, glm::value_ptr(modelView));

    if (enableLight)
    {
        glUniform1i(m_parameters[U_LIGHTENABLED], 1);
        glm::mat4 mit = glm::inverseTranspose(modelView);
        glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE,
            glm::value_ptr(mit));
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
void SceneCutscene::RenderMeshOnScreen(Mesh* mesh, float x, float y,
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

    RenderMesh(mesh, false);

    projectionStack.PopMatrix();
    viewStack.PopMatrix();
    modelStack.PopMatrix();
    glEnable(GL_DEPTH_TEST);
}

// ─────────────────────────────────────────────────────────────────────────────
void SceneCutscene::RenderTextOnScreen(Mesh* mesh, std::string text,
    glm::vec3 color, float size,
    float x, float y)
{
    if (!mesh || mesh->textureID <= 0)
        return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f);

    projectionStack.PushMatrix();
    projectionStack.LoadMatrix(ortho);
    viewStack.PushMatrix();
    viewStack.LoadIdentity();
    modelStack.PushMatrix();
    modelStack.LoadIdentity();
    modelStack.Translate(x, y, 0.f);
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
            glm::vec3(0.5f + i * 1.0f, 0.5f, 0.f));
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