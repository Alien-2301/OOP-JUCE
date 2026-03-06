/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 8 Feb 2026 5:53:17am
    Author:  San Shwe Htet

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <juce_dsp/juce_dsp.h> 

class DJAudioPlayer : public juce::AudioSource
{
public:
    DJAudioPlayer(juce::AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // File loading & Audio Manipulating
    void loadURL(juce::URL file);
    void play();
    void stop();
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);
    void setSpeed(double ratio);
    void setGain(double gain);

    float  trackPositionRelative();
    double gettrackPositionSeconds();

    void         setTrackFilePath(const std::string& path) { currentFilePath = path; }
    juce::String getTrackFilePath() const                  { return currentFilePath; }

    // R3 Hot Cue
    void   setCue(int cueIndex, double position);
    double getCue(int cueIndex);
    void   jumpToCue(int cueIndex);
    void   UnregisterCues(int cueIndex);
    void   UnregisterAllCues();
    bool   cueRegisitered(int cueIndex) const;

    // =========================================================================
    // R4 – Three-band EQ
    // gainDb range: -24.0 dB … +24.0 dB  (0.0 = flat)
    // =========================================================================
    void   setLowEQ (double gainDb);
    void   setMidEQ (double gainDb);
    void   setHighEQ(double gainDb);

    double getLowEQ()  const { return lowGainDb;  };
    double getMidEQ()  const { return midGainDb;  };
    double getHighEQ() const { return highGainDb; };
    
    // =========================================================================
    // R5 – BPM Detector and Presentor
    // gainDb range: -24.0 dB … +24.0 dB  (0.0 = flat)
    // =========================================================================
    
    
    double getRawBPM() const {return detectedBPM;};
    double getRefinedBPM() const {return detectedBPM * currentSampleRate;};

private:
    // -------------------------------------------------------------------------
    // DSP helpers
    // -------------------------------------------------------------------------
    /** Update coefficients for all three bands. Called whenever a gain changes
     *  or after prepareToPlay supplies a valid sample rate. */
    void updateEQCoefficients();

    // -------------------------------------------------------------------------
    // Audio-source chain
    // -------------------------------------------------------------------------
    juce::AudioFormatManager& formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource    transportSource;
    juce::ResamplingAudioSource   resampleSource { &transportSource, false, 2 };

    // -------------------------------------------------------------------------
    // DSP EQ  (three second-order IIR filters)
    // -------------------------------------------------------------------------
    using Filter      = juce::dsp::IIR::Filter<float>;
    using FilterCoefs = juce::dsp::IIR::Coefficients<float>;

    // One stereo-capable filter per band  (ProcessorDuplicator handles 2 ch)
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> lowFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> midFilter;
    juce::dsp::ProcessorDuplicator<Filter, FilterCoefs> highFilter;

    juce::dsp::ProcessSpec dspSpec { 44100.0, 512, 2 };
    bool dspPrepared { false };

    // Current gain values (dB) – stored so we can persist / restore them
    double lowGainDb  { 0.0 };
    double midGainDb  { 0.0 };
    double highGainDb { 0.0 };

    juce::String currentFilePath;

    // R3 Hot Cue (Private)
    std::array<double, 8> hotCueArray;
    
    // R5 BPM
    void analyzeBPM(juce::AudioFormatReader* reader); //passing the pointer in the reader
    
    
    double detectedBPM = 0.0;
    double currentSampleRate = 1.0; //track the current sampling ratio
};
