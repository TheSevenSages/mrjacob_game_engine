//
//  TextDB.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/10/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "TextDB.h"

// Loads the fonts to be used for text rendering into loadedFonts
void TextDB::LoadFonts()
{
    /* Load font files in resources/fonts */
    const std::string fontDirectoryPath = "resources/fonts";
    
    // Fills up loadedFonts if the path exists
    if (std::filesystem::exists(fontDirectoryPath)) 
    {
        for (const auto& fontFile : std::filesystem::directory_iterator(fontDirectoryPath))
        {
            if (fontFile.path() != fontDirectoryPath + "/.DS_Store")
            {
                loadedFonts[fontFile.path().filename().stem().stem().string()][16] = TTF_OpenFont(fontFile.path().string().c_str(), 16);
            }
        }
    }
}

// Get a scene from loadedFonts based on the fonts name
TTF_Font* TextDB::GetFont(std::string fontName, int fontSize)
{
    // Throw error if the font does not exist
    if (loadedFonts.find(fontName) == loadedFonts.end())
    {
        std::cout << "error: font " << fontName << " missing";
        exit(0);
    }
    
    // Loads font at fontSize if it does not already exist
    if (loadedFonts[fontName].find(fontSize) == loadedFonts[fontName].end())
    {
        /* Load font file from resources/fonts */
        const std::string fontPath = "resources/fonts/" + fontName + ".ttf";
        // Open the font at the desired size, if not done so already
        loadedFonts[fontName][fontSize] = TTF_OpenFont(fontPath.c_str(), fontSize);
    }
    
    return loadedFonts[fontName][fontSize];
}
