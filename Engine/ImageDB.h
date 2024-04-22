//
//  ImageDB.h
//  game_engine
//
//  Created by Jacob Robinson on 2/10/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef ImageDB_h
#define ImageDB_h

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


class ImageDB
{
public:
    // Loads all of the images in the resources/images directory into loadedImages
    static void LoadImages();
    
    // Get an image from loadedImages based on the images name
    static SDL_Texture* GetImage(std::string imageName);
    
private:
    static inline std::unordered_map<std::string, SDL_Texture*> loadedImages;
};

#endif /* ImageDB_h */
