/*
  ==============================================================================

    PlayListComponent.h
    Created: 20 Feb 2026 12:38:50am
    Author:  San Shwe Htet

 ==============&=========&=======================================================
*/

#pragma once
#include <JuceHeader.h>
#include <iostream>
#include "DjAudioPlayer.h"
#include "DeckGUI.h"

class PlayListComponent :
public juce::Component,
public juce::TableListBoxModel,
public juce::Button::Listener

{
    public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //Component Class
//    PlayListComponent(DJAudioPlayer* player1, DJAudioPlayer* player2,  juce::AudioFormatManager& formatManager);
    
    // new Constructor using DeckGUI
    PlayListComponent(DeckGUI* _deck1, DeckGUI* _deck2,
                      DJAudioPlayer* _player1, DJAudioPlayer* _player2,
                      juce::AudioFormatManager& _formatManager);
    ~PlayListComponent() override;

    void paint(juce::Graphics &g) override;
    void resized() override;
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //TableListBoxModel Class
    int getNumRows() override;
    void paintRowBackground (juce::Graphics &,
                        int rowNumber,
                        int width,
                        int height,
                        bool rowIsSelected) override;
    void paintCell (juce::Graphics &,
                        int rowNumber,
                        int columnId,
                        int width,
                        int height,
                        bool rowIsSelected) override;
    
    virtual void cellClicked (int rowNumber, int columnId, const juce::MouseEvent &) override;
    
    virtual juce::Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;
    
    void buttonClicked (juce::Button*) override;

    private:
    
    juce::TableListBox tableComponent;
    
    //Declaring the track struct
    struct trackInfos{
        juce::File file;
        juce::String trackName;
        double trackDuration;
    };
    
    //Persistant states
    void loadFile(juce::File file);
    void saveLibrary(); void loadLibrary();
    juce::String formatDuration(double seconds);
    
    //Audio Formating
    juce::TextButton loadButton{"Add Tracks"};
    juce::FileChooser fChooser{"Select audio files...", {}, "*.mp3;*.wav;*.aiff;*.flac;*.ogg"};
//    juce::FileChooser fChooser{"Select audio files..."};
    juce::AudioFormatManager& formatManager;
    
    
    //Implementation of both DeckGUI and DJAudioPlayer for R2
    DJAudioPlayer* player1;
    DJAudioPlayer* player2;
    
    DeckGUI* deck1;
    DeckGUI* deck2;
    
    
    std::vector <trackInfos> tracks;
};
