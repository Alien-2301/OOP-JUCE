/*
  ==============================================================================

    DeckGUI.cpp
    Created: 9 Feb 2026 4:47:15pm
    Author:  San Shwe Htet

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

DeckGUI::DeckGUI(DJAudioPlayer*  _djAudioPlayer,
                 juce::AudioFormatManager& formatManagerToUse,
                 juce::AudioThumbnailCache& cacheToUse,
                 CueManager*  _cueManager,
                 EQManager*   _eqManager)
    : djAudioPlayer { _djAudioPlayer },
      waveformDisplay(formatManagerToUse, cacheToUse),
      cueManager(_cueManager),
      eqManager (_eqManager)
{
    // =========================================================================
    // PLAY BUTTON
    // =========================================================================
    addAndMakeVisible(playButton);
    playButton.setButtonText("PLAY");
    playButton.addListener(this);

    // =========================================================================
    // PAUSE BUTTON
    // =========================================================================
    addAndMakeVisible(pauseButton);
    pauseButton.setButtonText("PAUSE");
    pauseButton.addListener(this);

    // =========================================================================
    // LOAD BUTTON
    // =========================================================================
    addAndMakeVisible(loadButton);
    loadButton.setButtonText("LOAD");
    loadButton.addListener(this);

    // =========================================================================
    // VOLUME SLIDER
    // =========================================================================
    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);

    // =========================================================================
    // POSITION SLIDER
    // =========================================================================
    addAndMakeVisible(positionSlider);
    positionSlider.setRange(0.0, 1.0);
    positionSlider.addListener(this);

    // =========================================================================
    // SPEED SLIDER
    // =========================================================================
    addAndMakeVisible(speedSlider);
    speedSlider.setRange(0.5, 2.0);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);

    // =========================================================================
    // WAVEFORM DISPLAY
    // =========================================================================
    addAndMakeVisible(waveformDisplay);

    // =========================================================================
    // WAVEFORM POSITION SLIDER
    // =========================================================================
    addAndMakeVisible(waveFormPosSlider);
    waveFormPosSlider.addListener(this);

    // =========================================================================
    // R4 – EQ KNOBS  (range: -24 dB … +24 dB, default 0 dB)
    // =========================================================================
    //constructor
    auto setupEQKnob = [this](juce::Slider& s, juce::Label& lbl, const juce::String& name)
    {
        s.setSliderStyle(juce::Slider::Rotary);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        s.setRange(-24.0, 24.0, 0.1);
        s.setValue(0.0);
        s.addListener(this);
        addAndMakeVisible(s);

        lbl.setText(name, juce::dontSendNotification);
        lbl.setJustificationType(juce::Justification::centred);
        lbl.setFont(juce::Font(12.0f, juce::Font::bold));
        addAndMakeVisible(lbl);
    };

    setupEQKnob(lowEQSlider,  lowEQLabel,  "LOW");
    setupEQKnob(midEQSlider,  midEQLabel,  "MID");
    setupEQKnob(highEQSlider, highEQLabel, "HIGH");

    // =========================================================================
    // HOT CUE BUTTONS
    // =========================================================================
    addAndMakeVisible(resetAllCues);
    resetAllCues.addListener(this);

    for (int i = 0; i < (int)cueButtons.size(); ++i)
    {
        cueButtons[i].setButtonText("C" + juce::String(i + 1));
        cueButtons[i].addListener(this);
        addAndMakeVisible(cueButtons[i]);
    };
    
    //R5: BPM label construction
          addAndMakeVisible(bpmText);
          bpmText.setJustificationType(juce::Justification::centred);

    // =========================================================================
    // TIMER
    // =========================================================================
    startTimer(200);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

// =============================================================================
// Layout
// =============================================================================

void DeckGUI::resized()
{
    // We now have 12 rows (was 10) to fit the EQ panel
    const float rowH = (float)getHeight() / 12.0f;

    playButton   .setBounds(0, 0,           getWidth(), (int)rowH);
    pauseButton  .setBounds(0, (int)rowH,   getWidth(), (int)rowH);
    volumeSlider .setBounds(0, (int)rowH*2, getWidth(), (int)rowH);
    positionSlider.setBounds(0,(int)rowH*3, getWidth(), (int)rowH);
    speedSlider  .setBounds(0, (int)rowH*4, getWidth(), (int)rowH);
    waveformDisplay.setBounds(0,(int)rowH*5, getWidth(), (int)(rowH * 2));

    // ----- EQ panel: label row + knob row (rows 7-8) -----
    const int eqTop     = (int)(rowH * 7);
    const int eqLblH    = 16;
    const int eqKnobH   = (int)rowH * 2 ;  // a little extra height for rotary
    const int eqW       = getWidth() / 3;

    for (int i = 0; i < 3; ++i)
    {
        int x = i * eqW;
        juce::Label*  lbl = (i == 0) ? &lowEQLabel  : (i == 1) ? &midEQLabel  : &highEQLabel;
        juce::Slider* sld = (i == 0) ? &lowEQSlider : (i == 1) ? &midEQSlider : &highEQSlider;

        lbl->setBounds(x, eqTop,             eqW, eqLblH);
        sld->setBounds(x, eqTop + eqLblH,    eqW, eqKnobH);
    }

    // ----- Cue buttons (row 10) -----
    const int numCues    = (int)cueButtons.size();
    const int gap        = 5;
    const int buttonW    = (getWidth() - (numCues + 1) * gap) / numCues;
    const int buttonH    = (int)rowH - 5;
    const int cueY       = (int)(rowH * 10) + 2;

    for (int i = 0; i < numCues; ++i)
        cueButtons[i].setBounds(gap + i * (buttonW + gap), cueY, buttonW, buttonH);

    resetAllCues.setBounds(getWidth() / 2, (int)(rowH * 11), getWidth() / 4, (int)rowH);
    loadButton  .setBounds(0,              (int)(rowH * 11), getWidth() / 2, (int)rowH);
    
    //R5 bpm text above loadButton
    bpmText.setBounds(0, rowH * 8, getWidth() / 2, rowH);
}

// =============================================================================
// Paint
// =============================================================================

void DeckGUI::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Draw a subtle border around the EQ section
    const float rowH = (float)getHeight() / 12.0f;
    juce::Rectangle<float> eqArea(0.0f, rowH * 7.0f,
                                   (float)getWidth(), rowH * 2.5f);
    g.setColour(juce::Colours::darkviolet.withAlpha(0.4f));
    g.drawRoundedRectangle(eqArea.reduced(2), 4.0f, 1.5f);

    g.setColour(juce::Colours::grey.withAlpha(0.4f));
    g.setFont(juce::Font(10.0f));
    g.drawText("EQ", eqArea.reduced(4), juce::Justification::topLeft, false);
}

// =============================================================================
// Button clicks
// =============================================================================

void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        djAudioPlayer->play();
        DBG("Play Button clicked");
    }
    else if (button == &pauseButton)
    {
        djAudioPlayer->stop();
        DBG("Pause Button clicked");
    }
    else if (button == &loadButton)
    {
        DBG("Load Button clicked");
        fChooser.launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& chooser)
            {
                auto file = chooser.getResult();
                if (file != juce::File{})
                {
                    juce::URL audioURL { file };
                    djAudioPlayer->loadURL(audioURL);
                    waveformDisplay.loadURL(audioURL);

                    currentTrackPath = file.getFullPathName().toStdString();

                    // Load saved EQ for this track
                    if (eqManager != nullptr)
                    {
                        eqManager->loadTrackEQ(currentTrackPath, djAudioPlayer);
                        lowEQSlider .setValue(djAudioPlayer->getLowEQ(),  juce::dontSendNotification);
                        midEQSlider .setValue(djAudioPlayer->getMidEQ(),  juce::dontSendNotification);
                        highEQSlider.setValue(djAudioPlayer->getHighEQ(), juce::dontSendNotification);
                    }
                }
                else
                {
                    DBG("No file selected.");
                }
            });
    }
    else if (button == &resetAllCues)
    {
        djAudioPlayer->UnregisterAllCues();
    }
    else
    {
        // Hot-cue buttons
        for (int i = 0; i < (int)cueButtons.size(); ++i)
        {
            if (button == &cueButtons[i])
            {
                if (djAudioPlayer->cueRegisitered(i))
                {
                    DBG("Jump to Cue " << i);
                    djAudioPlayer->jumpToCue(i);
                }
                else
                {
                    DBG("Registering Cue " << i);
                    djAudioPlayer->setCue(i, djAudioPlayer->gettrackPositionSeconds());
                    if (cueManager != nullptr && !currentTrackPath.empty())
                        cueManager->saveTrackCues(currentTrackPath, djAudioPlayer);
                }
                break;
            }
        }
    }
}

// =============================================================================
// Slider changes
// =============================================================================

void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        djAudioPlayer->setGain(slider->getValue());
    }
    else if (slider == &positionSlider)
    {
        djAudioPlayer->setPositionRelative(slider->getValue());
    }
    else if (slider == &speedSlider)
    {
        djAudioPlayer->setSpeed(slider->getValue());
    }
    // R4 – EQ knobs
    else if (slider == &lowEQSlider)
    {
        djAudioPlayer->setLowEQ(slider->getValue());
        if (eqManager != nullptr && !currentTrackPath.empty())
            eqManager->saveTrackEQ(currentTrackPath, djAudioPlayer);
    }
    else if (slider == &midEQSlider)
    {
        djAudioPlayer->setMidEQ(slider->getValue());
        if (eqManager != nullptr && !currentTrackPath.empty())
            eqManager->saveTrackEQ(currentTrackPath, djAudioPlayer);
    }
    else if (slider == &highEQSlider)
    {
        djAudioPlayer->setHighEQ(slider->getValue());
        if (eqManager != nullptr && !currentTrackPath.empty())
            eqManager->saveTrackEQ(currentTrackPath, djAudioPlayer);
    }
}

// =============================================================================
// Timer callback
// =============================================================================

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(djAudioPlayer->trackPositionRelative());

    for (int i = 0; i < (int)cueButtons.size(); ++i)
    {
        if (djAudioPlayer->cueRegisitered(i))
        {
            cueButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::hotpink);
            cueButtons[i].setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        }
        else
        {
            cueButtons[i].removeColour(juce::TextButton::buttonColourId);
            cueButtons[i].removeColour(juce::TextButton::textColourOffId);
        }
    }
    
    // R5: BPM display
    double bpm = djAudioPlayer->getRefinedBPM();
       if (bpm > 0.0)
           bpmText.setText("BPM: " + juce::String(bpm, 1), juce::dontSendNotification);
       else
           bpmText.setText("BPM: --", juce::dontSendNotification);
}

// =============================================================================
// File drag & drop
// =============================================================================

bool DeckGUI::isInterestedInFileDrag(const juce::StringArray&)
{
    return true;
}

void DeckGUI::filesDropped(const juce::StringArray& files, int, int)
{
    if (files.size() != 1) return;

    juce::File file { files[0] };
    djAudioPlayer->loadURL(juce::URL { file });
    waveformDisplay.loadURL(juce::URL { file });

    currentTrackPath = file.getFullPathName().toStdString();
    djAudioPlayer->setTrackFilePath(currentTrackPath);

    if (cueManager != nullptr)
        cueManager->loadTrackCues(currentTrackPath, djAudioPlayer);

    // R4 – restore EQ and update knobs
    if (eqManager != nullptr)
    {
        eqManager->loadTrackEQ(currentTrackPath, djAudioPlayer);
        lowEQSlider .setValue(djAudioPlayer->getLowEQ(),  juce::dontSendNotification);
        midEQSlider .setValue(djAudioPlayer->getMidEQ(),  juce::dontSendNotification);
        highEQSlider.setValue(djAudioPlayer->getHighEQ(), juce::dontSendNotification);
    }
}

// =============================================================================
// loadTrack (called from PlayListComponent)
// =============================================================================

void DeckGUI::loadTrack(juce::URL audioURL)
{
    // Save cues & EQ for the track we are leaving
    if (!currentTrackPath.empty())
    {
        if (cueManager != nullptr)
            cueManager->saveTrackCues(currentTrackPath, djAudioPlayer);
        if (eqManager != nullptr)
            eqManager->saveTrackEQ(currentTrackPath, djAudioPlayer);
    }

    djAudioPlayer->UnregisterAllCues();
    djAudioPlayer->loadURL(audioURL);
    waveformDisplay.loadURL(audioURL);

    currentTrackPath = audioURL.getLocalFile().getFullPathName().toStdString();

    if (cueManager != nullptr)
        cueManager->loadTrackCues(currentTrackPath, djAudioPlayer);

    // R4 – restore EQ and update knobs
    if (eqManager != nullptr)
    {
        eqManager->loadTrackEQ(currentTrackPath, djAudioPlayer);
        lowEQSlider .setValue(djAudioPlayer->getLowEQ(),  juce::dontSendNotification);
        midEQSlider .setValue(djAudioPlayer->getMidEQ(),  juce::dontSendNotification);
        highEQSlider.setValue(djAudioPlayer->getHighEQ(), juce::dontSendNotification);
    }
}
