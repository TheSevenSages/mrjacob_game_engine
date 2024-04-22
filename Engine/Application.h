//
//  Application.h
//  game_engine
//
//  Created by Jacob Robinson on 3/7/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef Application_h
#define Application_h

#include <thread>
#include <random>

#include "Helper.h"

class Application
{
public:
    // Gets the current frame
    static int GetFrame()
    {
        return Helper::GetFrameNumber();
    }
    
    // Quits the application
    static void Quit()
    {
        exit(0);
    }
    
    // Generates a random number between the max and min values.
    // Precision is the number of decimal places you want to be randomized
    static float RandomNumber(float min, float max, int precision)
    {
        // Random number engine
        std::random_device rd;
        std::mt19937 gen(rd());
        
        int r_min = min * precision;
        int r_max = max * precision;
        
        // Define a uniform distribution
        std::uniform_int_distribution<> dis(r_min, r_max);
        
        // Generate and return the random number
        float num = dis(gen) / (float)precision;
        return num;
    }
    
    // Puts the application to sleep for the specified number of milliseconds
    static void Sleep(int duration_ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }
    
    // Opens a url in the os that this application is running on
    static void OpenURL(std::string url)
    {
        std::string command;
        
#ifdef _WIN32
        command = "start " + url;
#endif
        
#ifdef __APPLE__
        command = "open " + url;
#endif
        
#ifdef __linux__
       command = "xdg-open " + url;
#endif
        
        std::system(command.c_str());
        
    }
};

#endif /* Application_h */
