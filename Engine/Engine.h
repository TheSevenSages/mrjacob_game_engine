//
//  engine.h
//  game_engine
//
//  Created by Jacob Robinson on 1/26/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef engine_h
#define engine_h

#include <stdio.h>
#include <iostream>
#include <math.h>

#include "glm/glm.hpp"
#include "lua.hpp"
#include "LuaBridge.h"

#include "EngineUtils.h"
#include "Renderer.h"
#include "Input.h"
#include "Helper.h"
#include "Camera.h"

#include "SceneDB.h"
#include "ImageDB.h"
#include "TemplateDB.h"
#include "TextDB.h"
#include "AudioDB.h"
#include "ComponentDB.h"
#include "EventBus.h"

using namespace std;

class Engine
{
public:
    // Called to start the game
    static int Game();
    
private:
    // The default font to be used when rendering text
    static string defaultFontName;
    
    // The default text size to be used
    static int defaultTextSize;
    
    // The default text color to be used
    static SDL_Color defaultTextColor;
    
    // The next scene to be loaded
    static string nextScene;
    static bool loadNewScene;
    
    // True if the game should be quit out of
    static bool quit;
    
    // The main loop of the game
    static int GameLoop();
    
    // Initializes variables
    static void Initialize();
    
    // Renders the Heads Up Display
    static void RenderHUD();
    
    // Gets the players input and acts accordingly
    static void GetPlayerInput();
    
    // Draws debug tools called gizmos
    static void DrawGizmos();
};

#endif /* engine_h */
