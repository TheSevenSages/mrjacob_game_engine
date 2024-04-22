//
//  EventBus.h
//  game_engine
//
//  Created by Jacob Robinson on 4/4/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef EventBus_h
#define EventBus_h

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "lua.hpp"
#include "LuaBridge.h"

using namespace std;

struct SubEvent
{
    bool subscribe = true;
    std::string event_type;
    std::shared_ptr<luabridge::LuaRef> function;
    std::shared_ptr<luabridge::LuaRef> component;
};

class EventBus
{
public:
    static std::unordered_map<std::string, std::vector<std::pair<std::shared_ptr<luabridge::LuaRef>, std::shared_ptr<luabridge::LuaRef>>>> events;
    static std::vector<SubEvent> subEvents;
    
    static void Publish(std::string event_type, luabridge::LuaRef event_object);
    static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
    static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
    
    static void ProcessSubEvents();
};

#endif /* EventBus_h */
