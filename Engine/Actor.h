//
//  Actor.h
//  game_engine
//
//  Created by Jacob Robinson on 2/18/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef Actor_h
#define Actor_h

#include <unordered_set>
#include <memory>

#include "lua.hpp"
#include "LuaBridge.h"

#include "SDL2_image/SDL_image.h"
#include "EngineUtils.h"
#include "ImageDB.h"
#include "ComponentDB.h"
#include "Renderer.h"
#include "glm/glm.hpp" // Student : You need to get glm added to your project source code or this line will fail.
#include "PhysicsHandler.h"

class Actor;

class ActorDB
{
public:
    static int numAddedComponents;
};

class Actor
{
private:
    
public:
    std::string name = "";
    int ID = -1;
    
    bool enabled = true;
    bool destroyOnLoad = true;
    bool destroyed = false;
    
    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> components;
    std::map<std::string, std::shared_ptr<luabridge::LuaRef>> componentsToAdd;
    std::vector<std::string> componentsToRemove;
    
    std::queue<std::string> componentsToUpdate;
    std::queue<std::string> componentsToUpdateLate;
    std::unordered_set<std::string> componentsWithOnDestroy;
    
    std::priority_queue<std::string> componentsWithCollisionEnter;
    std::priority_queue<std::string> componentsWithCollisionExit;
    std::priority_queue<std::string> componentsWithTriggerEnter;
    std::priority_queue<std::string> componentsWithTriggerExit;
    
    
    // Called the frame an actor is loaded
    void Start();
    
    // Processes all components added to the actor on this frame
    void ProcessAddedComponents();
    
    // Calls "OnUpdate" for every component on this actor that has it
    void Update();
    
    // Calls "OnLateUpdate" for every component on this actor that has it
    void LateUpdate();
    
    // Processes all components removed from the actor on this frame
    void ProcessRemovedComponents();
    
    // Calls OnCollisionEnter on every component on this actor that has it
    void CollisionEnter(Collision col);
    
    // Calls OnCollisionExit on every component on this actor that has it
    void CollisionExit(Collision col);

    // Calls OnTriggerEnter on every component on this actor that has it
    void TriggerEnter(Collision col);

    // Calls OnTriggerExit on every component on this actor that has it
    void TriggerExit(Collision col);
    
    // Returns this actors name
    std::string GetName();
    
    // Returns this actors ID
    int GetID();
    
    // Adds a new component of the type name given to this actor and returns a reference to it
    luabridge::LuaRef AddComponent(std::string type_name);
    
    // Removes a given component from this actor
    void RemoveComponent(luabridge::LuaRef component_ref);
    
    // Initializes new components created this frame
    void InitNewComponents();
    
    // Returns the component on this actor that corresponds to the given key
    luabridge::LuaRef GetComponentByKey(std::string key);
    
    // Returns a component on this actor that is of the given type
    // If multiple components have this type this returns the first one (sorted by its key)
    luabridge::LuaRef GetComponent(std::string type_name);
    
    // Returns all of the components on this actor that are of the given type
    luabridge::LuaRef GetComponents(std::string type_name);
    
    // Injects a reference to this actor into the components so that developers can get the actor that a component is on.
    void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref);
    
    // Initializes an actor with the values in JSON code
    void LoadActorWithJSON(const rapidjson::Value& actorData);
    
    Actor() {}
    
    // Assignment operator
    Actor& operator=(const Actor &copiedActor);
};

#endif /* Actor_h */
