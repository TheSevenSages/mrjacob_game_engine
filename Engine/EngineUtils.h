//
//  EngineUtils.h
//  game_engine
//
//  Created by Jacob Robinson on 2/1/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef EngineUtils_h
#define EngineUtils_h

#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <filesystem>
#include <cstdlib>

#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

class EngineUtils {
public:
    static rapidjson::Document game_config;
    static rapidjson::Document rendering_config;
    
    // Checks to make sure a directory exists, and throws an error if it doesn't and is maked as required.
    static bool ConfirmDirectory(std::string path, bool required);
    
    // Reads a JSON file into a Document
    static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document);
};

#endif /* EngineUtils_h */
