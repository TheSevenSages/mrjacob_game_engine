//
//  EngineUtils.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/2/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "EngineUtils.h"

rapidjson::Document EngineUtils::game_config;
rapidjson::Document EngineUtils::rendering_config;

// Checks to make sure a directory exists, and throws an error if it doesn't and is maked as required.
bool EngineUtils::ConfirmDirectory(std::string path, bool required)
{
    if (!std::filesystem::exists(std::filesystem::path(path)))
    {
        if (required)
        {
            std::cout << "error: " << path << " missing";
            exit(0);
        }
        return false;
    }
    
    return true;
}

// Reads a JSON file into a Document
void EngineUtils::ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
{
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    std::fclose(file_pointer);
    
    if (out_document.HasParseError())
    {
        rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
        std::cout << "error parsing json at [" << path << "]" << std::endl;
        exit(0);
    }
}
