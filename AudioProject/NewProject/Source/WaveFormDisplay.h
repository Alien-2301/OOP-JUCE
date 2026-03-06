/*
  ==============================================================================

    WaveFormDisplay.h
    Created: 17 Feb 2026 4:33:56am
    Author:  San Shwe Htet

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class WaveFormDisplay   : public juce::Component
{
    public:
    WaveFormDisplay(juce::AudioFormatManager & formatManagerToUse, juce::AudioThumbnailCache & cacheToUse);
    ~WaveFormDisplay();
    
    void paint (juce::Graphics&) override;
    void resized() override;
    void setPositionRelative(double pos);
    
    void loadURL(juce::URL audioURL);
    
    private:
    juce::AudioThumbnail audioThumbnail;
    bool fileLoaded;
    double position;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveFormDisplay);
};
