/*
  ==============================================================================

    EQStorage.cpp
    Created: 28 Feb 2026 9:05:42pm
    Author:  San Shwe Htet

  ==============================================================================
*/

#include "EQStorage.h"

// CSV columns: trackPath, low, mid, high, originalLow, originalMid, originalHigh

EQManager::EQManager()
{
    loadEQProfiles();
}

EQManager::~EQManager()
{
    saveEQProfiles();
}

// =============================================================================
// File helper
// =============================================================================

juce::File EQManager::getDataFile()
{
    juce::File f = juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                       .getChildFile("SynthWavePlayerEQ.csv");

    if (!f.existsAsFile())
    {
        f.create();
        f.appendText("trackPath,low,mid,high,originalLow,originalMid,originalHigh\n");
        DBG("EQManager: created new EQ file: " << f.getFullPathName());
    }
    return f;
}

// =============================================================================
// Load / save per-track
// =============================================================================

void EQManager::loadTrackEQ(const std::string& trackPath, DJAudioPlayer* player)
{
    if (trackPath.empty() || player == nullptr) return;

    auto it = eqMap.find(trackPath);
    if (it != eqMap.end())
    {
        player->setLowEQ (it->second.low);
        player->setMidEQ (it->second.mid);
        player->setHighEQ(it->second.high);
        DBG("EQManager: loaded EQ for \"" << trackPath << "\" "
            << "L=" << it->second.low
            << " M=" << it->second.mid
            << " H=" << it->second.high);
    }
    else
    {
        // First time this track is seen – flat EQ, record originals as 0 dB
        player->setLowEQ (0.0);
        player->setMidEQ (0.0);
        player->setHighEQ(0.0);
        DBG("EQManager: no saved EQ for \"" << trackPath << "\", using flat");
    }
}

void EQManager::saveTrackEQ(const std::string& trackPath, DJAudioPlayer* player)
{
    if (trackPath.empty() || player == nullptr) return;

    auto it = eqMap.find(trackPath);

    EQSettings s;
    s.low  = player->getLowEQ();
    s.mid  = player->getMidEQ();
    s.high = player->getHighEQ();

    if (it == eqMap.end())
    {
        // First save → originals == current
        s.originalLow  = s.low;
        s.originalMid  = s.mid;
        s.originalHigh = s.high;
    }
    else
    {
        // Keep the originals from when the track was first recorded
        s.originalLow  = it->second.originalLow;
        s.originalMid  = it->second.originalMid;
        s.originalHigh = it->second.originalHigh;
    }

    eqMap[trackPath] = s;

    DBG("EQManager: saved EQ for \"" << trackPath << "\" "
        << "L=" << s.low << " M=" << s.mid << " H=" << s.high);

    saveEQProfiles();
}

// =============================================================================
// CSV persistence
// =============================================================================

void EQManager::saveEQProfiles()
{
    juce::File f = getDataFile();
    juce::FileOutputStream out(f);
    if (!out.openedOk())
    {
        DBG("EQManager::saveEQProfiles – could not open file");
        return;
    }

    out.setPosition(0);
    out.truncate();
    out << "trackPath,low,mid,high,originalLow,originalMid,originalHigh\n";

    for (const auto& [path, s] : eqMap)
    {
        out << juce::String(path)  << ","
            << s.low               << ","
            << s.mid               << ","
            << s.high              << ","
            << s.originalLow       << ","
            << s.originalMid       << ","
            << s.originalHigh      << "\n";
    }

    DBG("EQManager: wrote " << (int)eqMap.size() << " track(s) to disk");
}

void EQManager::loadEQProfiles()
{
    juce::File f = getDataFile();
    juce::FileInputStream in(f);
    if (!in.openedOk()) { DBG("EQManager::loadEQProfiles – cannot open file"); return; }

    in.readNextLine(); // skip header

    while (!in.isExhausted())
    {
        juce::String line = in.readNextLine().trim();
        if (line.isEmpty()) continue;

        juce::StringArray t;
        t.addTokens(line, ",", "");
        if (t.size() < 7) continue;

        std::string path = t[0].toStdString();
        EQSettings s;
        s.low          = t[1].getDoubleValue();
        s.mid          = t[2].getDoubleValue();
        s.high         = t[3].getDoubleValue();
        s.originalLow  = t[4].getDoubleValue();
        s.originalMid  = t[5].getDoubleValue();
        s.originalHigh = t[6].getDoubleValue();

        eqMap[path] = s;
        DBG("EQManager: loaded EQ for \"" << path << "\"");
    }
    DBG("EQManager: loaded " << (int)eqMap.size() << " track(s)");
}
