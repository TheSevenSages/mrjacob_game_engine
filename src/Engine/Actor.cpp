//
//  Actor.cpp
//  game_engine
//
//  Created by Jacob Robinson on 3/1/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "Actor.h"
#include "SceneDB.h"
#include "ParticleSystem.h"

// ActorDB class
int ActorDB::numAddedComponents = 0;

// Actor class
// Called the frame an actor is loaded
void Actor::Start()
{
    // Gives actor their ID
    ID = SceneDB::numActorsLoaded;
    SceneDB::numActorsLoaded += 1;
}

// Processes all components added to the actor on this frame
void Actor::ProcessAddedComponents()
{
    // Don't update this Actor if it isn't enabled
    if (enabled == false) {return;};
    std::vector<std::string> processed;
    
    for (auto& component : componentsToAdd)
    {
        // Don't start this component if it isn't enabled
        if ((*component.second)["enabled"] == false) {continue;};
        
        // Mark this component as processed
        (*component.second)["started"] = true;
        processed.push_back(component.first);
        
        try
        {
            // OnStart is called for each component the frame they are loaded into the game
            luabridge::LuaRef OnStart = (*component.second)["OnStart"];
            if (OnStart.isFunction())
            {
                OnStart(*component.second);
            }
        }
        catch(const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
    
    // Removes all the processed components from "componentsToAdd"
    for (std::string key : processed)
    {
        componentsToAdd.erase(key);
    }
}

// Calls "OnUpdate" for every component on this actor
void Actor::Update()
{
    // Don't both with updates if there are none
    if (componentsToUpdate.empty()) {return;}
    
    size_t size = componentsToUpdate.size();
    for (int i = 0; i < size; i++)
    {
        // Don't update this Actor if it isn't enabled
        if (enabled == false) {return;};
        
        std::string key = componentsToUpdate.front();
        componentsToUpdate.pop();
        
        if (components.find(key) == components.end()) {continue;}
        
        std::shared_ptr<luabridge::LuaRef> component = components[key];
        
        // Push this component to the back of the queue
        componentsToUpdate.push(key);
        
        // Don't update this component if it isn't enabled
        if ((*component)["enabled"] == false) {continue;};
        
        // Don't update this component if it hasn't had a chance to run its "OnStart" function (if it exists)
        if ((*component)["started"] == false) {continue;};
        
        try
        {
            // OnUpdate is called for each component every single frame
            luabridge::LuaRef OnUpdate = (*component)["OnUpdate"];
            if (OnUpdate.isFunction())
            {
                OnUpdate(*component);
            }
        }
        catch(const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
    
}

// Calls "OnLateUpdate" for every component on this actor
void Actor::LateUpdate()
{
    // Don't both with lateUpdates if there are none
    if (componentsToUpdateLate.empty()) {return;}
    
    size_t size = componentsToUpdateLate.size();
    for (int i = 0; i < size; i++)
    {
        // Don't update this Actor if it isn't enabled
        if (enabled == false) {return;};
        
        std::string key = componentsToUpdateLate.front();
        componentsToUpdateLate.pop();
        
        if (components.find(key) == components.end()) {continue;}
        
        std::shared_ptr<luabridge::LuaRef> component = components[key];
        
        // Push this component to the back of the queue
        componentsToUpdateLate.push(key);
        
        // Don't update this component if it isn't enabled
        if ((*component)["enabled"] == false) {continue;};
        
        // Don't update this component if it hasn't had a chance to run its "OnStart" function (if it exists)
        if ((*component)["started"] == false) {continue;};
        
        try
        {
            // OnLateUpdate is called after all OnUpdates for each component every single frame
            luabridge::LuaRef OnLateUpdate = (*component)["OnLateUpdate"];
            if (OnLateUpdate.isFunction())
            {
                OnLateUpdate(*component);
            }
        }
        catch(const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
}

// Processes all components removed from the actor on this frame
void Actor::ProcessRemovedComponents()
{
    for (auto& component_key : componentsToRemove)
    {
        // Calls "OnDestroy" if this component contains that lifecycle function
        if (componentsWithOnDestroy.find(component_key) != componentsWithOnDestroy.end())
        {
            luabridge::LuaRef OnDestroy = (*components[component_key])["OnDestroy"];
            OnDestroy(*components[component_key]);
            componentsWithOnDestroy.erase(component_key);
        }
        
        components.erase(component_key);
    }
    componentsToRemove.clear();
}

// Calls OnCollisionEnter on every component on this actor that has it
void Actor::CollisionEnter(Collision col)
{
    // Don't both with calls if there are none
    if (componentsWithCollisionEnter.empty()) {return;}
    
    size_t size = componentsWithCollisionEnter.size();
    for (int i = 0; i < size; i++)
    {
        // Don't update this Actor if it isn't enabled
        if (enabled == false) {return;};
        
        std::string key = componentsWithCollisionEnter.top();
        componentsWithCollisionEnter.pop();
        
        if (components.find(key) == components.end()) {continue;}
        
        std::shared_ptr<luabridge::LuaRef> component = components[key];
        
        // Push this component to the back of the queue
        componentsWithCollisionEnter.push(key);
        
        // Don't update this component if it isn't enabled
        if ((*component)["enabled"] == false) {continue;};
        
        // Don't update this component if it hasn't had a chance to run its "OnStart" function (if it exists)
        if ((*component)["started"] == false) {continue;};
        
        try
        {
            // OnUpdate is called for each component every single frame
            luabridge::LuaRef OnCollisionEnter = (*component)["OnCollisionEnter"];
            if (OnCollisionEnter.isFunction())
            {
                OnCollisionEnter(*component, col);
            }
        }
        catch(const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
}

// Calls OnCollisionEnter on every component on this actor that has it
void Actor::CollisionExit(Collision col)
{
    // Don't both with calls if there are none
    if (componentsWithCollisionExit.empty()) {return;}
    
    size_t size = componentsWithCollisionExit.size();
    for (int i = 0; i < size; i++)
    {
        // Don't update this Actor if it isn't enabled
        if (enabled == false) {return;};
        
        std::string key = componentsWithCollisionExit.top();
        componentsWithCollisionExit.pop();
        
        if (components.find(key) == components.end()) {continue;}
        
        std::shared_ptr<luabridge::LuaRef> component = components[key];
        
        // Push this component to the back of the queue
        componentsWithCollisionExit.push(key);
        
        // Don't update this component if it isn't enabled
        if ((*component)["enabled"] == false) {continue;};
        
        // Don't update this component if it hasn't had a chance to run its "OnStart" function (if it exists)
        if ((*component)["started"] == false) {continue;};
        
        try
        {
            // OnUpdate is called for each component every single frame
            luabridge::LuaRef OnCollisionExit = (*component)["OnCollisionExit"];
            if (OnCollisionExit.isFunction())
            {
                OnCollisionExit(*component, col);
            }
        }
        catch(const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
}


// Calls OnTriggerEnter on every component on this actor that has it
void Actor::TriggerEnter(Collision col)
{
    // Don't both with calls if there are none
    if (componentsWithTriggerEnter.empty()) { return; }

    size_t size = componentsWithTriggerEnter.size();
    for (int i = 0; i < size; i++)
    {
        // Don't update this Actor if it isn't enabled
        if (enabled == false) { return; };

        std::string key = componentsWithTriggerEnter.top();
        componentsWithTriggerEnter.pop();

        if (components.find(key) == components.end()) { continue; }

        std::shared_ptr<luabridge::LuaRef> component = components[key];

        // Push this component to the back of the queue
        componentsWithTriggerEnter.push(key);

        // Don't update this component if it isn't enabled
        if ((*component)["enabled"] == false) { continue; };

        // Don't update this component if it hasn't had a chance to run its "OnStart" function (if it exists)
        if ((*component)["started"] == false) { continue; };

        try
        {
            // OnUpdate is called for each component every single frame
            luabridge::LuaRef OnTriggerEnter = (*component)["OnTriggerEnter"];
            if (OnTriggerEnter.isFunction())
            {
                OnTriggerEnter(*component, col);
            }
        }
        catch (const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
}

// Calls OnTriggerExit on every component on this actor that has it
void Actor::TriggerExit(Collision col)
{
    // Don't both with calls if there are none
    if (componentsWithTriggerExit.empty()) { return; }

    size_t size = componentsWithTriggerExit.size();
    for (int i = 0; i < size; i++)
    {
        // Don't update this Actor if it isn't enabled
        if (enabled == false) { return; };

        std::string key = componentsWithTriggerExit.top();
        componentsWithTriggerExit.pop();

        if (components.find(key) == components.end()) { continue; }

        std::shared_ptr<luabridge::LuaRef> component = components[key];

        // Push this component to the back of the queue
        componentsWithTriggerExit.push(key);

        // Don't update this component if it isn't enabled
        if ((*component)["enabled"] == false) { continue; };

        // Don't update this component if it hasn't had a chance to run its "OnStart" function (if it exists)
        if ((*component)["started"] == false) { continue; };

        try
        {
            // OnUpdate is called for each component every single frame
            luabridge::LuaRef OnTriggerExit = (*component)["OnTriggerExit"];
            if (OnTriggerExit.isFunction())
            {
                OnTriggerExit(*component, col);
            }
        }
        catch (const std::exception& e)
        {
            std::string errorMessage = e.what();
#ifdef _WIN32
            std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');
#endif
            std::cout << "\033[31m" << name << " : " << errorMessage << "\033[0m" << std::endl;
        }
    }
}

// Returns this actors name
std::string Actor::GetName()
{
    return name;
}

// Returns this actors ID
int Actor::GetID()
{
    return ID;
}

// Adds a new component of the type name given to this actor and returns a reference to it
luabridge::LuaRef Actor::AddComponent(std::string type_name)
{
    // Creates and gets a reference to a new table on the Lua stack
    lua_newtable(ComponentDB::luaState);
    luabridge::LuaRef newComponent(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
    
    // Access the key of the component
    std::string key = "r" + std::to_string(ActorDB::numAddedComponents);
    ActorDB::numAddedComponents++;
    
    // Different creation methods if this component is C++ or Lua based
    if (ComponentDB::IsComponentTypeCPP(type_name))
    {
        newComponent = ComponentDB::NewCPPComponent(type_name);
    }
    else
    {
        ComponentDB::EstablishInheritance(newComponent, *ComponentDB::GetComponent(type_name));
    }
    
    newComponent["type"] = type_name;

    // Gives the component its key
    newComponent["key"] = key;
    
    // Sets the component to be enabled by default
    newComponent["enabled"] = true;
    
    // "Started" will be false unless the component has had a chance to try and run its "OnStart" function if one exists.
    // This is to prevent runtime-enabled components from running "OnUpdate" before "OnStart"
    newComponent["started"] = false;
    
    componentsToAdd[key] = std::make_shared<luabridge::LuaRef>(newComponent);
    
    InjectConvenienceReferences(componentsToAdd[key]);
    
    return newComponent;
}

// Removes a given component from this actor
void Actor::RemoveComponent(luabridge::LuaRef component_ref)
{
    component_ref["enabled"] = false;
    componentsToRemove.push_back(component_ref["key"]);
}

// Initializes new components created this frame
void Actor::InitNewComponents()
{
    for (auto& component : componentsToAdd)
    {
        std::string key = (*component.second)["key"].tostring();
        
        // Skip if this component has already been initialized
        if (components.find(key) != components.end()) {continue;}
        
        // Injects the reference for all components before "OnStart" is called.
        // This allows Lua scripts to get the actor for another script during "OnStart".
        InjectConvenienceReferences(component.second);
    
        // Add component to the components list
        // Doing this before the new component is processed allows new components to access each other before they are all processed.
        components[key] = component.second;
        
        // If this component has an update function add it to the queue
        luabridge::LuaRef OnUpdate = (*component.second)["OnUpdate"];
        if (OnUpdate.isFunction())
        {
            componentsToUpdate.push(key);
        }
        // If this component has a late update function add it to the queue
        luabridge::LuaRef OnLateUpdate = (*component.second)["OnLateUpdate"];
        if (OnLateUpdate.isFunction())
        {
            componentsToUpdateLate.push(key);
        }
        
        // If this component has a OnCollisionEnter function add it to the queue
        luabridge::LuaRef OnCollisionEnter = (*component.second)["OnCollisionEnter"];
        if (OnCollisionEnter.isFunction())
        {
            componentsWithCollisionEnter.push(key);
        }
        // If this component has a OnCollisionExit function add it to the queue
        luabridge::LuaRef OnCollisionExit = (*component.second)["OnCollisionExit"];
        if (OnCollisionExit.isFunction())
        {
            componentsWithCollisionExit.push(key);
        }

        // If this component has a OnTriggerEnter function add it to the queue
        luabridge::LuaRef OnTriggerEnter = (*component.second)["OnTriggerEnter"];
        if (OnTriggerEnter.isFunction())
        {
            componentsWithTriggerEnter.push(key);
        }

        // If this component has a OnTriggerExit function add it to the queue
        luabridge::LuaRef OnTriggerExit = (*component.second)["OnTriggerExit"];
        if (OnTriggerExit.isFunction())
        {
            componentsWithTriggerExit.push(key);
        }
        
        // If this component has an OnDestroy function add it to the set
        luabridge::LuaRef OnDestroy = (*component.second)["OnDestroy"];
        if (OnDestroy.isFunction())
        {
            componentsWithOnDestroy.insert(key);
        }
    }
}

// Returns the component on this actor that corresponds to the given key
luabridge::LuaRef Actor::GetComponentByKey(std::string key)
{
    // Return null if there are no components on this actor
    if (components.empty()) {return luabridge::LuaRef(ComponentDB::luaState);}
    
    if (components.find(key) != components.end())
    {
        if ((*components[key])["enabled"] == true)
        {
            return *components[key];
        }
    }
    
    return luabridge::LuaRef(ComponentDB::luaState);
}

// Returns a component on this actor that is of the given type
// If multiple components have this type this returns the first one (sorted by its key)
luabridge::LuaRef Actor::GetComponent(std::string type_name)
{
    // Return null if there are no components on this actor
    if (components.empty()) {return luabridge::LuaRef(ComponentDB::luaState);}
    
    for (auto& component : components)
    {
        if ((*component.second)["type"].tostring() == type_name && (*component.second)["enabled"] == true)
        {
            return *component.second;
        }
    }
    
    return luabridge::LuaRef(ComponentDB::luaState);
}

// Returns all of the components on this actor that are of the given type
luabridge::LuaRef Actor::GetComponents(std::string type_name)
{
    // Return null if there are no components on this actor
    if (components.empty()) {return luabridge::LuaRef(ComponentDB::luaState);}
    
    // Creates and gets a reference to a new table on the Lua stack
    lua_newtable(ComponentDB::luaState);
    luabridge::LuaRef componentsOfType(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
    
    componentsOfType[0] = luabridge::LuaRef(ComponentDB::luaState);
    int i = 1;
    
    for (auto& component : components)
    {
        if ((*component.second)["type"] == type_name && (*component.second)["enabled"] == true)
        {
            componentsOfType[i] = *component.second;
            i++;
        }
    }
    
    return componentsOfType;
}

// Injects a reference to this actor into the components so that developers can get the actor that a component is on.
void Actor::InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref)
{
    (*component_ref)["actor"] = this;
}

// Initializes an actor with the values in JSON code
void Actor::LoadActorWithJSON(const rapidjson::Value& actorData)
{
    // Assigns the values to the new actor
    if (actorData.HasMember("name"))
    {
        name = actorData["name"].GetString();
    }
    
    // Components
    if (actorData.HasMember("components"))
    {
        const rapidjson::Value& actorComponents = actorData["components"];
        // Iterate over each component
        for (rapidjson::Value::ConstMemberIterator itr = actorComponents.MemberBegin(); itr != actorComponents.MemberEnd(); itr++)
        {
            // Creates and gets a reference to a new table on the Lua stack
            lua_newtable(ComponentDB::luaState);
            luabridge::LuaRef newComponent(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
            
            // Access the key of the component
            std::string key = itr->name.GetString();
            
            // Establishes inheritance between the new component and its type if specified
            if (itr->value.HasMember("type"))
            {
                std::string type = itr->value["type"].GetString();
                
                // Different creation methods if this component is C++ or Lua based
                if (ComponentDB::IsComponentTypeCPP(type))
                {
                    newComponent = ComponentDB::NewCPPComponent(type);
                }
                else
                {
                    ComponentDB::EstablishInheritance(newComponent, *ComponentDB::GetComponent(type));
                }
                
                newComponent["type"] = type;
            }
            // If the type is not specified for this component, check if its already been set
            else if (componentsToAdd.find(key) != componentsToAdd.end())
            {
                // Makes our new component a copy of the templated one
                if (ComponentDB::IsComponentTypeCPP((*componentsToAdd[key])["type"]))
                {
                    newComponent = ComponentDB::CopyCPPComponent(componentsToAdd[key], (*componentsToAdd[key])["type"]);
                }
                else
                {
                    ComponentDB::EstablishInheritance(newComponent, *componentsToAdd[key]);
                }
            }
            // If the type for this component is not specified anywhere, throw an error
            else
            {
                std::cout << "error: component type unspecified for " << key << " on " << name;
                exit(0);
            }
            
            // Gives the component its key
            newComponent["key"] = key;
            
            // Sets the component to be enabled by default
            newComponent["enabled"] = true;
            
            // "Started" will be false unless the component has had a chance to try and run its "OnStart" function if one exists.
            // This is to prevent runtime-enabled components from running "OnUpdate" before "OnStart"
            newComponent["started"] = false;
            
            
            // Preform required overrides on component properties
            // Sets component properties to specified values
            const rapidjson::Value& componentProperties = itr->value;
            for (rapidjson::Value::ConstMemberIterator itr2 = componentProperties.MemberBegin(); itr2 != componentProperties.MemberEnd(); itr2++)
            {
                luabridge::LuaRef property = newComponent[itr2->name.GetString()];
                // Only properties in the table can be modified, we should not allow the dev to create new ones this way
                if (!property.isNil())
                {
                    // BOOLS
                    if (property.isBool())
                    {
                        newComponent[itr2->name.GetString()] = itr2->value.GetBool();
                    }
                    // NUMBERS
                    else if (property.isNumber())
                    {
                        // Truncates the value if needed
                        if (std::abs(static_cast<int>(itr2->value.GetDouble()) - itr2->value.GetDouble()) < 1e-10)
                        {
                            newComponent[itr2->name.GetString()] = static_cast<int>(itr2->value.GetDouble());
                        }
                        else
                        {
                            newComponent[itr2->name.GetString()] = itr2->value.GetDouble();
                        }
                    }
                    // STRINGS
                    else if (property.isString())
                    {
                        newComponent[itr2->name.GetString()] = itr2->value.GetString();
                    }
                    // ARRAYS
                    else if (itr2->value.IsArray())
                    {
                        // Loads in a 2D array
                        if (itr2->value[0].IsArray())
                        {
                            std::vector<std::vector<float>> list;
                            
                            // Transfers all of our data to a c++ vector
                            for (int i = 0; i < itr2->value.Size(); i++)
                            {
                                list.push_back({});
                                for (int j = 0; j < itr2->value[i].Size(); j++)
                                {
                                    list[i].push_back((itr2->value[i][j].GetFloat()));
                                }
                            }
                            
                            newComponent[itr2->name.GetString()] = list;
                        }
                        // Loads in 1D array
                        else
                        {
                            std::vector<float> list;
                            
                            // Transfers all of our data to a c++ vector
                            for (int i = 0; i < itr2->value.Size(); i++)
                            {
                                list.push_back((itr2->value[i].GetFloat()));
                            }
                            
                            newComponent[itr2->name.GetString()] = list;
                        }
                    }
                }
            }
            
            componentsToAdd[key] = std::make_shared<luabridge::LuaRef>(newComponent);
        }
    }
}

// Assignment operator
Actor& Actor::operator=(const Actor &copiedActor)
{
    name = copiedActor.name;
    enabled = copiedActor.enabled;
    destroyOnLoad = copiedActor.destroyOnLoad;
    destroyed = copiedActor.destroyed;
    
    // Copies over their components
    for (auto component : copiedActor.componentsToAdd)
    {
        // Creates and gets a reference to a new table on the Lua stack
        lua_newtable(ComponentDB::luaState);
        luabridge::LuaRef newComponent(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
        
        if (ComponentDB::IsComponentTypeCPP((*component.second)["type"]))
        {
            newComponent = ComponentDB::CopyCPPComponent(component.second, (*component.second)["type"]);
        }
        else
        {
            ComponentDB::EstablishInheritance(newComponent, *component.second);
        }
        
        componentsToAdd[newComponent["key"]] = std::make_shared<luabridge::LuaRef>(newComponent);
    }
    
    return *this;
}
