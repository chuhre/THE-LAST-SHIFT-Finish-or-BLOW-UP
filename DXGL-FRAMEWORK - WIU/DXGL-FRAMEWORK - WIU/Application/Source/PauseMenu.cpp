#include "PauseMenu.h"

#include "GL\glew.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <GLFW/glfw3.h>

#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "LoadTGA.h"
#include "SceneManager.h"

// ─────────────────────────────────────────────────────────────────────────────
PauseMenu::PauseMenu()
    : m_programID(0)
    , m_parameters(nullptr)
    , m_textMesh(nullptr)
    , m_isPaused(false)
    , m_selectedOption(OPTION_RESUME)
    , m_pendingSelection(-1)
    , m_pWasDown(false)
    , m_upWasDown(false)
    , m_downWasDown(false)
    , m_enterWasDown(false)
{
}

PauseMenu::~PauseMenu()
{
}

// ─────────────────────────────────────────────────────────────────────────────
void PauseMenu::Init(unsigned int programID, unsigned int parameters[])
{
    m_programID = programID;
    m_parameters = parameters;   // borrow — do not free

    // Font atlas — same calibri.tga used across all scenes
    m_textMesh = MeshBuilder::GenerateText("pauseText", 16, 16);
    m_textMesh->textureID = LoadTGA("Images//calibri.tga");

    m_isPaused = false;
    m_selectedOption = OPTION_RESUME;
    m_pendingSelection = -1;

    m_pWasDown = false;
    m_upWasDown = false;
    m_downWasDown = false;
    m_enterWasDown = false;
}

// ─────────────────────────────────────────────────────────────────────────────
void PauseMenu::Update(double /*dt*/)
{
    KeyboardController* kb = KeyboardController::GetInstance();

    // ── P: toggle pause (press, not hold) ────────────────────────────────
    bool escDown = kb->IsKeyDown('P');
    if (escDown && !m_pWasDown)
    {
        m_isPaused = !m_isPaused;
        m_selectedOption = OPTION_RESUME;   // always reset cursor on open

        if (m_isPaused)
        {
            // Show and unlock cursor while paused
            glfwSetInputMode(glfwGetCurrentContext(),
                GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            // Hide cursor again when resuming
            glfwSetInputMode(glfwGetCurrentContext(),
                GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    m_pWasDown = escDown;

    // ── Navigation and selection only meaningful when paused ─────────────
    if (!m_isPaused)
        return;

    // ── W / Up Arrow: move cursor up ──────────────────────────────────────
    bool upDown = kb->IsKeyDown('W') || kb->IsKeyDown(VK_UP);
    if (upDown && !m_upWasDown)
    {
        m_selectedOption--;
        if (m_selectedOption < 0)
            m_selectedOption = NUM_OPTIONS - 1;
    }
    m_upWasDown = upDown;

    // ── S / Down Arrow: move cursor down ──────────────────────────────────
    bool downDown = kb->IsKeyDown('S') || kb->IsKeyDown(VK_DOWN);
    if (downDown && !m_downWasDown)
    {
        m_selectedOption++;
        if (m_selectedOption >= NUM_OPTIONS)
            m_selectedOption = 0;
    }
    m_downWasDown = downDown;

    // ── ENTER: confirm selection ──────────────────────────────────────────
    bool enterDown = kb->IsKeyDown(VK_RETURN);
    if (enterDown && !m_enterWasDown)
    {
        m_pendingSelection = m_selectedOption;

        if (m_selectedOption == OPTION_RESUME)
        {
            // Close the overlay immediately on Resume
            m_isPaused = false;
            glfwSetInputMode(glfwGetCurrentContext(),
                GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else if (m_selectedOption == OPTION_MAIN_MENU)
        {
            // Hand off to SceneManager — scene switch happens next frame
            SceneManager::GetInstance()->SwitchScene(SceneManager::SCENE_MENU);
        }
    }
    m_enterWasDown = enterDown;
}

// ─────────────────────────────────────────────────────────────────────────────
void PauseMenu::Render()
{
    if (!m_isPaused)
        return;

    glUseProgram(m_programID);

    // ── Semi-transparent dark overlay drawn in ortho space ───────────────
    // We draw all text in the same 800×600 ortho as the other scenes.

    // ── "PAUSED" title ───────────────────────────────────────────────────
    RenderTextOnScreen("PAUSED",
        glm::vec3(1.f, 0.85f, 0.f),    // golden yellow
        50.f, 295.f, 390.f);

    // ── Menu options ──────────────────────────────────────────────────────
    const char* labels[NUM_OPTIONS] = { "RESUME", "MAIN MENU" };

    float startY = 290.f;
    float stepY = 55.f;

    for (int i = 0; i < NUM_OPTIONS; ++i)
    {
        bool selected = (i == m_selectedOption);

        // Highlight colour: bright white when selected, dim grey otherwise
        glm::vec3 color = selected
            ? glm::vec3(1.f, 1.f, 0.f)     // yellow highlight
            : glm::vec3(0.6f, 0.6f, 0.6f); // unselected grey

        float size = selected ? 35.f : 30.f;

        // Arrow cursor to the left of the selected option
        float textX = 270.f;
        if (selected)
        {
            RenderTextOnScreen(">",
                glm::vec3(1.f, 1.f, 0.f), size,
                textX - 40.f, startY - i * stepY);
        }

        RenderTextOnScreen(labels[i], color, size,
            textX, startY - i * stepY);
    }

    // ── Controls hint at the bottom ───────────────────────────────────────
    RenderTextOnScreen("W/S - Navigate     ENTER - Confirm     P - Resume",
        glm::vec3(0.5f, 0.5f, 0.5f),
        18.f, 145.f, 80.f);
}

// ─────────────────────────────────────────────────────────────────────────────
void PauseMenu::Exit()
{
    if (m_textMesh)
    {
        delete m_textMesh;
        m_textMesh = nullptr;
    }
    // m_parameters is borrowed — do NOT delete it here
}

// ─────────────────────────────────────────────────────────────────────────────
int PauseMenu::ConsumeSelection()
{
    int sel = m_pendingSelection;
    m_pendingSelection = -1;
    return sel;
}

// ─────────────────────────────────────────────────────────────────────────────
// RenderTextOnScreen — same implementation as SceneShooting / SceneMenu,
// but uses the borrowed m_programID and m_parameters.
// ─────────────────────────────────────────────────────────────────────────────
void PauseMenu::RenderTextOnScreen(const std::string& text,
    glm::vec3 color,
    float size, float x, float y)
{
    if (!m_textMesh || m_textMesh->textureID <= 0)
        return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f);

    m_projectionStack.PushMatrix();
    m_projectionStack.LoadMatrix(ortho);
    m_viewStack.PushMatrix();
    m_viewStack.LoadIdentity();
    m_modelStack.PushMatrix();
    m_modelStack.LoadIdentity();
    m_modelStack.Translate(x, y, 0);
    m_modelStack.Scale(size, size, size);

    glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
    glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
    glUniform1i(m_parameters[U_LIGHTENABLED], 0);
    glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_textMesh->textureID);
    glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

    for (unsigned i = 0; i < text.length(); ++i)
    {
        glm::mat4 charSpacing = glm::translate(
            glm::mat4(1.f),
            glm::vec3(0.5f + i * 1.0f, 0.5f, 0.f));

        glm::mat4 MVP = m_projectionStack.Top() *
            m_viewStack.Top() *
            m_modelStack.Top() *
            charSpacing;

        glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE,
            glm::value_ptr(MVP));
        m_textMesh->Render((unsigned)text[i] * 6, 6);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
    m_projectionStack.PopMatrix();
    m_viewStack.PopMatrix();
    m_modelStack.PopMatrix();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}