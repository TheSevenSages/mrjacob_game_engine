//
//  ImageDB.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/10/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "ImageDB.h"

// Loads all of the images in the resources/images directory into loadedImages
void ImageDB::LoadImages()
{
    /* Load image files in resources/images */
    const std::string imageDirectoryPath = "resources/images";
    
    // Fills up loadedImages if the path exists
    if (std::filesystem::exists(imageDirectoryPath)) 
    {
        for (const auto& imageFile : std::filesystem::directory_iterator(imageDirectoryPath))
        {
            if (imageFile.path() != imageDirectoryPath + "/.DS_Store")
            {
                SDL_Renderer* r = Renderer::renderer;
                SDL_Texture* img = IMG_LoadTexture(r, imageFile.path().string().c_str());
                loadedImages[imageFile.path().filename().stem().stem().string()] = img;
            }
        }
    }
}

// Get a scene from loadedImages based on the images name
SDL_Texture* ImageDB::GetImage(std::string imageName)
{
    if (loadedImages.find(imageName) == loadedImages.end())
    {
        std::cout << "error: missing image " << imageName;
        exit(0);
    }
    return loadedImages[imageName];
}
