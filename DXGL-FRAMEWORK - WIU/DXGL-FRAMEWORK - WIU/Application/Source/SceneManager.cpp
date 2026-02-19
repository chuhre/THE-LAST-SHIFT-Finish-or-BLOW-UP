#include "SceneManager.h"

SceneManager* SceneManager::m_instance = nullptr;

SceneManager::SceneManager(void)
    : currentScene(nullptr)
    , currentSceneType(SCENE_MENU)
    , nextSceneType(SCENE_MENU)
    , needsSwitch(false)
{
    // Initialize all scene pointers to nullptr
    for (int i = 0; i < SCENE_TOTAL; i++)
    {
        scenes[i] = nullptr;
    }
}

SceneManager::~SceneManager(void)
{
}

SceneManager* SceneManager::GetInstance(void)
{
    if (m_instance == nullptr)
    {
        m_instance = new SceneManager();
    }
    return m_instance;
}

void SceneManager::DestroyInstance(void)
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
}

void SceneManager::Init(void)
{
    // Create all scenes
    /*scenes[SCENE_MENU] = new SceneMenu();
    scenes[SCENE_START] = new SceneStart();
    scenes[SCENE_END] = new SceneEnd();*/

    // Initialize the first scene
    currentSceneType = SCENE_START;
    currentScene = scenes[currentSceneType];
    currentScene->Init();
}

void SceneManager::Update(double dt)
{
    // Handle scene switching if needed
    if (needsSwitch)
    {
        // Exit current scene
        if (currentScene)
        {
            currentScene->Exit();
        }

        // Switch to next scene
        currentSceneType = nextSceneType;
        currentScene = scenes[currentSceneType];

        // Initialize new scene
        if (currentScene)
        {
            currentScene->Init();
        }

        needsSwitch = false;
    }

    // Update current scene
    if (currentScene)
    {
        currentScene->Update(dt);
    }
}

void SceneManager::Render(void)
{
    if (currentScene)
    {
        currentScene->Render();
    }
}

void SceneManager::Exit(void)
{
    // Exit current scene
    if (currentScene)
    {
        currentScene->Exit();
    }

    // Delete all scenes
    for (int i = 0; i < SCENE_TOTAL; i++)
    {
        if (scenes[i])
        {
            delete scenes[i];
            scenes[i] = nullptr;
        }
    }
}

void SceneManager::SwitchScene(SCENE_TYPE sceneType)
{
    nextSceneType = sceneType;
    needsSwitch = true;
}

SceneManager::SCENE_TYPE SceneManager::GetCurrentSceneType(void)
{
    return currentSceneType;
}