//
//  TextDB.h
//  game_engine
//
//  Created by Jacob Robinson on 2/10/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef TextDB_h
#define TextDB_h

#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <filesystem>
#include <cstdlib>
#include <unordered_map>
#include <map>

#include "EngineUtils.h"
#include "Renderer.h"
#include "SDL2_image/SDL_image.h"
#include "SDL2/SDL.h"
#include "SDL2_ttf/SDL_ttf.h"

class TextDB
{
public:
    // Loads the fonts to be used for text rendering into loadedFonts
    static void LoadFonts();
    
    // Get a scene from loadedFonts based on the fonts name
    static TTF_Font* GetFont(std::string fontName, int fontSize);
    
private:    
    static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> loadedFonts;
};

#endif /* TextDB_h */
