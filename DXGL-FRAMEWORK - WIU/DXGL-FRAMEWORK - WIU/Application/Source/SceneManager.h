#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <bitset>
#include "Scene.h"

class SceneManager
{
public:
    enum SCENE_TYPE
    {
        SCENE_MENU = 0,
        SCENE_START,
        SCENE_END,
        SCENE_TOTAL
    };

private:
    static SceneManager* m_instance;

    Scene* scenes[SCENE_TOTAL];
    Scene* currentScene;
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

};
#endif

#pragma once
