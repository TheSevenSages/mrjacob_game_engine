//
//  Renderer.h
//  game_engine
//
//  Created by Jacob Robinson on 2/9/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef Renderer_h
#define Renderer_h

#include <queue>

#include "SDL2_image/SDL_image.h"
#include "SDL2/SDL.h"
#include "Helper.h"
#include "ImageDB.h"
#include "EngineUtils.h"

struct Text
{
    std::string content;
    std::string fontName;
    
    int x;
    int y;
    
    int size;
    SDL_Color color;
};

struct Image
{
    std::string imageName;
    
    float x;
    float y;
    int rotationDegrees;
    float scaleX;
    float scaleY;
    float pivotX;
    float pivotY;
    
    SDL_Color color;
    
    int sortingOrder;
    int requestOrder;
};

struct Pixel
{
    int x;
    int y;
    
    SDL_Color color;
};

struct ImageOrderComparator
{
    // Comparator function
    bool operator()(Image A, Image B)
    {
        if (A.sortingOrder == B.sortingOrder) 
        {
            return A.requestOrder > B.requestOrder;
        }
        
        return A.sortingOrder > B.sortingOrder;
    }
};

class Renderer
{
public:
    // How many pixels we're using to render one cell
    static const int PIXELS_PER_UNIT = 100;
    
    static SDL_Window* window;
    static SDL_Renderer* renderer;
    
    static int windowWidth;
    static int windowHeight;
    
    static float cameraOffsetX;
    static float cameraOffsetY;
    
    static float zoomFactor;
    static float easeFactor;
    
    static int clear_r;
    static int clear_g;
    static int clear_b;
    
    // Show Gizmos (Debug tools such as viewing box collders)
    static bool show_gizmos;
    
    // Initializes the window and renderer
    static void RenderStart();
    
    // Adds text to be drawn to the textToDraw vector with the given parameters
    static void DrawText(const std::string& text_content, float x, float y, std::string font_name, float font_size, float r, float g, float b, float a);
    
    // Draws UI
    static void DrawUI(std::string imageName, float x, float y);
    
    // Draws UI with some extra parameters
    static void DrawUIEx(std::string imageName, float x, float y, float r, float g, float b, float a, float sortingOrder);
    
    // Draws a scene space image
    static void Draw(std::string imageName, float x, float y);
    
    // Draws a scene space image with some extra parameters
    static void DrawEx(std::string imageName, float x, float y, float rotationDegrees, float scaleX, float scaleY, float pivotX, float pivotY, float r, float g, float b, float a, float sortingOrder);
    
    // Draws a pixel on the screen
    static void DrawPixel(float x, float y, float r, float g, float b, float a);
    
    // Renders all of the needed text and images in proper order
    static void Render();
private:
    static inline std::priority_queue<Image, std::vector<Image>, ImageOrderComparator> sceneImagesToDraw;
    static inline std::priority_queue<Image, std::vector<Image>, ImageOrderComparator> UIImagesToDraw;
    static inline std::queue<Text> textToDraw;
    static inline std::queue<Pixel> pixelsToDraw;
    
    // Draws all the text in the textToDraw queue to the window
    static void RenderText();
    
    // Draws all the images in the sceneImagesToDraw queue to the window
    static void RenderSceneSpaceImages();
    
    // Draws all the images in the UIImagesToDraw queue to the window
    static void RenderUIImages();
    
    // Draws all the pixels in the pixelsToDraw queue to the window
    static void RenderPixels();
    
    // Returns true if the given rect is viewable by the camera
    static bool IsImageInCamera(float x, float y, float w, float h);
};

#endif /* Renderer_h */
