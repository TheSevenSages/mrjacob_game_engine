//
//  SceneDB.h
//  game_engine
//
//  Created by Jacob Robinson on 2/2/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef SceneDB_h
#define SceneDB_h

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
#include <queue>
#include <stdio.h>
#include <filesystem>
#include <cstdlib>
#include <cmath>
#include <math.h>
#include <unordered_map>
#include <map>
#include <memory>

#include "lua.hpp"
#include "LuaBridge.h"

#include "EngineUtils.h"
#include "TemplateDB.h"
#include "AudioDB.h"
#include "Camera.h"
#include "Actor.h"
#include "Renderer.h"
#include "glm/glm.hpp" // Student : You need to get glm added to your project source code or this line will fail.
#include  "glm/gtx/hash.hpp"

class Scene
{
public:
    
    // The name of this scene
    std::string name;
    
    // The games camera
    Camera camera;
    
    // A list of actors in the scene, in the order that they were loaded
    std::map<int, std::shared_ptr<Actor>> actors;
    
    // A list of actors that need to be added to the scene this frame
    std::vector<std::shared_ptr<Actor>> actorsToAdd;
    
    // A list of actors UUIDs that need to be removed from the scene this frame
    std::vector<int> actorsToDestroy;

    // Actors to not destroy when a new scene is loaded
    std::vector<std::shared_ptr<Actor>> immortalActors;
    
    // Update all of the actors in this scene
    void UpdateActors();
    
    // Adds a new actor into this scene and returns a reference to it.
    Actor* AddNewActor(std::string actor_template_name);
    
    // To be called when this scene is loaded into the current scene
    void Init();
    
    // Constructs a new scene from a .scene file
    Scene(rapidjson::Document *sceneDocument);
    
    Scene();
    
    ~Scene();
private:
    // The memory location of all the default actors in the scene
    std::vector<Actor> defaultActors;
};

class SceneDB
{
public:
    // The current number of actors that have been loaded since the game started across all scenes
    static int numActorsLoaded;
    
    // The current scene that the game is in
    static Scene currentScene;
    
    // Stores if loading a new scene is needed and what the name of the scene is
    static std::string nextScene;
    static bool loadNewScene;
    
    // Loads all of the scenes in the resources/scenes directory into loadedScenes
    static void LoadScenes();
    
    // Queues up a new scene to be loaded into the current scene
    static void ChangeScene(std::string newScene);
    
    // Loads the new scene into the current scene
    static void LoadNewScene();
    
    // Makes it so the given actor persists throughout scene loads
    static void DontDestroy(Actor* actor);
    
    // Gets the name of the current scene
    static std::string GetCurrentSceneName();
    
    // Get a scene from loadedScenes based on the scenes name
    static Scene GetScene(std::string sceneName);
    
    // Finds an actor in the currentScene that has the provided name
    // If multiple actors have this name this returns the one that was loaded first
    static Actor* FindActorWithName(std::string actor_name);
    
    // Finds all actors in the currentScene that have the provided name
    static luabridge::LuaRef FindAllActorsWithName(std::string actor_name);
    
    // Finds an actor with the given UUID
    static std::shared_ptr<Actor> FindActorByID(int ID);
    
    // Creates a new actor and adds it to the current scene, then returns a reference to it
    static Actor* Instantiate(std::string actor_template_name);
    
    // Destroys an actor and removes it from the sccene
    static void Destroy(Actor* actor);
    
    // Sets camera position
    static void CameraSetPosition(float x, float y);
    
    // Gets the current camera position
    static float CameraGetPositionX();
    static float CameraGetPositionY();
    
    // Sets the current camera zoom
    static void SetZoom(float zoomFactor);
    
    // Gets the current camera zoom
    static float GetZoom();
private:
    static inline std::unordered_map<std::string, Scene> loadedScenes;
};

#endif /* SceneDB_h */
