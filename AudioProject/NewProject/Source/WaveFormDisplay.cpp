/*
  ==============================================================================

    WaveFormDisplay.cpp
    Created: 17 Feb 2026 4:33:56am
    Author:  San Shwe Htet

  ==============================================================================
*/

#include "WaveFormDisplay.h"
#include <JuceHeader.h>

WaveFormDisplay::WaveFormDisplay(juce::AudioFormatManager & formatManagerToUse, juce::AudioThumbnailCache & cacheToUse):
audioThumbnail(1000, formatManagerToUse, cacheToUse),
fileLoaded(false),
position(0)
{
    
}

WaveFormDisplay::~WaveFormDisplay(){

}


void WaveFormDisplay::resized(){
    
}

void WaveFormDisplay::paint(juce::Graphics& g){
    
    //Background Color
    g.fillAll (getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    //Outline
    g.setColour (juce::Colours::darkviolet);
    g.drawRect (getLocalBounds(), 1);
    
    if (fileLoaded){
        audioThumbnail.drawChannel(g, getLocalBounds(), 0.0, audioThumbnail.getTotalLength(), 0, 1.0f);
        g.setColour(juce::Colours::lightgreen);
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight());
    }
    else {
        g.setColour (juce::Colours::pink);
        g.setFont (24.0f);
        g.drawText ("WaveformDisplay", getLocalBounds(),
                       juce::Justification::centred, true);
    }
    
}

void WaveFormDisplay::setPositionRelative(double pos)
{
    if (pos != position)
    {
        position = pos;
repaint(); }
}

void WaveFormDisplay::loadURL(juce::URL audioURL){
    audioThumbnail.clear();
     fileLoaded  = audioThumbnail.setSource(new juce::URLInputSource(audioURL));
     if (fileLoaded)
     {
       std::cout << "wfd: loaded! " << std::endl;
       repaint();
     }
     else {
       std::cout << "wfd: not loaded! " << std::endl;
     }
}
