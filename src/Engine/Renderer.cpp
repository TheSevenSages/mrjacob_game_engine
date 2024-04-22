//
//  Renderer.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/9/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "Renderer.h"
#include "TextDB.h"
#include "SceneDB.h"

SDL_Window* Renderer::window;
SDL_Renderer* Renderer::renderer;

int Renderer::windowWidth = 640;
int Renderer::windowHeight = 360;

float Renderer::cameraOffsetX = 0.0f;
float Renderer::cameraOffsetY = 0.0f;

float Renderer::zoomFactor = 1.0f;
float Renderer::easeFactor = 1.0f;

int Renderer::clear_r = 255;
int Renderer::clear_g = 255;
int Renderer::clear_b = 255;

bool Renderer::show_gizmos = false;

// Initializes the window and renderer
void Renderer::RenderStart()
{
    std::string gameTitle = "";
    
    if (EngineUtils::game_config.HasMember("game_title"))
    {
        gameTitle = EngineUtils::game_config["game_title"].GetString();
    }
    
    window = Helper::SDL_CreateWindow498(gameTitle.c_str(), 0, 0, windowWidth, windowHeight, 0);
    renderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC + SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(renderer, clear_r, clear_g, clear_b, 255);
    SDL_RenderClear(renderer);
}

// Adds text to be drawn to the textToDraw vector with the given parameters
void Renderer::DrawText(const std::string& text_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a)
{
    Text newText;
    newText.content = text_content;
    newText.fontName = font_name;
    newText.size = static_cast<int>(font_size);
    
    newText.color.r = static_cast<int>(r);
    newText.color.g = static_cast<int>(g);
    newText.color.b = static_cast<int>(b);
    newText.color.a = static_cast<int>(a);
    
    newText.x = static_cast<int>(x);
    newText.y = static_cast<int>(y);
    
    textToDraw.push(newText);
}

// Draws UI
void Renderer::DrawUI(std::string imageName, float x, float y)
{
    Image newImage;
    newImage.imageName = imageName;
    
    newImage.x = static_cast<int>(x);
    newImage.y = static_cast<int>(y);
    newImage.rotationDegrees = 0;
    newImage.scaleX = 1.0f;
    newImage.scaleY = 1.0f;
    newImage.pivotX = 0.5f;
    newImage.pivotY = 0.5f;
    
    newImage.color.r = 255;
    newImage.color.g = 255;
    newImage.color.b = 255;
    newImage.color.a = 255;
    
    newImage.sortingOrder = 0;
    newImage.requestOrder = static_cast<int>(UIImagesToDraw.size());
    
    UIImagesToDraw.push(newImage);
}

// Draws UI with some extra parameters
void Renderer::DrawUIEx(std::string imageName, float x, float y, float r, float g, float b, float a, float sortingOrder)
{
    Image newImage;
    newImage.imageName = imageName;
    
    newImage.x = static_cast<int>(x);
    newImage.y = static_cast<int>(y);
    newImage.rotationDegrees = 0;
    newImage.scaleX = 1.0f;
    newImage.scaleY = 1.0f;
    newImage.pivotX = 0.5f;
    newImage.pivotY = 0.5f;
    
    newImage.color.r = static_cast<int>(r);
    newImage.color.g = static_cast<int>(g);
    newImage.color.b = static_cast<int>(b);
    newImage.color.a = static_cast<int>(a);
    
    newImage.sortingOrder = static_cast<int>(sortingOrder);
    newImage.requestOrder = static_cast<int>(UIImagesToDraw.size());
    
    UIImagesToDraw.push(newImage);
}

// Draws a scene space image
void Renderer::Draw(std::string imageName, float x, float y)
{
    Image newImage;
    newImage.imageName = imageName;
    
    newImage.x = x;
    newImage.y = y;
    newImage.rotationDegrees = 0;
    newImage.scaleX = 1.0f;
    newImage.scaleY = 1.0f;
    newImage.pivotX = 0.5f;
    newImage.pivotY = 0.5f;
    
    newImage.color.r = 255;
    newImage.color.g = 255;
    newImage.color.b = 255;
    newImage.color.a = 255;
    
    newImage.sortingOrder = 0;
    newImage.requestOrder = static_cast<int>(sceneImagesToDraw.size());
    
    sceneImagesToDraw.push(newImage);
}

// Draws a scene space image with some extra parameters
void Renderer::DrawEx(std::string imageName, float x, float y, float rotationDegrees, float scaleX, float scaleY, float pivotX, float pivotY, float r, float g, float b, float a, float sortingOrder)
{
    Image newImage;
    newImage.imageName = imageName;
    
    newImage.x = x;
    newImage.y = y;
    newImage.rotationDegrees = static_cast<int>(rotationDegrees);
    newImage.scaleX = scaleX;
    newImage.scaleY = scaleY;
    newImage.pivotX = pivotX;
    newImage.pivotY = pivotY;
    
    newImage.color.r = static_cast<int>(r);
    newImage.color.g = static_cast<int>(g);
    newImage.color.b = static_cast<int>(b);
    newImage.color.a = static_cast<int>(a);
    
    newImage.sortingOrder = static_cast<int>(sortingOrder);
    newImage.requestOrder = static_cast<int>(sceneImagesToDraw.size());
    
    sceneImagesToDraw.push(newImage);
}

// Draws a pixel on the screen
void Renderer::DrawPixel(float x, float y, float r, float g, float b, float a)
{
    Pixel newPixel;
    
    newPixel.x = static_cast<int>(x);
    newPixel.y = static_cast<int>(y);
    
    newPixel.color.r = static_cast<int>(r);
    newPixel.color.g = static_cast<int>(g);
    newPixel.color.b = static_cast<int>(b);
    newPixel.color.a = static_cast<int>(a);
    
    pixelsToDraw.push(newPixel);
}

// Renders all of the needed text and images in proper order
void Renderer::Render()
{
    RenderSceneSpaceImages();
    RenderUIImages();
    RenderText();
    RenderPixels();
}

// Draws all the text in the textToDraw vector to the window
void Renderer::RenderText()
{
    while (!textToDraw.empty())
    {
        Text t = textToDraw.front();
        textToDraw.pop();
        
        SDL_Rect rect;
        rect.x = t.x;
        rect.y = t.y;
        rect.w = windowWidth;
        rect.h = windowHeight;
        
        TTF_SizeText(TextDB::GetFont(t.fontName, t.size), t.content.c_str(), &rect.w, &rect.h);
        
        SDL_Surface* surface = TTF_RenderText_Solid(TextDB::GetFont(t.fontName, t.size), t.content.c_str(), t.color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_FreeSurface(surface);
    }
}

// Draws all the images in the sceneImagesToDraw queue to the window
void Renderer::RenderSceneSpaceImages()
{
    SDL_RenderSetScale(renderer, SceneDB::currentScene.camera.zoom, SceneDB::currentScene.camera.zoom);
    while (!sceneImagesToDraw.empty())
    {
        Image i = sceneImagesToDraw.top();
        sceneImagesToDraw.pop();
        
        float posX = 0.0f;
        float posY = 0.0f;
        
        SDL_Rect rect;
        SDL_Point center;
        // Gets the width of the image to render
        SDL_QueryTexture(ImageDB::GetImage(i.imageName), NULL, NULL, &rect.w, &rect.h);
        
        // Apply scale
        int flip = SDL_FLIP_NONE;
        if (i.scaleX < 0) {flip |= SDL_FLIP_HORIZONTAL;}
        if (i.scaleY < 0) {flip |= SDL_FLIP_VERTICAL;}
            
        rect.w *= std::abs((i.scaleX));
        rect.h *= std::abs((i.scaleY));
        
        center.x = static_cast<int>(rect.w * i.pivotX);
        center.y = static_cast<int>(rect.h * i.pivotY);
        
        // Baseline position relative to the camera
        posX = i.x - SceneDB::currentScene.camera.position.x;
        posY = i.y - SceneDB::currentScene.camera.position.y;
        
        // Offset by camera offset values
        posX = (posX - SceneDB::currentScene.camera.offsetX) * PIXELS_PER_UNIT;
        posY = (posY - SceneDB::currentScene.camera.offsetY) * PIXELS_PER_UNIT;
        
        // Centers the actors position on the center of the camera
        posX += SceneDB::currentScene.camera.cameraWidth * 0.5f * (1.0f / SceneDB::currentScene.camera.zoom);
        posY += SceneDB::currentScene.camera.cameraHeight * 0.5f * (1.0f / SceneDB::currentScene.camera.zoom);
        
        // Offsets the render position by the pivot ammount
        posX -= center.x;
        posY -= center.y;
    
        // Cast the position to ints so they can represent pixels on the screen
        rect.x = static_cast<int>(posX);
        rect.y = static_cast<int>(posY);
        
        //if (!IsImageInCamera(rect.x, rect.y, rect.w, rect.h)) {continue;}
        
        // Sets the correct color and alpha to the texture
        SDL_SetTextureColorMod(ImageDB::GetImage(i.imageName), i.color.r, i.color.g, i.color.b);
        SDL_SetTextureAlphaMod(ImageDB::GetImage(i.imageName), i.color.a);
        
        Helper::SDL_RenderCopyEx498(-1, "dummy", renderer, ImageDB::GetImage(i.imageName), NULL, &rect, static_cast<int>(i.rotationDegrees), &center, (SDL_RendererFlip)flip);

        // Removes the color and alpha from the texture
        SDL_SetTextureColorMod(ImageDB::GetImage(i.imageName), 255, 255, 255);
        SDL_SetTextureAlphaMod(ImageDB::GetImage(i.imageName), 255);
    }
    SDL_RenderSetScale(renderer, 1, 1);
}

// Draws all the images in the UIImagesToDraw queue to the window
void Renderer::RenderUIImages()
{
    while (!UIImagesToDraw.empty())
    {
        Image i = UIImagesToDraw.top();
        UIImagesToDraw.pop();
        
        SDL_Point center;
        SDL_Rect rect;
        // Gets the width of the image to render
        SDL_QueryTexture(ImageDB::GetImage(i.imageName), NULL, NULL, &rect.w, &rect.h);
        
        // Apply scale
        int flip = SDL_FLIP_NONE;
        if (i.scaleX < 0) {flip |= SDL_FLIP_HORIZONTAL;}
        if (i.scaleY < 0) {flip |= SDL_FLIP_VERTICAL;}
        
        rect.w *= std::abs(static_cast<int>(i.scaleX));
        rect.h *= std::abs(static_cast<int>(i.scaleY));
        
        // Calculate the center point of the image
        center.x = static_cast<int>(rect.w * i.pivotX);
        center.y = static_cast<int>(rect.h * i.pivotY);
        
        // The screen position in pixels to print the Ui image at
        rect.x = static_cast<int>(i.x);
        rect.y = static_cast<int>(i.y);
        
        // Sets the correct color and alpha to the texture
        SDL_SetTextureColorMod(ImageDB::GetImage(i.imageName), i.color.r, i.color.g, i.color.b);
        SDL_SetTextureAlphaMod(ImageDB::GetImage(i.imageName), i.color.a);
        
        Helper::SDL_RenderCopyEx498(-1, "dummy", renderer, ImageDB::GetImage(i.imageName), NULL, &rect, static_cast<int>(i.rotationDegrees), &center, (SDL_RendererFlip)flip);
        
        // Removes the color and alpha from the texture
        SDL_SetTextureColorMod(ImageDB::GetImage(i.imageName), 255, 255, 255);
        SDL_SetTextureAlphaMod(ImageDB::GetImage(i.imageName), 255);
    }
}

// Draws all the pixels in the pixelsToDraw queue to the window
void Renderer::RenderPixels()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    while (!pixelsToDraw.empty())
    {
        Pixel p = pixelsToDraw.front();
        pixelsToDraw.pop();
        
        SDL_SetRenderDrawColor(renderer, p.color.r, p.color.g, p.color.b, p.color.a);
        SDL_RenderDrawPoint(renderer, p.x, p.y);
        SDL_SetRenderDrawColor(renderer, clear_r, clear_g, clear_b, 255);
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// Returns true if the given rect is viewable by the camera
bool Renderer::IsImageInCamera(float x, float y, float w, float h)
{
    bool withinWidth = false;
    bool withinHeight = false;
       
    // Check if image is within the camera width
    if (x + (w / 2) >= 0 && x - (w / 2) <= windowWidth)
    {
       withinWidth = true;
    }
    // Check if image is within the camera height
    if (y + (h / 2) >= 0 && y - (h / 2) <= windowHeight)
    {
       withinHeight = true;
    }

    if (withinWidth && withinHeight)
    {
       return true;
    }
    
    return false;
}
