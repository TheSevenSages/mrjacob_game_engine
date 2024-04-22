//
//  AudioDB.h
//  game_engine
//
//  Created by Jacob Robinson on 2/11/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#ifndef AudioDB_h
#define AudioDB_h

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
#include "AudioHelper.h"

#include "SDL2/SDL.h"
#include "SDL2_mixer/SDL_mixer.h"

class AudioDB
{
public:
    // Loads all of the audio in the resources/audio directory into loadedAudio
    static void LoadAudio();
    
    // Play audio from loadedAudio based on its name
    static void PlayAudio(int channel, std::string audioName, bool loop);
    
    // Halt the audio on the given channel
    static void HaltAudio(int channel);
    
    // Changes the volume of a specific audio channel
    static void SetVolume(int channel, int volume);
    
private:
    static inline std::unordered_map<std::string, Mix_Chunk*> loadedAudio;
};
#endif /* AudioDB_h */
