#pragma once
#include <JuceHeader.h>
#include "DrumPattern.h"
#include "EnergyAnalyzer.h"
#include "DrumSampler.h"

/** Drum styles supported by the adaptive drummer. */
enum class DrumStyle { Rock, Jazz, Funk, Electronic, Brushes };

/**
 * AdaptiveDrummer — F5 orchestrator.
 *
 * Ties together EnergyAnalyzer, DrumPattern, and DrumSampler to produce
 * audio output that adapts to the energy of a guide track.
 *
 * Usage:
 *   1. Call setStyle() and setBpm() before playback.
 *   2. Feed guide-track audio into analyzeGuideBuffer() per block.
 *   3. Call processBlock() from the audio thread to fill the output buffer.
 */
class AdaptiveDrummer
{
public:
    AdaptiveDrummer();
    ~AdaptiveDrummer() = default;

    //==========================================================================
    void setStyle (DrumStyle style);
    void setBpm   (double bpm);
    void setTapTempo ();          ///< Call on each tap; BPM is derived from intervals.

    /** Feed a mono guide-track block for energy analysis. */
    void analyzeGuideBuffer (const juce::AudioBuffer<float>& buffer, double sampleRate);

    /** Fill outBuffer with drum audio. Call from the audio thread. */
    void processBlock (juce::AudioBuffer<float>& outBuffer, int numSamples, double sampleRate);

    void reset();

private:
    void selectPattern();
    void scheduleFill();

    DrumStyle      currentStyle   { DrumStyle::Rock };
    double         bpm            { 120.0 };
    double         currentEnergy  { 0.0 };
    bool           fillPending    { false };

    EnergyAnalyzer energyAnalyzer;
    DrumPattern    drumPattern;
    DrumSampler    drumSampler;

    // Tap-tempo state
    juce::int64    lastTapTime    { 0 };
    int            tapCount       { 0 };
    double         tapAccumBpm    { 0.0 };

    // Playback position (in samples within current pattern)
    int            playheadSample { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdaptiveDrummer)
};
