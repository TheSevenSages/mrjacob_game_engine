//
//  EventBus.cpp
//  game_engine
//
//  Created by Jacob Robinson on 4/4/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "EventBus.h"

std::unordered_map<std::string, std::vector<std::pair<std::shared_ptr<luabridge::LuaRef>, std::shared_ptr<luabridge::LuaRef>>>> EventBus::events;
std::vector<SubEvent> EventBus::subEvents;

void EventBus::Publish(std::string event_type, luabridge::LuaRef event_object)
{
    // Runs every subscribed function
    for (auto& subscriber : events[event_type])
    {
        shared_ptr<luabridge::LuaRef> function = subscriber.second;
        if (function->isFunction())
        {
            (*function)(*subscriber.first, event_object);
        }
    }
}

void EventBus::Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function)
{
    SubEvent s;
    s.subscribe = true;
    s.event_type = event_type;
    s.function = std::make_shared<luabridge::LuaRef>(function);
    s.component = std::make_shared<luabridge::LuaRef>(component);
    
    subEvents.push_back(s);
}

void EventBus::Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function)
{
    SubEvent s;
    s.subscribe = false;
    s.event_type = event_type;
    s.function = std::make_shared<luabridge::LuaRef>(function);
    s.component = std::make_shared<luabridge::LuaRef>(component);
    
    subEvents.push_back(s);
}

void EventBus::ProcessSubEvents()
{
    for (auto& e : subEvents)
    {
        if (e.subscribe)
        {
            events[e.event_type].push_back(std::make_pair(e.component, e.function));
        }
        else
        {
            size_t size = events[e.event_type].size();
            for (int i = 0; i < size; i++)
            {
                if (events[e.event_type][i].second.get()->tostring() == e.function.get()->tostring())
                {
                    events[e.event_type].erase(events[e.event_type].begin() + i);
                    return;
                }
            }
        }
    }
    
    subEvents.clear();
}
