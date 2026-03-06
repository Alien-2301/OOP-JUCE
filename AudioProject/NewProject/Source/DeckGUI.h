/*
  ==============================================================================

    DeckGUI.h
    Created: 9 Feb 2026 4:47:15pm
    Author:  San Shwe Htet

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "DjAudioPlayer.h"
#include "WaveFormDisplay.h"
#include "CueStorage.h"
#include "EQStorage.h"       // R4

class DeckGUI : public juce::Component,
                public juce::Button::Listener,
                public juce::Slider::Listener,
                public juce::FileDragAndDropTarget,
                public juce::Timer
{
public:
    DeckGUI(DJAudioPlayer*  _djAudioPlayer,
            juce::AudioFormatManager& formatManagerToUse,
            juce::AudioThumbnailCache& cacheToUse,
            CueManager*  _cueManager,
            EQManager*   _eqManager);        // R4
    ~DeckGUI();

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    void loadTrack(juce::URL audioURL);

private:
    // -------------------------------------------------------------------------
    // Buttons
    // -------------------------------------------------------------------------
    juce::TextButton playButton;
    juce::TextButton pauseButton;
    juce::TextButton loadButton;

    // -------------------------------------------------------------------------
    // Sliders
    // -------------------------------------------------------------------------
    juce::Label  volumeLabel;
    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Slider waveFormPosSlider;
    juce::Slider speedSlider;

    // R4 – EQ knobs (rotary sliders)
    juce::Slider lowEQSlider;
    juce::Slider midEQSlider;
    juce::Slider highEQSlider;

    // R4 – EQ labels
    juce::Label lowEQLabel;
    juce::Label midEQLabel;
    juce::Label highEQLabel;

    // -------------------------------------------------------------------------
    // File chooser
    // -------------------------------------------------------------------------
    juce::FileChooser fChooser { "Select a file..." };

    // -------------------------------------------------------------------------
    // Audio player
    // -------------------------------------------------------------------------
    DJAudioPlayer* djAudioPlayer;

    // -------------------------------------------------------------------------
    // Waveform
    // -------------------------------------------------------------------------
    WaveFormDisplay waveformDisplay;

    // -------------------------------------------------------------------------
    // Cue manager (R3)
    // -------------------------------------------------------------------------
    CueManager*  cueManager;
    std::string  currentTrackPath;

    std::array<juce::TextButton, 7> cueButtons;
    juce::TextButton resetAllCues { "Reset Cues" };

    // -------------------------------------------------------------------------
    // EQ manager (R4)
    // -------------------------------------------------------------------------
    EQManager* eqManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DeckGUI)
    
    // -------------------------------------------------------------------------
    // R5: Adding the BPM label
    // -------------------------------------------------------------------------
    juce::Label bpmText;
    
};
