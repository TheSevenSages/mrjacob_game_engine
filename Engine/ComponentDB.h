//
//  ComponentDB.h
//  game_engine
//
//  Created by Jacob Robinson on 2/29/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef ComponentDB_h
#define ComponentDB_h

#include <filesystem>
#include <memory>

#include "lua.hpp"
#include "LuaBridge.h"

#include "PhysicsHandler.h"

class Actor;

class ComponentDB
{
public:
    static inline lua_State* luaState;
    
    // Initializes variables
    static void Initialize();
    
    // Initializes the luaState
    static void InitializeState();
    
    // Initializes C functions that devs can call in Lua
    static void InitializeFunctions();
    
    // Loads all of the components in the resources/component_types directory into componentTables
    static void LoadComponents();
    
    // Returns true if the given type is a C++ based component
    static bool IsComponentTypeCPP(std::string type);
    
    // Creates a C++ component and returns the LuaRef
    static luabridge::LuaRef NewCPPComponent(std::string type);
    
    // Makes a new copy of the given CPP component and returns it
    static luabridge::LuaRef CopyCPPComponent(shared_ptr<luabridge::LuaRef> component, std::string type);

    // Get a component from componentTables based on the components name
    static std::shared_ptr<luabridge::LuaRef> GetComponent(std::string componentName);
    
    // Establishes inheritance between two tables by setting one to be the metatable of another
    static void EstablishInheritance(luabridge::LuaRef & instance_table, luabridge::LuaRef & parent_table);
    
private:
    static inline std::unordered_map<std::string, std::shared_ptr<luabridge::LuaRef>> componentTables;
    
    // Prints a debug message to cout
    static void Log(std::string message);
    
    // Prints an error message to cerr
    static void LogError(std::string message);
};

#endif /* ComponentDB_h */
