#pragma once

#include <JuceHeader.h>
#include "DjAudioPlayer.h"
#include "DeckGUI.h"
#include "PlayListComponent.h"
#include "CueStorage.h"
#include "EQStorage.h"      // R4

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioFormatManager formatManager;
    juce::AudioThumbnailCache cacheToUse { 100 };

    // Managers
    CueManager cueManager;
    EQManager  eqManager;              // R4

    // Deck 1
    DJAudioPlayer player1 { formatManager };
    DeckGUI       deck1   { &player1, formatManager, cacheToUse,
                            &cueManager, &eqManager };

    // Deck 2
    DJAudioPlayer player2 { formatManager };
    DeckGUI       deck2   { &player2, formatManager, cacheToUse,
                            &cueManager, &eqManager };

    // Mixer
    juce::MixerAudioSource mixerSource;

    // Playlist
    PlayListComponent playListComponent;

    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
