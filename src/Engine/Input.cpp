//
//  Input.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/15/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "Input.h"

// Call before main loop begins.
void Input::Init()
{
    // All inputs begin in the UP state.
    for (int code = SDL_SCANCODE_UNKNOWN; code < SDL_NUM_SCANCODES; code++)
    {
        keyboard_states[static_cast<SDL_Scancode>(code)] = INPUT_STATE_UP;
    }
    // Mouse buttons
    for (int button = 1; button < 4; button++)
    {
        mouse_button_states[button] = INPUT_STATE_UP;
    }
}

// Call every frame at start of event loop.
void Input::ProcessEvent(const SDL_Event & e)
{
    // Keyboard key processing
    SDL_Scancode key = e.key.keysym.scancode;
    if (e.type == SDL_KEYDOWN)
    {
        keyboard_states[key] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_scancodes.push_back(key);
    }
    else if (e.type == SDL_KEYUP)
    {
        keyboard_states[key] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_scancodes.push_back(key);
    }
    
    // Mouse button processing
    int button = e.button.button;
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        mouse_button_states[button] = INPUT_STATE_JUST_BECAME_DOWN;
        just_became_down_buttons.push_back(button);
    }
    else if (e.type == SDL_MOUSEBUTTONUP)
    {
        mouse_button_states[button] = INPUT_STATE_JUST_BECAME_UP;
        just_became_up_buttons.push_back(button);
    }
    
    // Mouse move events
    if (e.type == SDL_MOUSEMOTION)
    {
        mouse_position.x = e.motion.x;
        mouse_position.y = e.motion.y;
    }
    
    // Scroll wheel events
    if (e.type == SDL_MOUSEWHEEL)
    {
        mouse_scroll_this_frame = e.wheel.preciseY;
    }
}

// Call every frame at the end of the event loop.
void Input::LateUpdate()
{
    // Keyboard key processing
    for (SDL_Scancode code : just_became_down_scancodes)
    {
        keyboard_states[code] = INPUT_STATE_DOWN;
    }
    just_became_down_scancodes.clear();
    
    for (SDL_Scancode code : just_became_up_scancodes)
    {
        keyboard_states[code] = INPUT_STATE_UP;
    }
    just_became_up_scancodes.clear();
    
    // Mouse button processing
    for (int button : just_became_down_buttons)
    {
        mouse_button_states[button] = INPUT_STATE_DOWN;
    }
    just_became_down_buttons.clear();
    
    for (int button : just_became_up_buttons)
    {
        mouse_button_states[button] = INPUT_STATE_UP;
    }
    just_became_up_buttons.clear();
    
    mouse_scroll_this_frame = 0.0f;
}

// Returns true every frame that a key is being held down.
bool Input::GetKey(std::string keycode)
{
    // Returns false if a non-valid keycode is presented
    if (__keycode_to_scancode.find(keycode) == __keycode_to_scancode.end())
    {
        return false;
    }
    
    if (keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_DOWN ||
        keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_JUST_BECAME_DOWN)
    {
        return true;
    }
    
    return false;
}

// Returns true the frame that a key gets pressed down.
bool Input::GetKeyDown(std::string keycode)
{
    // Returns false if a non-valid keycode is presented
    if (__keycode_to_scancode.find(keycode) == __keycode_to_scancode.end())
    {
        return false;
    }
    
    if (keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_JUST_BECAME_DOWN)
    {
        return true;
    }
    
    return false;
}

// Returns true the frame that a key stops getting pressed.
bool Input::GetKeyUp(std::string keycode)
{
    // Returns false if a non-valid keycode is presented
    if (__keycode_to_scancode.find(keycode) == __keycode_to_scancode.end())
    {
        return false;
    }
    
    if (keyboard_states[__keycode_to_scancode.at(keycode)] == INPUT_STATE_JUST_BECAME_UP)
    {
        return true;
    }
    
    return false;
}

// Returns the current mouse position.
glm::vec2 Input::GetMousePosition()
{
    return mouse_position;
}

// Returns true every frame that a mouse button is being held down.
bool Input::GetMouseButton(int button)
{
    if (mouse_button_states[button] == INPUT_STATE_DOWN || mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN)
    {
        return true;
    }
    
    return false;
}

// Returns true the frame that a mouse button gets pressed down.
bool Input::GetMouseButtonDown(int button)
{
    if (mouse_button_states[button] == INPUT_STATE_JUST_BECAME_DOWN)
    {
        return true;
    }
    
    return false;
}

// Returns true the frame that a mouse button stops getting pressed.
bool Input::GetMouseButtonUp(int button)
{
    if (mouse_button_states[button] == INPUT_STATE_JUST_BECAME_UP)
    {
        return true;
    }
    
    return false;
}

// Returns the change in the mouse scroll position between this frame and the last.
float Input::GetMouseScrollDelta()
{
    return mouse_scroll_this_frame;
}
