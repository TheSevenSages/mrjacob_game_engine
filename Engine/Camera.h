//
//  Camera.h
//  game_engine
//
//  Created by Jacob Robinson on 2/10/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef Camera_h
#define Camera_h

#include <vector>

#include "glm/glm.hpp"

class Camera
{
public:
    // Camera bounds
    int cameraWidth = 13;
    int cameraHeight = 9;
    
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    
    float zoom = 1.0f;
    float ease = 1.0f;
    
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
};


#endif /* Camera_h */
