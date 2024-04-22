//
//  ComponentDB.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/29/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "ComponentDB.h"
#include "SceneDB.h"
#include "Actor.h"
#include "Application.h"
#include "Input.h"
#include "Renderer.h"
#include "AudioDB.h"
#include "EventBus.h"
#include "ParticleSystem.h"

// Initializes variables
void ComponentDB::Initialize()
{
    InitializeState();
    InitializeFunctions();
    LoadComponents();
}

// Initializes the luaState
void ComponentDB::InitializeState()
{
    luaState = luaL_newstate();
    luaL_openlibs(luaState);
}

// Initializes C functions that devs can call in Lua
void ComponentDB::InitializeFunctions()
{
    /* Inject the scripting API */
    /* Debug static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Debug")
        .addFunction("Log", ComponentDB::Log)
        .addFunction("LogError", ComponentDB::LogError)
        .endNamespace();
    
    /* Application static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Application")
        .addFunction("GetFrame", Application::GetFrame)
        .addFunction("Quit", Application::Quit)
        .addFunction("Sleep", Application::Sleep)
        .addFunction("OpenURL", Application::OpenURL)
        .addFunction("RandomNumber", Application::RandomNumber)
        .endNamespace();
    
    /* Actor class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::GetName)
        .addFunction("GetID", &Actor::GetID)
        .addFunction("GetComponentByKey", &Actor::GetComponentByKey)
        .addFunction("GetComponent", &Actor::GetComponent)
        .addFunction("GetComponents", &Actor::GetComponents)
        .addFunction("AddComponent", &Actor::AddComponent)
        .addFunction("RemoveComponent", &Actor::RemoveComponent)
        .endClass();
    
    /* Actor static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Actor")
        .addFunction("Find", SceneDB::FindActorWithName)
        .addFunction("FindAll", SceneDB::FindAllActorsWithName)
        .addFunction("Instantiate", SceneDB::Instantiate)
        .addFunction("Destroy", SceneDB::Destroy)
        .endNamespace();
    
    /* vec2 class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<glm::vec2>("vec2")
        .addProperty("x", &glm::vec2::x)
        .addProperty("y", &glm::vec2::y)
        .endClass();
    
    // Exposes c++ vector to Lua
    luabridge::getGlobalNamespace(luaState)
        .beginClass<std::vector<float>>("FVector")
        .endClass();
    
    // Exposes 2D c++ vector to Lua
    luabridge::getGlobalNamespace(luaState)
        .beginClass<std::vector<std::vector<float>>>("FVector2D")
        .endClass();
    
    /* Input static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Input")
        .addFunction("GetKey", Input::GetKey)
        .addFunction("GetKeyDown", Input::GetKeyDown)
        .addFunction("GetKeyUp", Input::GetKeyUp)
        .addFunction("GetMousePosition", Input::GetMousePosition)
        .addFunction("GetMouseButton", Input::GetMouseButton)
        .addFunction("GetMouseButtonDown", Input::GetMouseButtonDown)
        .addFunction("GetMouseButtonUp", Input::GetMouseButtonUp)
        .addFunction("GetMouseScrollDelta", Input::GetMouseScrollDelta)
        .endNamespace();
    
    /* Text static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Text")
        .addFunction("Draw", Renderer::DrawText)
        .endNamespace();
    
    /* Audio static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Audio")
        .addFunction("Play", AudioDB::PlayAudio)
        .addFunction("Halt", AudioDB::HaltAudio)
        .addFunction("SetVolume", AudioDB::SetVolume)
        .endNamespace();
    
    /* Image static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Image")
        .addFunction("DrawUI", Renderer::DrawUI)
        .addFunction("DrawUIEx", Renderer::DrawUIEx)
        .addFunction("Draw", Renderer::Draw)
        .addFunction("DrawEx", Renderer::DrawEx)
        .addFunction("DrawPixel", Renderer::DrawPixel)
        .endNamespace();
    
    /* Camera static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Camera")
        .addFunction("SetPosition", SceneDB::CameraSetPosition)
        .addFunction("GetPositionX", SceneDB::CameraGetPositionX)
        .addFunction("GetPositionY", SceneDB::CameraGetPositionY)
        .addFunction("SetZoom", SceneDB::SetZoom)
        .addFunction("GetZoom", SceneDB::GetZoom)
        .endNamespace();
    
    /* Scene static class (namespace) */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Scene")
        .addFunction("Load", SceneDB::ChangeScene)
        .addFunction("GetCurrent", SceneDB::GetCurrentSceneName)
        .addFunction("DontDestroy", SceneDB::DontDestroy)
        .endNamespace();
    
    /* Vector2 class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void(*) (float, float)>()
        .addProperty("x", &b2Vec2::x)
        .addProperty("y", &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
        .addFunction("__sub", &b2Vec2::operator_sub)
        .addFunction("__mul", &b2Vec2::operator_mul)
        .addStaticFunction("Distance", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Distance))
        .addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endClass();

    /* Collision class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Collision>("collision")
        .addData("other", &Collision::other)
        .addData("point", &Collision::point)
        .addData("relative_velocity", &Collision::relative_velocity)
        .addData("normal", &Collision::normal)
        .endClass();

    /* HitResult class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<HitResult>("HitResult")
        .addData("actor", &HitResult::actor)
        .addData("point", &HitResult::point)
        .addData("is_trigger", &HitResult::is_trigger)
        .addData("normal", &HitResult::normal)
        .endClass();
    
    /* ParticleSystem class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<ParticleSystem>("ParticleSystem")
        .addData("MAX_NUM_PARTICLES", &ParticleSystem::MAX_NUM_PARTICLES)
        .addData("type", &ParticleSystem::type)
        .addData("key", &ParticleSystem::key)
        .addData("actor", &ParticleSystem::actor)
        .addData("enabled", &ParticleSystem::enabled)
        .addData("started", &ParticleSystem::started)
        .addData("emitting", &ParticleSystem::emitting)
        .addData("duration", &ParticleSystem::duration)
        .addData("loop", &ParticleSystem::loop)
        .addData("emission_rate", &ParticleSystem::emission_rate)
        .addData("particle_lifetime", &ParticleSystem::particle_lifetime)
        .addData("emitter_pos_x", &ParticleSystem::emitter_pos_x)
        .addData("emitter_pos_y", &ParticleSystem::emitter_pos_y)
        .addData("emission_range_x", &ParticleSystem::emission_range_x)
        .addData("emission_range_y", &ParticleSystem::emission_range_y)
        .addData("emission_direction", &ParticleSystem::emission_direction)
        .addData("emission_direction_range", &ParticleSystem::emission_direction_range)
        .addData("change_movement", &ParticleSystem::change_movement)
        .addData("movement_pattern", &ParticleSystem::movement_pattern)
        .addData("speed", &ParticleSystem::speed)
        .addData("speed_sine_loop", &ParticleSystem::speed_sine_loop)
        .addData("change_size", &ParticleSystem::change_size)
        .addData("size_pattern", &ParticleSystem::size_pattern)
        .addData("size_change_per_second", &ParticleSystem::size_change_per_second)
        .addData("size_sine_loop", &ParticleSystem::size_sine_loop)
        .addData("speed_sine_amplitude", &ParticleSystem::speed_sine_amplitude)
        .addData("image", &ParticleSystem::image)
        .addData("colors", &ParticleSystem::colors)
        .addData("change_color", &ParticleSystem::change_color)
        .addData("sorting_order", &ParticleSystem::sorting_order)
        .addData("x", &ParticleSystem::x)
        .addData("y", &ParticleSystem::y)
        .addData("body_type", &ParticleSystem::body_type)
        .addData("precise", &ParticleSystem::precise)
        .addData("gravity_scale", &ParticleSystem::gravity_scale)
        .addData("density", &ParticleSystem::density)
        .addData("angular_friction", &ParticleSystem::angular_friction)
        .addData("rotation", &ParticleSystem::rotation)
        .addData("mass", &ParticleSystem::mass)
        .addData("has_collider", &ParticleSystem::has_collider)        
        .addData("collide_with_other_particles", &ParticleSystem::collide_with_other_particles)
        .addData("collider_type", &ParticleSystem::collider_type)
        .addData("starting_size", &ParticleSystem::starting_size)
        .addData("friction", &ParticleSystem::friction)
        .addData("bounciness", &ParticleSystem::bounciness)
        .addFunction("StartEmitting", &ParticleSystem::StartEmitting)
        .addFunction("StopEmitting", &ParticleSystem::StopEmitting)
        .addFunction("OnStart", &ParticleSystem::OnStart)
        .addFunction("OnUpdate", &ParticleSystem::OnUpdate)
        .addFunction("OnDestroy", &ParticleSystem::OnDestroy)
        .endClass();
    
    /* Rigidbody class */
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Rigidbody>("Rigidbody")
        .addData("enabled", &Rigidbody::enabled)
        .addData("started", &Rigidbody::started)
        .addData("key", &Rigidbody::key)
        .addData("type", &Rigidbody::type)
        .addData("actor", &Rigidbody::actor)
        .addData("x", &Rigidbody::x)
        .addData("y", &Rigidbody::y)
        .addData("body_type", &Rigidbody::body_type)
        .addData("precise", &Rigidbody::precise)
        .addData("gravity_scale", &Rigidbody::gravity_scale)
        .addData("density", &Rigidbody::density)
        .addData("angular_friction", &Rigidbody::angular_friction)
        .addData("rotation", &Rigidbody::rotation)
    
        // Collider
        .addData("has_collider", &Rigidbody::has_collider)
        .addData("width", &Rigidbody::width)
        .addData("height", &Rigidbody::height)
        .addData("radius", &Rigidbody::radius)
        .addData("friction", &Rigidbody::friction)
        .addData("bounciness", &Rigidbody::bounciness)
        .addData("collider_type", &Rigidbody::collider_type)
    
        // Trigger
        .addData("has_trigger", &Rigidbody::has_trigger)
        .addData("trigger_width", &Rigidbody::trigger_width)
        .addData("trigger_height", &Rigidbody::trigger_height)
        .addData("trigger_radius", &Rigidbody::trigger_radius)
        .addData("trigger_type", &Rigidbody::trigger_type)

        // Force
        .addFunction("AddForce", &Rigidbody::AddForce)
    
        // Setters
        .addFunction("SetVelocity", &Rigidbody::SetVelocity)
        .addFunction("SetPosition", &Rigidbody::SetPosition)
        .addFunction("SetRotation", &Rigidbody::SetRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)

        // Getters
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("GetVelocity", &Rigidbody::GetVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)

        .addFunction("OnStart", &Rigidbody::OnStart)
        .addFunction("OnDestroy", &Rigidbody::OnDestroy)
        .endClass();
    
    /* Physics static Lua class */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Physics")
        .addFunction("Raycast", RayCaster::Raycast)
        .addFunction("RaycastAll", RayCaster::RaycastAll)
        .endNamespace();
    
    /* Event static Lua class */
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Event")
        .addFunction("Publish", EventBus::Publish)
        .addFunction("Subscribe", EventBus::Subscribe)
        .addFunction("Unsubscribe", EventBus::Unsubscribe)
        .endNamespace();
}

// Loads all of the components in the resources/component_types directory into componentTables
void ComponentDB::LoadComponents()
{
    /* Load component files in resources/component_types */
    const std::string componentDirectoryPath = "resources/component_types";
    
    // Fills up componentTables if the path exists
    if (std::filesystem::exists(componentDirectoryPath))
    {
        for (const auto& componentFile : std::filesystem::directory_iterator(componentDirectoryPath))
        {
            if (componentFile.path() != componentDirectoryPath + "/.DS_Store")
            {
                std::string componentName = componentFile.path().stem().string();
                
                if (luaL_dofile(luaState, componentFile.path().string().c_str()) != LUA_OK)
                {
                    std::cout << "problem with lua file " << componentName;
                    exit(0);
                }
                
                componentTables.insert({
                    componentName,
                    std::make_shared<luabridge::LuaRef>(luabridge::getGlobal(luaState, componentName.c_str()))
                });
            }
        }
    }
}

// Returns true if the given type is a C++ based component
bool ComponentDB::IsComponentTypeCPP(std::string type)
{
    if (type == "Rigidbody" || type == "ParticleSystem") {return true;}
    
    return false;
}

// Creates a C++ component and returns the LuaRef
luabridge::LuaRef ComponentDB::NewCPPComponent(std::string type)
{
    if (type == "Rigidbody")
    {
        Rigidbody* rigidbody = new Rigidbody();
        luabridge::LuaRef newComponent(luaState, rigidbody);
        return newComponent;
    }
    
    if (type == "ParticleSystem")
    {
        ParticleSystem* particleSystem = new ParticleSystem();
        luabridge::LuaRef newComponent(luaState, particleSystem);
        return newComponent;
    }
    
    return luabridge::LuaRef(ComponentDB::luaState);
}

// Makes a new copy of the given CPP component and returns it
luabridge::LuaRef ComponentDB::CopyCPPComponent(shared_ptr<luabridge::LuaRef> component, std::string type)
{
    // Creates and gets a reference to a new table on the Lua stack
    lua_newtable(ComponentDB::luaState);
    luabridge::LuaRef newComponent(luabridge::get<luabridge::LuaRef>(ComponentDB::luaState, -1));
    
    if (ComponentDB::IsComponentTypeCPP((*component)["type"]))
    {
        if ((*component)["type"].tostring() == "Rigidbody")
        {
            Rigidbody* r = new Rigidbody((*component).cast<Rigidbody>());
            newComponent = luabridge::LuaRef(ComponentDB::luaState, r);
        }
        else if ((*component)["type"].tostring() == "ParticleSystem")
        {
            ParticleSystem* p = new ParticleSystem((*component).cast<ParticleSystem>());
            newComponent = luabridge::LuaRef(ComponentDB::luaState, p);
        }
    }
    
    return newComponent;
}

// Get a component from componentTables based on the components name
std::shared_ptr<luabridge::LuaRef> ComponentDB::GetComponent(std::string componentName)
{
    if (componentTables.find(componentName) == componentTables.end())
    {
        std::cout << "error: failed to locate component " << componentName;
        exit(0);
    }
    return componentTables[componentName];
}

// Establishes inheritance between two tables by setting one to be the metatable of another
void ComponentDB::EstablishInheritance(luabridge::LuaRef & instance_table, luabridge::LuaRef & parent_table)
{
    /* We must create a metatable to establish inheritance in Lua */
    luabridge::LuaRef new_metatable = luabridge::newTable(luaState);
    new_metatable["__index"] = parent_table;
    
    /* We must use the raw lua C-API (lua stack) to preform a "setmetatable" operation */
    instance_table.push(luaState);
    new_metatable.push(luaState);
    lua_setmetatable(luaState, -2);
    lua_pop(luaState, 1);
}

// Prints a debug message to cout
void ComponentDB::Log(std::string message)
{
    std::cout << message << std::endl;
}

// Prints an error message to cerr
void ComponentDB::LogError(std::string message)
{
    std::cerr << message << std::endl;
}
