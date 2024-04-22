//
//  SceneDB.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/3/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "SceneDB.h"

// Scene Class:
// Update all of the actors in this scene
void Scene::UpdateActors()
{
    // Add all of the new actors to this scene
    for (auto actor : actorsToAdd)
    {
        actor->Start();
        
        // Initializes all components added to new actors at runtime
        actor->InitNewComponents();
        
        actors[actor->ID] = actor;
    }
    actorsToAdd.clear();
    
    // Process all of the components added to actors this frame
    for (auto actor : actors)
    {
        actor.second->ProcessAddedComponents();
    }
    
    // Update all actors
    for (auto actor : actors)
    {
        actor.second->Update();
    }
    
    // Late update all actors
    for (auto actor : actors)
    {
        actor.second->LateUpdate();
        
        // Initializes all components added to existing actors at runtime
        actor.second->InitNewComponents();
    }
    
    // Processes all of the components removed from actors this frame
    for (auto actor : actors)
    {
        actor.second->ProcessRemovedComponents();
    }
    
    // Destroys all of the needed actors
    for (auto actorID : actorsToDestroy)
    {
        Actor* toDestroy = nullptr;
        if (actors.find(actorID) != actors.end())
        {
            toDestroy = actors[actorID].get();
            actors.erase(actorID);
        }
        else
        {
            // Remove from the actors to add vector if this actor hasn't been loaded yet
            int indexToRemove = -1;
            for (int i = 0; i < actorsToAdd.size(); i++)
            {
                if (actorID == actorsToAdd[i]->ID)
                {
                    indexToRemove = i;
                }
            }
            
            if (indexToRemove != -1)
            {
                toDestroy = actorsToAdd[indexToRemove].get();
                actorsToAdd.erase(actorsToAdd.begin() + indexToRemove);
            }
        }
        
        // Removes them from the immortal actors vector if they are marked as immortal
        if (toDestroy != nullptr && !toDestroy->destroyOnLoad)
        {
            int indexToRemove = -1;
            for (int i = 0; i < immortalActors.size(); i++)
            {
                if (actorID == immortalActors[i]->ID)
                {
                    indexToRemove = i;
                }
            }
            
            if (indexToRemove != -1)
            {
                immortalActors.erase(immortalActors.begin() + indexToRemove);
            }
        }
        
        //delete actor;
    }
    actorsToDestroy.clear();
}

// Adds a new actor into this scene and returns a reference to it.
Actor* Scene::AddNewActor(std::string actor_template_name)
{
    // Ensures correct copying of components:
    Actor temp;
    temp = TemplateDB::GetTemplate(actor_template_name);
    
    std::shared_ptr<Actor> newActor = std::make_shared<Actor>(temp);
    actorsToAdd.push_back(newActor);
    
    newActor->InitNewComponents();
    
    return newActor.get();
}

// To be called when this scene is loaded into the current scene
void Scene::Init()
{
    // Adds all of the immortal actors to the actors vector since they've already been processed
    for (auto actor : immortalActors)
    {
        actors[actor->ID] = actor;
    }
    
    // Adds all of the default actors to the actorsToAdd vector
    for (auto actor : defaultActors)
    {
        // Ensures correct copying of components:
        Actor def;
        def = actor;
        
        std::shared_ptr<Actor> newActor = std::make_shared<Actor>(def);
        actorsToAdd.push_back(newActor);
    }
}

Scene::Scene()
{
}

// Constructs a new scene from a .scene file
Scene::Scene(rapidjson::Document *sceneDocument)
{
    if (sceneDocument->HasMember("actors"))
    {
        for (auto& member : (*sceneDocument)["actors"].GetArray())
        {
            Actor newActor;
            
            // If the actor has a value for template, set the actors values to the templates values.
            if (member.HasMember("template"))
            {
                newActor = TemplateDB::GetTemplate(member["template"].GetString());
            }
            
            newActor.LoadActorWithJSON(member);

            defaultActors.push_back(newActor);
        }
    }
}

Scene::~Scene()
{
    /*
    for (auto member : actors)
    {
        Actor* actor = member.second;
        if (actor->destroyOnLoad)
        {
            delete actor;
        }
    }
    */
}

// SceneDB (Data Base) Class:
// The current number of actors that have been loaded since the game started across all scenes
int SceneDB::numActorsLoaded = 0;
// The current scene that the game is in
Scene SceneDB::currentScene;
// Stores if loading a new scene is needed and what the name of the scene is
std::string SceneDB::nextScene = "";
bool SceneDB::loadNewScene = false;

// Loads all of the scenes in the resources/scenes directory into loadedScenes
void SceneDB::LoadScenes()
{
    /* Load scene files in resources/scenes */
    const std::string sceneDirectoryPath = "resources/scenes";
    
    // Fills up loadedScenes if the path exists
    if (std::filesystem::exists(sceneDirectoryPath)) {
        for (const auto& sceneFile : std::filesystem::directory_iterator(sceneDirectoryPath))
        {
            if (sceneFile.path() != sceneDirectoryPath + "/.DS_Store")
            {
                rapidjson::Document sceneDocument;
                EngineUtils::ReadJsonFile(sceneFile.path().string(), sceneDocument);
                
                Scene newScene(&sceneDocument);
                
                // Cameras bounds are the same as the windows
                newScene.camera.cameraHeight = Renderer::windowHeight;
                newScene.camera.cameraWidth = Renderer::windowWidth;
                newScene.camera.offsetX = Renderer::cameraOffsetX;
                newScene.camera.offsetY = Renderer::cameraOffsetY;
                newScene.camera.zoom = Renderer::zoomFactor;
                newScene.camera.ease = Renderer::easeFactor;
                
                newScene.name = sceneFile.path().filename().stem().stem().string();
                
                loadedScenes[newScene.name] = newScene;
            }
        }
    }
}

// Queues up a new scene to be loaded into the current scene
void SceneDB::ChangeScene(std::string newScene)
{
    nextScene = newScene;
    loadNewScene = true;
}

// Loads the new scene into the current scene
void SceneDB::LoadNewScene()
{
    // Destroys the current scene
    for (auto& member : currentScene.actors)
    {
        Destroy(member.second.get());
    }
    
    // Destroys the current scene
    for (auto& member : currentScene.actors)
    {
        member.second.get()->ProcessRemovedComponents();
    }
    
    Scene newCurrentScene = GetScene(nextScene);
    
    // Transfers the immortal actors
    newCurrentScene.immortalActors = currentScene.immortalActors;
    
    currentScene = newCurrentScene;
    currentScene.Init();
}

// Makes it so the given actor persists throughout scene loads
void SceneDB::DontDestroy(Actor* actor)
{
    std::shared_ptr<Actor> immortalActor = FindActorByID(actor->ID);
    
    immortalActor->destroyOnLoad = false;
    currentScene.immortalActors.push_back(immortalActor);
}

// Gets the name of the current scene
std::string SceneDB::GetCurrentSceneName()
{
    return currentScene.name;
}

// Get a scene from loadedScenes based on the scenes name
Scene SceneDB::GetScene(std::string sceneName)
{
    if (loadedScenes.find(sceneName) == loadedScenes.end())
    {
        std::cout << "error: scene " << sceneName << " is missing";
        exit(0);
    }
    return loadedScenes[sceneName];
}

// Finds an actor in the currentScene that has the provided name
// If multiple actors have this name this returns the one that was loaded first
Actor* SceneDB::FindActorWithName(std::string actor_name)
{
    // Actors that have already been loaded
    for (int i = 0; i < currentScene.actors.size(); i++)
    {
        Actor* actor = currentScene.actors[i].get();
        if (actor->name == actor_name && actor->enabled == true)
        {
            return actor;
        }
    }
    
    // Find should still be able to get actors that have been created this frame
    for (auto actor : currentScene.actorsToAdd)
    {
        if (actor->name == actor_name && actor->enabled == true)
        {
            return actor.get();
        }
    }
    
    return luabridge::LuaRef(ComponentDB::luaState);
}

// Finds all actors in the currentScene that have the provided name
luabridge::LuaRef SceneDB::FindAllActorsWithName(std::string actor_name)
{
    // Creates and gets a reference to a new table on the Lua stack
    lua_newtable(ComponentDB::luaState);
    luabridge::LuaRef actorsWithName(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
    
    actorsWithName[0] = luabridge::LuaRef(ComponentDB::luaState);
    
    int index = 1;
    
    for (auto actor : currentScene.actors)
    {
        if (actor.second->name == actor_name && actor.second->enabled == true)
        {
            actorsWithName[index] = actor.second.get();
            index++;
        }
    }
    
    for (auto actor : currentScene.actorsToAdd)
    {
        if (actor->name == actor_name && actor->enabled == true)
        {
            actorsWithName[index] = actor.get();
            index++;
        }
    }
    
    return actorsWithName;
}

// Finds an actor with the given UUID
std::shared_ptr<Actor> SceneDB::FindActorByID(int ID)
{
    // Actors that have already been loaded
    if (currentScene.actors.find(ID) != currentScene.actors.end())
    {
        if (currentScene.actors[ID]->enabled == true)
        {
            return currentScene.actors[ID];
        }
    }
    
    // Find should still be able to get actors that have been created this frame
    for (auto actor : currentScene.actorsToAdd)
    {
        if (actor->ID == ID && actor->enabled == true)
        {
            return actor;
        }
    }
    
    return luabridge::LuaRef(ComponentDB::luaState);
}

// Creates a new actor and adds it to the current scene
Actor* SceneDB::Instantiate(std::string actor_template_name)
{
    return currentScene.AddNewActor(actor_template_name);
}

// Destroys an actor and removes it from the sccene
void SceneDB::Destroy(Actor* actor)
{
    // Do not destroy an actor twice
    if (actor->destroyed) {return;}
    
    actor->destroyed = true;
    actor->enabled = false;
    
    // Adds all components to be removed
    for (auto& member : actor->components)
    {
        actor->componentsToRemove.push_back((*member.second)["key"]);
    }
    
    currentScene.actorsToDestroy.push_back(actor->ID);
}

// Sets camera position
void SceneDB::CameraSetPosition(float x, float y)
{
    currentScene.camera.position.x = x;
    currentScene.camera.position.y = y;
}

// Gets the current camera position
float SceneDB::CameraGetPositionX() {return currentScene.camera.position.x;};
float SceneDB::CameraGetPositionY() {return currentScene.camera.position.y;};

// Sets the current camera zoom
void SceneDB::SetZoom(float zoomFactor) {currentScene.camera.zoom = zoomFactor;};

// Gets the current camera zoom
float SceneDB::GetZoom() {return currentScene.camera.zoom;};
