#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <bitset>
#include "Scene.h"


class SceneLobby;
class SceneDucks;
class SceneShooting;
class SceneCans;
class SceneTank;


class SceneManager
{
public:
    enum SCENE_TYPE
    {
        SCENE_MENU,
        SCENE_LOBBY = 0,
        SCENE_DUCKS,
        SCENE_SHOOTING,
        SCENE_CANS,
        SCENE_TANK,
        SCENE_END,
        SCENE_TOTAL
    };

private:
    static SceneManager* m_instance;

    Scene* scenes[SCENE_TOTAL];
    Scene* currentScene;
    SCENE_TYPE prevSceneType;
    SCENE_TYPE currentSceneType;
    SCENE_TYPE nextSceneType;
    bool needsSwitch;

    SceneManager(void);
    ~SceneManager(void);

public:
    static SceneManager* GetInstance(void);
    static void DestroyInstance(void);

    void Init(void);
    void Update(double dt);
    void Render(void);
    void Exit(void);

    void SwitchScene(SCENE_TYPE sceneType);
    SCENE_TYPE GetCurrentSceneType(void);
    SCENE_TYPE leadsTo;
    bool gameCompleted[4];    // track which games are done
    bool getIsGameCompleted(int index) { return gameCompleted[index]; }

};
#endif

#pragma once
