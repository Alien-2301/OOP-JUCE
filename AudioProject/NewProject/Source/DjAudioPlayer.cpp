/*
  ==============================================================================

    DJAudioPlayer.cpp
    Created: 8 Feb 2026 5:53:17am
    Author:  San Shwe Htet

  ==============================================================================
*/

#include "DjAudioPlayer.h"

#include <algorithm>
#include <vector>

using namespace juce;

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager)
    : formatManager(_formatManager)
{
    UnregisterAllCues();
}

DJAudioPlayer::~DJAudioPlayer() {}

// =============================================================================
// AudioSource overrides
// =============================================================================

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource .prepareToPlay(samplesPerBlockExpected, sampleRate);

    // Build the DSP spec and prepare the three EQ filters
    dspSpec.sampleRate       = sampleRate;
    dspSpec.maximumBlockSize = (juce::uint32)samplesPerBlockExpected;
    dspSpec.numChannels      = 2;

    lowFilter .prepare(dspSpec);
    midFilter .prepare(dspSpec);
    highFilter.prepare(dspSpec);

    dspPrepared = true;
    updateEQCoefficients();
}

void DJAudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // 1. Fill buffer from the resampling / transport chain
    resampleSource.getNextAudioBlock(bufferToFill);

    if (!dspPrepared) return;

    // 2. Wrap the buffer in a DSP AudioBlock and run it through the EQ filters
    juce::dsp::AudioBlock<float> block(
        bufferToFill.buffer->getArrayOfWritePointers(),
        (size_t)bufferToFill.buffer->getNumChannels(),
        (size_t)bufferToFill.startSample,
        (size_t)bufferToFill.numSamples);

    juce::dsp::ProcessContextReplacing<float> context(block);
    lowFilter .process(context);
    midFilter .process(context);
    highFilter.process(context);
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource .releaseResources();

    if (dspPrepared)
    {
        lowFilter .reset();
        midFilter .reset();
        highFilter.reset();
        dspPrepared = false;
    }
}

// =============================================================================
// File / playback control
// =============================================================================

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr)
    {
        DBG("File Loaded Successfully");
        std::unique_ptr<AudioFormatReaderSource> newSource(
            new AudioFormatReaderSource(reader, true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        
        //R5
        analyzeBPM(reader); //passing the raw ptr before the readerSource
    }
}

void DJAudioPlayer::play()  { transportSource.start(); }
void DJAudioPlayer::stop()  { transportSource.stop();  }

void DJAudioPlayer::setPosition(double posInSecs)
{
    if (posInSecs < 0 || posInSecs > transportSource.getLengthInSeconds())
        DBG("ERROR: AudioPosition out of range");
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (readerSource == nullptr) return;
    setPosition(pos * transportSource.getLengthInSeconds());
}

void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0.0 || gain > 1.0)
    {
        DBG("DJAudioPlayer::setGain: warning – gain out of range");
        return;
    }
    transportSource.setGain((float)gain);
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0.0 || ratio > 100.0)
        DBG("DJAudioPlayer::setSpeed: ratio out of range");
    else{
        currentSampleRate = ratio;
        resampleSource.setResamplingRatio(ratio);
        DBG("Current Sampling ratio is: " << currentSampleRate);
    }
        
}

float DJAudioPlayer::trackPositionRelative()
{
    double len = transportSource.getLengthInSeconds();
    if (len <= 0.0) return 0.0f;
    return (float)(transportSource.getCurrentPosition() / len);
}

double DJAudioPlayer::gettrackPositionSeconds()
{
    return transportSource.getCurrentPosition();
}

// =============================================================================
// R4 – EQ
// =============================================================================

void DJAudioPlayer::setLowEQ(double gainDb)
{
    lowGainDb = gainDb;
    if (dspPrepared) updateEQCoefficients();
}

void DJAudioPlayer::setMidEQ(double gainDb)
{
    midGainDb = gainDb;
    if (dspPrepared) updateEQCoefficients();
}

void DJAudioPlayer::setHighEQ(double gainDb)
{
    highGainDb = gainDb;
    if (dspPrepared) updateEQCoefficients();
}

void DJAudioPlayer::updateEQCoefficients()
{
    // Band crossover frequencies
    // Low shelf  : below 250 Hz
    // Peak (mid) : centred at 1 kHz
    // High shelf : above 5 kHz

    const double sr = dspSpec.sampleRate;

    *lowFilter.state  = *FilterCoefs::makeLowShelf (sr, 250.0,  0.707, (float)Decibels::decibelsToGain(lowGainDb));
    *midFilter.state  = *FilterCoefs::makePeakFilter(sr, 1000.0, 0.707, (float)Decibels::decibelsToGain(midGainDb));
    *highFilter.state = *FilterCoefs::makeHighShelf (sr, 5000.0, 0.707, (float)Decibels::decibelsToGain(highGainDb));
}

// =============================================================================
// R3 – Hot Cues
// =============================================================================

bool DJAudioPlayer::cueRegisitered(int cueIndex) const
{
    return (cueIndex >= 0 && cueIndex < 8) && (hotCueArray[cueIndex] >= 0.0);
}

void DJAudioPlayer::setCue(int cueIndex, double position)
{
    if (readerSource == nullptr) { DBG("setCue: no track loaded"); return; }
    if (cueIndex >= 0 && cueIndex < 8)
        hotCueArray[cueIndex] = position;
}

double DJAudioPlayer::getCue(int cueIndex)
{
    return (cueIndex >= 0 && cueIndex < 8) ? hotCueArray[cueIndex] : -1.0;
}

void DJAudioPlayer::jumpToCue(int cueIndex)
{
    if (cueIndex >= 0 && cueIndex < 8 && hotCueArray[cueIndex] >= 0.0)
        setPosition(hotCueArray[cueIndex]);
}

void DJAudioPlayer::UnregisterCues(int cueIndex)
{
    if (cueIndex >= 0 && cueIndex < 8)
        hotCueArray[cueIndex] = -1.0;
}

void DJAudioPlayer::UnregisterAllCues()
{
    for (int i = 0; i < (int)hotCueArray.size(); ++i)
        hotCueArray[i] = -1.0;
}


// =============================================================================
// R5 – BPM Detector
// =============================================================================
void DJAudioPlayer::analyzeBPM(juce::AudioFormatReader* reader)
{
    if (reader == nullptr) return;

    const int sampleRate   = (int)reader->sampleRate;
    const int blockSize    = 512;
    const int maxSamples   = sampleRate * 60;
    const int totalSamples = std::min(maxSamples, (int)reader->lengthInSamples);

    // --- 1. Build energy array ---
    juce::AudioBuffer<float> buffer(1, blockSize);
    std::vector<float> energy;
    energy.reserve(totalSamples / blockSize);

    for (int pos = 0; pos + blockSize < totalSamples; pos += blockSize)
    {
        buffer.clear();
        reader->read(&buffer, 0, blockSize, pos, true, true);
        float rms = buffer.getRMSLevel(0, 0, blockSize);
        energy.push_back(rms * rms);
    }

    if (energy.empty()) return;

    // --- 2. Threshold ---
    float avgEnergy = 0.0f;
    for (float e : energy) avgEnergy += e;
    avgEnergy /= (float)energy.size();

    // --- 3. Peak-pick beats ---
    std::vector<int> beatFrames;
    const float threshold = avgEnergy * 1.5f;
    const int   minGap    = (int)(0.3 * sampleRate / blockSize);
    int lastBeat = -minGap;

    for (int i = 1; i < (int)energy.size() - 1; ++i)
    {
        if (energy[i] > threshold &&
            energy[i] > energy[i - 1] &&
            energy[i] > energy[i + 1] &&
            (i - lastBeat) > minGap)
        {
            beatFrames.push_back(i);
            lastBeat = i;
        }
    }

    if (beatFrames.size() < 2) { detectedBPM = 0.0; return; }

    // --- 4. Average interval → BPM ---
    double totalInterval = 0.0;
    for (int i = 1; i < (int)beatFrames.size(); ++i)
        totalInterval += beatFrames[i] - beatFrames[i - 1];

    double avgInterval    = totalInterval / (beatFrames.size() - 1);
    double secondsPerBeat = (avgInterval * blockSize) / sampleRate;
    detectedBPM           = 60.0 / secondsPerBeat;

    DBG("Detected BPM: " << detectedBPM);
}
