/*
  ==============================================================================

    CuePanel.h
    Created: 23 Feb 2026 11:28:12pm
    Author:  San Shwe Htet

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <string>
#include <map>
#include <array>

#include "DjAudioPlayer.h"

class CueManager

{
public:
    CueManager();
    ~CueManager();
    
    void saveTrackCues(const std::string& audioTrack, DJAudioPlayer* player);
    
    void loadTrackCues(const std::string& audioTrack, DJAudioPlayer* player);
    
    void saveCueProfiles();
    
    void loadCueProfiles();

        
private:
    
    juce::File readCueDataFile();
    
    //music filename = [Array of 8 Cue buttons]
    std::map<std::string, std::array<double, 8>> cueMap;
};

