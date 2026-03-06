/*
  ==============================================================================

    CuePanel.cpp
    Created: 23 Feb 2026 11:28:12pm
    Author:  San Shwe Htet

  ==============================================================================
*/


#include "DjAudioPlayer.h"
#include "CueStorage.h"

#include <array>
#include <map>
#include <fstream>

CueManager::CueManager(){
    loadCueProfiles();
    readCueDataFile();
    
}

CueManager::~CueManager(){
//    saveCueProfiles();
    saveCueProfiles();
}

//Extracting Files

juce::File CueManager::readCueDataFile(){
    

    juce::File homeDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory);
    juce::File csvFile = homeDir.getChildFile("SynthWavePlayerCue.csv");
    
    if (!csvFile.existsAsFile()){
        DBG("File does not exists. Creating: " << csvFile.getFullPathName());
        // Create an empty file
        auto fileCreation = csvFile.create();
        if (fileCreation.wasOk())
        {DBG ("Created new cue file: " << csvFile.getFullPathName());
        //Header for CSV
            csvFile.appendText("trackPath,C1,C2,C3,C4,C5,C6,C7,C8\n");
        }
        
        else DBG ("Failed to create newfile" << fileCreation.getErrorMessage());
        
    }
    
    DBG("Retrieving the file: " << csvFile.getFullPathName());
    
    
    return csvFile;
}


void CueManager::saveTrackCues(const std::string& audioTrack, DJAudioPlayer* player){
    
    if (audioTrack.empty() || player == nullptr)
        {
            DBG("saveTrackCues: empty track path or null player – skipping");
            return;
        }
    
    std::array<double, 8> cues;
       for (int i = 0; i < 8; ++i)
           cues[i] = player->getCue(i);
   
       cueMap[audioTrack] = cues;
   
       DBG("saveTrackCues: stored cues for \"" << audioTrack << "\"");
       for (int i = 0; i < 8; ++i)
           DBG("  C" << (i + 1) << " = " << cues[i]);
   
       saveCueProfiles();
    
    
}

void CueManager::loadTrackCues(const std::string& audioTrack, DJAudioPlayer* player)
{
    auto it = cueMap.find(audioTrack);

    if (it != cueMap.end())
    {
        DBG("Cues found for track: " << audioTrack);

        // Apply cues to the player
        for (int i = 0; i < 8; ++i)
            player->setCue(i, it->second[i]);
    }
    else
    {
        DBG("No saved cues for track: " << audioTrack);
        player->UnregisterAllCues();
    }
}


void CueManager::saveCueProfiles(){
    juce::File csvFile = readCueDataFile();
    
        // Overwrite cleanly: truncate via FileOutputStream rather than
        // delete+create, which avoids a race where the file briefly doesn't exist.
        juce::FileOutputStream out(csvFile);
        if (!out.openedOk())
        {
            DBG("saveCueProfiles: could not open file for writing – "
                << out.getStatus().getErrorMessage());
            return;
        }
    
        out.setPosition(0);
        out.truncate();
    
        out << "trackPath,C1,C2,C3,C4,C5,C6,C7,C8\n";
    
        for (const auto& [trackPath, cues] : cueMap)
        {
            out << juce::String(trackPath);
            for (int i = 0; i < 8; ++i)
                out << "," << cues[i];
            out << "\n";
        }
    
        DBG("saveCueProfiles: wrote " << (int)cueMap.size() << " track(s) to disk");
}


void CueManager::loadCueProfiles()
{
    juce::File csvFile = readCueDataFile();
    juce::FileInputStream in(csvFile);

    if (!in.openedOk())
    {
        DBG("loadCueProfiles: could not open file for reading");
        return;
    }

    // Skip header line
    in.readNextLine();

    while (!in.isExhausted())
    {
        juce::String line = in.readNextLine().trim();
        if (line.isEmpty()) continue;

        juce::StringArray tokens;
        tokens.addTokens(line, ",", "");

        if (tokens.size() < 9) continue;  // need path + 8 cues

        std::string trackPath = tokens[0].toStdString();
        std::array<double, 8> cues;

        for (int i = 0; i < 8; ++i)
            cues[i] = tokens[i + 1].getDoubleValue();

        cueMap[trackPath] = cues;
        DBG("loadCueProfiles: loaded cues for \"" << trackPath << "\"");
    }

    DBG("loadCueProfiles: loaded " << (int)cueMap.size() << " track(s)");
}
