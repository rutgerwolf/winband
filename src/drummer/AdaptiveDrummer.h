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
 *   1. Call prepare() when the audio device starts.
 *   2. Call setStyle() and setBpm() before playback.
 *   3. Feed guide-track audio into analyzeGuideBuffer() each block.
 *   4. Call processBlock() from the audio thread to fill the output buffer.
 */
class AdaptiveDrummer
{
public:
    AdaptiveDrummer();
    ~AdaptiveDrummer() = default;

    /** Call from audioDeviceAboutToStart / prepareToPlay. */
    void prepare (double sampleRate, int blockSize);

    void setStyle    (DrumStyle style);
    void   setBpm    (double bpm);
    double getBpm    () const noexcept { return bpm; }
    void setTapTempo ();  ///< Call on each tap; BPM derived from tap intervals.

    /** Load Salamander samples.  Call once after prepare(). */
    bool loadSamples (const juce::File& salamanderRoot);

    /** Feed a mono guide-track block for energy analysis. */
    void analyzeGuideBuffer (const juce::AudioBuffer<float>& buffer);

    /** Fill outBuffer with drum audio.  Call from the audio thread. */
    void processBlock (juce::AudioBuffer<float>& outBuffer,
                       int numSamples, double sampleRate);

    void reset();

private:
    void selectPattern();
    void checkForFill();

    DrumStyle currentStyle  { DrumStyle::Rock };
    double    bpm           { 120.0 };
    double    currentEnergy { 0.0 };
    float     prevEnergy    { 0.0f };  ///< for fill detection (was incorrectly static)
    bool      fillPending   { false };

    EnergyAnalyzer energyAnalyzer;
    DrumPattern    drumPattern;
    DrumSampler    drumSampler;

    // Tap-tempo state
    juce::int64 lastTapTime   { 0 };
    int         tapCount      { 0 };
    double      tapAccumBpm   { 0.0 };

    // Playback position (samples within current pattern)
    int playheadSample { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdaptiveDrummer)
};
