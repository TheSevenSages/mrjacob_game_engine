//
//  Engine.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/2/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "Engine.h"

#include "PhysicsHandler.h"

// The default font to be used when rendering text
string Engine::defaultFontName;

// The default text size to be used
int Engine::defaultTextSize = 16;

// The default text color to be used
SDL_Color Engine::defaultTextColor;

// The next scene to be loaded
string Engine::nextScene;
bool Engine::loadNewScene = false;

// True if the game should be quit out of
bool Engine::quit = false;

// Called to start the game
int Engine::Game()
{
    Initialize();
    
    int loopStatus = 0;
    
    while (loopStatus != 1) {
        loopStatus = GameLoop();
    }
    return 0;
}

// The main loop of the game
int Engine::GameLoop()
{
    if (quit)
    {
        return 1;
    }
    
    SDL_Event inputEvent;
    while(Helper::SDL_PollEvent498(&inputEvent))
    {
        Input::ProcessEvent(inputEvent);
        switch (inputEvent.type) 
        {
            case SDL_QUIT:
                quit = true;
                break;
                
            default:
                break;
        }
    }
    
    GetPlayerInput();
    
    // Clears the frame so new stuff can be drawn on it
    SDL_RenderClear(Renderer::renderer);
        
    SceneDB::currentScene.UpdateActors();
    
    Renderer::Render();
    
    EventBus::ProcessSubEvents();
    
    PhysicsHandler::Step();
    
    DrawGizmos();
    Helper::SDL_RenderPresent498(Renderer::renderer);
    Input::LateUpdate();
    
    // Loads a new scene if specified
    if (SceneDB::loadNewScene)
    {
        SceneDB::LoadNewScene();
    }
    
    return 0;
}

// Initializes variables
void Engine::Initialize()
{    
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    Input::Init();
    ComponentDB::Initialize();
    
    // Confirm the existence of JSON files, and load them if they exist:
    EngineUtils::ConfirmDirectory("resources/", true);
    
    // Game Config
    EngineUtils::ConfirmDirectory("resources/game.config", true);
    EngineUtils::ReadJsonFile("resources/game.config", EngineUtils::game_config);
    
    // Rendering Config
    if (EngineUtils::ConfirmDirectory("resources/rendering.config", false))
    {
        EngineUtils::ReadJsonFile("resources/rendering.config", EngineUtils::rendering_config);
        
        // Display gizmos if configured in rendering.config
        if (EngineUtils::rendering_config.HasMember("display_gizmos"))
        {
            Renderer::show_gizmos = EngineUtils::rendering_config["display_gizmos"].GetBool();
        }
        
        // Assign window bounds their custom values IF they exist, otherwise they stay as the default
        if (EngineUtils::rendering_config.HasMember("x_resolution"))
        {
            Renderer::windowWidth = EngineUtils::rendering_config["x_resolution"].GetInt();
        }
        if (EngineUtils::rendering_config.HasMember("y_resolution"))
        {
            Renderer::windowHeight = EngineUtils::rendering_config["y_resolution"].GetInt();
        }
        
        // Assign camera offset to its custom values IF they exist, otherwise they stay as the default
        if (EngineUtils::rendering_config.HasMember("cam_offset_x"))
        {
            Renderer::cameraOffsetX = EngineUtils::rendering_config["cam_offset_x"].GetFloat();
        }
        if (EngineUtils::rendering_config.HasMember("cam_offset_y"))
        {
            Renderer::cameraOffsetY = EngineUtils::rendering_config["cam_offset_y"].GetFloat();
        }
        
        // Assign camera zoom and ease to their custom values IF they exists, otherwise they say as the default
        if (EngineUtils::rendering_config.HasMember("zoom_factor"))
        {
            Renderer::zoomFactor = EngineUtils::rendering_config["zoom_factor"].GetFloat();
        }
        if (EngineUtils::rendering_config.HasMember("cam_ease_factor"))
        {
            Renderer::easeFactor = EngineUtils::rendering_config["cam_ease_factor"].GetFloat();
        }
        
        // Assign clear colors to their custom values IF they exist, otherwise they stay as the default
        if (EngineUtils::rendering_config.HasMember("clear_color_r"))
        {
            Renderer::clear_r = EngineUtils::rendering_config["clear_color_r"].GetInt();
        }
        if (EngineUtils::rendering_config.HasMember("clear_color_g"))
        {
            Renderer::clear_g = EngineUtils::rendering_config["clear_color_g"].GetInt();
        }
        if (EngineUtils::rendering_config.HasMember("clear_color_b"))
        {
            Renderer::clear_b = EngineUtils::rendering_config["clear_color_b"].GetInt();
        }
    }
    
    // Initialize the renderer class
    Renderer::RenderStart();
    
    // Load Audio
    AudioDB::LoadAudio();
    
    // Load Images
    ImageDB::LoadImages();
    
    // Loads Fonts
    TextDB::LoadFonts();
    
    // Load Templates
    TemplateDB::LoadTemplates();
    
    // Load Scenes
    SceneDB::LoadScenes();
    
    // Loads the starting scene
    if (EngineUtils::game_config.HasMember("initial_scene"))
    {
        SceneDB::currentScene = SceneDB::GetScene(EngineUtils::game_config["initial_scene"].GetString());
        SceneDB::currentScene.Init();
    }
    else
    {
        cout << "error: initial_scene unspecified";
        exit(0);
    }
    
    // Loads the default font
    if (EngineUtils::game_config.HasMember("font"))
    {
        defaultFontName = EngineUtils::game_config["font"].GetString();
    }
    
    defaultTextColor.r = 255;
    defaultTextColor.g = 255;
    defaultTextColor.b = 255;
    defaultTextColor.a = 255;
}

// Renders the Heads Up Display
void Engine::RenderHUD()
{
    // TODO: render HUD
}

// Gets the players input and acts accordingly
void Engine::GetPlayerInput()
{
    if (Input::GetKeyDown("q"))
    {
        quit = true;
    }
}

// Draws debug tools called gizmos
void Engine::DrawGizmos()
{
    if (!Renderer::show_gizmos)
    {
        return;
    }
}
