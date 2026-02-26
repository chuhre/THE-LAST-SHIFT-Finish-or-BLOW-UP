#include "SceneManager.h"
#include "SceneLobby.h"
#include "SceneDucks.h"
#include "SceneShooting.h"
#include "SceneCans.h"
#include "SceneTank.h"
#include "SceneCutscene.h"
#include "SceneMenu.h"

SceneManager* SceneManager::m_instance = nullptr;

SceneManager::SceneManager(void)
    : currentScene(nullptr)
    , currentSceneType(SCENE_LOBBY)
    , nextSceneType(SCENE_LOBBY)
    , prevSceneType(SCENE_LOBBY)
    , needsSwitch(false)
{
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
    scenes[SCENE_LOBBY] = new SceneLobby();
    scenes[SCENE_DUCKS] = new SceneDucks();
    scenes[SCENE_SHOOTING] = new SceneShooting();
    scenes[SCENE_CANS] = new SceneCans();
    scenes[SCENE_TANK] = new SceneTank();
    scenes[SCENE_CUTSCENE] = new SceneCutscene();
    scenes[SCENE_MENU] = new SceneMenu();

    currentSceneType = SCENE_MENU;
    currentScene = scenes[currentSceneType];
    currentScene->Init();
}

void SceneManager::Update(double dt)
{
    if (needsSwitch)
    {
        if (currentScene)
            currentScene->Exit();

        currentSceneType = nextSceneType;
        currentScene = scenes[currentSceneType];

        if (currentScene)
            currentScene->Init();

        needsSwitch = false;
    }

    if (currentScene)
        currentScene->Update(dt);
}

void SceneManager::Render(void)
{
    if (currentScene)
        currentScene->Render();
}

void SceneManager::Exit(void)
{
    if (currentScene)
        currentScene->Exit();

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
    prevSceneType = currentSceneType;
    nextSceneType = sceneType;
    needsSwitch = true;
}

SceneManager::SCENE_TYPE SceneManager::GetCurrentSceneType(void)
{
    return currentSceneType;
}

SceneManager::SCENE_TYPE SceneManager::GetPreviousScene(void)
{
    return prevSceneType;
}