//
//  TemplateDB.h
//  game_engine
//
//  Created by Jacob Robinson on 2/4/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef TemplateDB_h
#define TemplateDB_h

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <filesystem>
#include <cstdlib>
#include <unordered_map>
#include <optional>

#include "SDL2_image/SDL_image.h"
#include "EngineUtils.h"
#include "ImageDB.h"
#include "Actor.h"
#include "glm/glm.hpp" // Student : You need to get glm added to your project source code or this line will fail.

class TemplateDB
{
public:
    // Loads all of the templates in the resources/actor_templates directory into loadedTemplates
    static void LoadTemplates();

    // Get an actor template from loadedTemplates based on the templates name
    static Actor GetTemplate(std::string templateName);
    
private:
    static inline std::unordered_map<std::string, Actor> loadedTemplates;
};

#endif /* TemplateDB_h */
