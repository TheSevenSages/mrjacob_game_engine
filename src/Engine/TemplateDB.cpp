//
//  TemplateDB.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/4/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "TemplateDB.h"

// TemplateDB (Data Base) Class:
// Loads all of the templates in the resources/actor_templates directory into loadedTemplates
void TemplateDB::LoadTemplates()
{
    /* Load template files in resources/actor_templates */
    const std::string templateDirectoryPath = "resources/actor_templates";
    
    // Fills up loadedTemplates if the path exists
    if (std::filesystem::exists(templateDirectoryPath)) 
    {
        for (const auto& templateFile : std::filesystem::directory_iterator(templateDirectoryPath))
        {
            if (templateFile.path() != templateDirectoryPath + "/.DS_Store")
            {
                rapidjson::Document templateDocument;
                EngineUtils::ReadJsonFile(templateFile.path().string(), templateDocument);
                
                Actor newTemplate;
                newTemplate.LoadActorWithJSON(templateDocument);
                
                loadedTemplates[templateFile.path().filename().stem().stem().string()] = newTemplate;
            }
        }
    }
}

// Get an actor template from loadedTemplates based on the templates name
Actor TemplateDB::GetTemplate(std::string templateName)
{
    if (loadedTemplates.find(templateName) == loadedTemplates.end())
    {
        std::cout << "error: template " << templateName << " is missing";
        exit(0);
    }
    return loadedTemplates[templateName];
}

