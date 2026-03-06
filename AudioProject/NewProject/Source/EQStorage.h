/*
  ==============================================================================

    EQStorage.h
    Created: 28 Feb 2026 9:05:42pm
    Author:  San Shwe Htet

  ==============================================================================
*/

#pragma once



#pragma once
#include <JuceHeader.h>
#include <string>
#include <map>
#include <array>

#include "DjAudioPlayer.h"

// Stores { low, mid, high, originalLow, originalMid, originalHigh } per track
struct EQSettings
{
    double low          { 0.0 };
    double mid          { 0.0 };
    double high         { 0.0 };
    double originalLow  { 0.0 };   // first-ever value recorded for this track
    double originalMid  { 0.0 };
    double originalHigh { 0.0 };
};

class EQManager
{
public:
    EQManager();
    ~EQManager();

    // Apply the last saved EQ to the player, or flat (0 dB) if none saved.
    void loadTrackEQ (const std::string& trackPath, DJAudioPlayer* player);

    // Save the player's current EQ to memory + disk.
    void saveTrackEQ (const std::string& trackPath, DJAudioPlayer* player);

    // Persist / reload the whole map.
    void saveEQProfiles();
    void loadEQProfiles();

private:
    juce::File getDataFile();

    // trackPath → EQSettings
    std::map<std::string, EQSettings> eqMap;
};
