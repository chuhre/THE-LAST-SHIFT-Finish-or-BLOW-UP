#include "SceneManager.h"
#include "SceneLobby.h"
#include "SceneDucks.h"
#include "SceneShooting.h"
#include "SceneCans.h"
#include "SceneTank.h"

SceneManager* SceneManager::m_instance = nullptr;

SceneManager::SceneManager(void)
    : currentScene(nullptr)
	, currentSceneType(SCENE_LOBBY) //switch to lobby first since menu is not implemented yet
    , nextSceneType(SCENE_LOBBY)
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
    //scenes[SCENE_MENU] = new SceneMenu();
	scenes[SCENE_LOBBY] = new SceneLobby();
    scenes[SCENE_DUCKS] = new SceneDucks();
    scenes[SCENE_SHOOTING] = new SceneShooting();
    scenes[SCENE_CANS] = new SceneCans();
    scenes[SCENE_TANK] = new SceneTank();

    // Initialize the first scene
    currentSceneType = SCENE_LOBBY;
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