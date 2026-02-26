#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// PauseMenu
//
// A lightweight overlay — NOT a Scene subclass.
// Any scene that wants pause functionality should:
//
//   1. Add a PauseMenu member:          PauseMenu pauseMenu;
//   2. In Init():                       pauseMenu.Init(m_programID, m_parameters);
//   3. In Update(), before other logic:
//        pauseMenu.Update(dt);
//        if (pauseMenu.IsPaused()) return;   // skip game logic while paused
//   4. In Render(), after world render:
//        if (pauseMenu.IsPaused()) pauseMenu.Render();
//   5. In Exit():                       pauseMenu.Exit();
//
// The menu borrows the calling scene's compiled shader program and uniform
// table so no extra shaders are needed.
// ─────────────────────────────────────────────────────────────────────────────

class PauseMenu
{
public:
    // Options the player can land on
    enum Option
    {
        OPTION_RESUME = 0,
        OPTION_MAIN_MENU,
        NUM_OPTIONS
    };

    PauseMenu();
    ~PauseMenu();

    // Call once from the owning scene's Init().
    // Passes the already-compiled programID and uniform location array
    // so PauseMenu never loads its own shaders.
    void Init(unsigned int programID, unsigned int parameters[]);

    // Call every frame from the owning scene's Update().
    // Handles ESC toggle, W/S navigation, and ENTER selection.
    void Update(double dt);

    // Call from the owning scene's Render() AFTER the 3-D world is drawn.
    // Only draws when IsPaused() is true.
    void Render();

    // Call from the owning scene's Exit() to free the font mesh.
    void Exit();

    // Returns true while the menu is open — use this to gate game logic.
    bool IsPaused() const { return m_isPaused; }

    // Returns the option the player just confirmed, or -1 if nothing yet.
    // Resets to -1 on the next call, so check it every frame.
    int  ConsumeSelection();

private:
    // ── Uniform enum (must match owning scene's UNIFORM_TYPE order) ───────
    // We only need a small subset; indices are passed in from the scene.
    enum U
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
        U_TOTAL
    };

    void RenderTextOnScreen(const std::string& text, glm::vec3 color,
        float size, float x, float y);

    // ── Borrowed from owning scene (do NOT delete these) ─────────────────
    unsigned int  m_programID;
    unsigned int* m_parameters;     // points at owning scene's array

    // ── Own resources ─────────────────────────────────────────────────────
    Mesh* m_textMesh;               // font quad, loaded in Init()

    MatrixStack m_projectionStack;
    MatrixStack m_viewStack;
    MatrixStack m_modelStack;

    // ── State ─────────────────────────────────────────────────────────────
    bool m_isPaused;
    int  m_selectedOption;          // 0 = Resume, 1 = Main Menu
    int  m_pendingSelection;        // set when ENTER pressed, consumed by scene

    // Debounce: ignore key until it has been released once
    bool m_pWasDown;
    bool m_upWasDown;
    bool m_downWasDown;
    bool m_enterWasDown;
};

#endif