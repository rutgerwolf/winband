#pragma once
#include <JuceHeader.h>
#include "DrumPattern.h"

/**
 * DrumSampler — F5f / F5g
 *
 * Loads the Salamander Drumkit WAV samples and renders drum hits into
 * audio buffers based on a DrumPattern step sequence.
 *
 * Sample files are expected at:
 *   <appDir>/assets/samples/salamander/<voice>/<voice>-<velocity>.wav
 * e.g. assets/samples/salamander/kick/kick-hard.wav
 */
class DrumSampler
{
public:
    DrumSampler();
    ~DrumSampler() = default;

    /** Load samples from the given root directory.  Call once at startup. */
    bool loadSamples (const juce::File& salamanderRoot);

    /**
     * Render the next block of drum audio.
     * @param outBuffer       Destination (stereo preferred).
     * @param numSamples      Block size.
     * @param sampleRate      Current device sample rate.
     * @param pattern         Active DrumPattern.
     * @param bpm             Current tempo.
     * @param playheadSample  Current position within the pattern (samples).
     */
    void processBlock (juce::AudioBuffer<float>& outBuffer,
                       int                       numSamples,
                       double                    sampleRate,
                       const DrumPattern&        pattern,
                       double                    bpm,
                       int                       playheadSample);

    void reset();

private:
    struct Voice
    {
        juce::AudioBuffer<float> sample;
        int                      playPos { -1 };  ///< -1 = inactive
    };

    bool loadWav (const juce::File& file, juce::AudioBuffer<float>& dest);
    void triggerVoice (int voiceIndex);
    void mixVoices (juce::AudioBuffer<float>& outBuffer, int numSamples);

    static constexpr int kNumVoices = 6;  // Kick, Snare, HiHat, Crash, Ride, Tom
    Voice voices[kNumVoices];

    juce::AudioFormatManager formatManager;
    bool samplesLoaded { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumSampler)
};
