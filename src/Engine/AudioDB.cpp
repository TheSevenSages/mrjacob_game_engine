//
//  AudioDB.cpp
//  game_engine
//
//  Created by Jacob Robinson on 2/11/24.
//  Created for EECS 498: Game Engine Architecture at the University of Michigan
//
//  School Email: mrjacob@umich.edu

#include <stdio.h>

#include "AudioDB.h"

// Loads all of the audio in the resources/audio directory into loadedAudio
void AudioDB::LoadAudio()
{
    /* Load audio files in resources/audio */
    const std::string audioDirectoryPath = "resources/audio";
    
    // Fills up loadedAudio if the path exists
    if (std::filesystem::exists(audioDirectoryPath)) 
    {
        for (const auto& audioFile : std::filesystem::directory_iterator(audioDirectoryPath))
        {
            if (audioFile.path() != audioDirectoryPath + "/.DS_Store")
            {
                AudioHelper::Mix_OpenAudio498(48000, AUDIO_S16SYS, 1, 1024);
                Mix_Chunk* sound = AudioHelper::Mix_LoadWAV498(audioFile.path().string().c_str());
                loadedAudio[audioFile.path().filename().stem().stem().string()] = sound;
            }
        }
    }
    AudioHelper::Mix_AllocateChannels498(50);
}

// Play audio from loadedAudio based its name
void AudioDB::PlayAudio(int channel, std::string audioName, bool loop)
{
    if (loadedAudio.find(audioName) == loadedAudio.end())
    {
        std::cout << "error: failed to play audio clip " << audioName;
        exit(0);
    }
    
    // If loop is true this sound will loop forever, otherwise it won't loop at all
    int numLoops = 0;
    if (loop) {numLoops = -1;}
    
    AudioHelper::Mix_PlayChannel498(channel, loadedAudio[audioName], numLoops);
}

// Halt the audio on the given channel
void AudioDB::HaltAudio(int channel)
{
    AudioHelper::Mix_HaltChannel498(channel);
}

// Changes the volume of a specific audio channel
void AudioDB::SetVolume(int channel, int volume)
{
    AudioHelper::Mix_Volume498(channel, volume);
}
