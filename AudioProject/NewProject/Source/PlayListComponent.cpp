/*
  ==============================================================================

    PlayListComponent.cpp
    Created: 20 Feb 2026 12:38:50am
    Author:  San Shwe Htet

  ==============================================================================
*/

#include "PlayListComponent.h"
#include <JuceHeader.h>

PlayListComponent::PlayListComponent(DeckGUI* _deck1,
                                     DeckGUI* _deck2,
                                     DJAudioPlayer* _player1,
                                     DJAudioPlayer* _player2,
                                     juce::AudioFormatManager& _formatManager)
: deck1(_deck1), deck2(_deck2), player1(_player1), player2(_player2),  formatManager(_formatManager)
{
    /** Table Component
     1.  set Model
     2.  make it visible
     */
    tableComponent.setModel (this);
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(loadButton);
    loadButton.addListener(this);
    
    
    tableComponent.getHeader().addColumn("Title",    1, 300);
    tableComponent.getHeader().addColumn("Duration", 2, 100);
    tableComponent.getHeader().addColumn("Deck 1",   3, 100);
    tableComponent.getHeader().addColumn("Deck 2",   4, 100);
    
    //Persistance R2
    loadLibrary();
};


PlayListComponent::~PlayListComponent(){
    //Persistance R2
    saveLibrary();
};


void PlayListComponent::paint(juce::Graphics &g) {
    
};

void PlayListComponent::resized()
{
    int loadRowHeight = 30; // change this for adjustment
    loadButton.setBounds(0, 0, getWidth(), loadRowHeight);
    tableComponent.setBounds(0,loadRowHeight, getWidth(), getHeight() - loadRowHeight);
    
};

int PlayListComponent::getNumRows ()
{
    return tracks.size();
}

void PlayListComponent::paintRowBackground (juce::Graphics & g,
                    int rowNumber,
                    int width,
                    int height,
                    bool rowIsSelected)
{
    if (rowIsSelected)
        {
            g.fillAll(juce::Colours::orange);
    } else{
            g.fillAll(juce::Colours::darkgrey);
        
    }
    
}

void PlayListComponent::paintCell (
                                   juce::Graphics & g,
                                   int rowNumber,
                                   int columnId,
                                   int width,
                                   int height,
                                   bool rowIsSelected)
{
//    g.drawText (trackTitles[rowNumber], // the important bit
//                2, 0,
//                width - 4, height,
//                juce::Justification::centredLeft,
//                true);
    
    if (rowNumber >= (int)tracks.size()) return;
//    
    if (columnId == 1){
        g.drawText(tracks[rowNumber].trackName, 2, 0, width - 4, height,
                           juce::Justification::centredLeft, true);
    };
    
    if (columnId == 2){
        g.drawText(formatDuration(tracks[rowNumber].trackDuration), 2, 0, width - 4, height,
                           juce::Justification::centredLeft, true);
    };
}

void PlayListComponent::cellClicked (int rowNumber, int columnId, const juce::MouseEvent &)
{
    
}


/// PlayListComponent::loadFile = Loading and updating into the PlayList Component
void PlayListComponent::loadFile(juce::File file){
    auto* reader = formatManager.createReaderFor(file);
    if (reader != nullptr){
        trackInfos track;
        track.file  = file;
        track.trackName = file.getFileNameWithoutExtension();
        track.trackDuration = (double)reader->lengthInSamples / reader->sampleRate;
        tracks.push_back(track);
        
        // to avoid overload
        delete reader;
        //Updating to PlayListComponent
        tableComponent.updateContent();
    }
}

void PlayListComponent::loadLibrary(){
    juce::File saveFile = juce::File::getSpecialLocation(
        juce::File::userDocumentsDirectory).getChildFile("dj_library.csv");

    if (!saveFile.existsAsFile()) return;

    juce::FileInputStream stream(saveFile);
    while (!stream.isExhausted())
    {
        auto line   = stream.readNextLine();
        auto tokens = juce::StringArray::fromTokens(line, ",", "");
        if (tokens.size() == 2)
        {
            juce::File f(tokens[0]);
            if (f.existsAsFile())
            {
                trackInfos track;
                track.file     = f;
                track.trackName    = f.getFileNameWithoutExtension();
                track.trackDuration = tokens[1].getDoubleValue();
                tracks.push_back(track);
            }
        }
    }
    tableComponent.updateContent();
}

void PlayListComponent::saveLibrary(){
    juce::File saveFile = juce::File::getSpecialLocation(
        juce::File::userDocumentsDirectory).getChildFile("dj_library.csv");

    juce::FileOutputStream stream(saveFile);
    if (stream.openedOk())
    {
        stream.setPosition(0);
        stream.truncate();
        for (auto& track : tracks)
            stream << track.file.getFullPathName() << ","
            << track.trackDuration << "\n";
    }
}







juce::Component* PlayListComponent::refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate)
{
    
    if (columnId == 3 || columnId == 4){
        ///Scrolling Mechanic (Repaint new rows upon scrolling)
        if (existingComponentToUpdate == nullptr) //creating new row instead of recycling
        {
            juce::String label = "Play";
                        auto* btn = new juce::TextButton(label);
                        btn->addListener(this);
                        existingComponentToUpdate = btn;
        }
        // updating ID whenever JUCe recycles the case row
        existingComponentToUpdate->setComponentID(juce::String(rowNumber) + ":" + juce::String(columnId));
        return existingComponentToUpdate;
    }
    return nullptr;
}

void PlayListComponent::buttonClicked(juce::Button* button)
{
    if (button == &loadButton){
        DBG("PlaylistComponent::buttonClicked LoadButton");
        fChooser.launchAsync(
                             juce::FileBrowserComponent::openMode |
                             juce::FileBrowserComponent::canSelectFiles |
                             juce::FileBrowserComponent::canSelectMultipleItems,
                             [this](const juce::FileChooser& chooser)
                             {
                                 for (auto& file : chooser.getResults())
                                     loadFile(file);
                             });
        return;
    }
    juce::StringArray tokens = juce::StringArray::fromTokens(button->getComponentID(), ":", "");
    if (tokens.size() == 2)
    {
        int row = tokens[0].getIntValue();
        int col = tokens[1].getIntValue();
        
        if (row < (int)tracks.size())
        {
            juce::URL audioURL{tracks[row].file};
            
            if (col == 3)
            {
                deck1->loadTrack(audioURL);
                DBG("Loaded to Deck 1: " << tracks[row].trackName);
            }
            else if (col == 4)
            {
                deck2->loadTrack(audioURL);
                DBG("Loaded to Deck 2: " << tracks[row].trackName);
            };
        };
    };
};

juce::String PlayListComponent::formatDuration(double seconds)
{
    int mins = (int)seconds/60;
    int secs = (int)seconds % 60;
    
    return juce::String::formatted("%d:%02d", mins, secs);
}
