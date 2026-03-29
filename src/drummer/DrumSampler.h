#pragma once
#include <JuceHeader.h>
#include "DrumPattern.h"

/**
 * DrumSampler — F5f / F5g
 *
 * Loads the Salamander Drumkit WAV samples and renders drum hits into
 * an audio buffer based on a DrumPattern step sequence.
 *
 * Expected directory layout (flexible — first WAV in each subdir is used):
 *   <salamanderRoot>/kick/   → kick sample
 *   <salamanderRoot>/snare/  → snare sample
 *   <salamanderRoot>/hihat/  → hi-hat sample
 *   <salamanderRoot>/crash/  → crash sample
 *   <salamanderRoot>/ride/   → ride sample
 *   <salamanderRoot>/tom/    → tom sample
 *
 * Missing samples produce silence for that voice; the engine keeps running.
 */
class DrumSampler
{
public:
    DrumSampler();
    ~DrumSampler() = default;

    /** Register device sample rate for future resampling support. */
    void prepare (double sampleRate);

    /**
     * Load samples from the Salamander root directory.
     * Safe to call from the message thread before playback starts.
     * @return true if at least one voice loaded successfully.
     */
    bool loadSamples (const juce::File& salamanderRoot);

    /** True if loadSamples() succeeded for at least one voice. */
    bool areSamplesLoaded() const noexcept { return anyVoiceLoaded; }

    /**
     * Render the next block of drum audio into outBuffer.
     * Adds to existing content (call outBuffer.clear() first if needed).
     *
     * @param outBuffer       Destination stereo buffer.
     * @param numSamples      Block size.
     * @param sampleRate      Current device sample rate.
     * @param pattern         Active DrumPattern.
     * @param bpm             Current tempo.
     * @param playheadSample  Position within the current pattern (samples).
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
        int  playPos      { -1 };  ///< -1 = inactive
        int  triggerOffset { 0 };  ///< sample offset within current block
    };

    bool loadFirstWavInDir (const juce::File& dir, juce::AudioBuffer<float>& dest);
    void triggerVoice      (int voiceIndex, int blockOffset);
    void mixVoices         (juce::AudioBuffer<float>& outBuffer, int numSamples);

    static constexpr int kNumVoices = 6;  // Kick, Snare, HiHat, Crash, Ride, Tom
    static const char*   kVoiceDirs[kNumVoices];

    Voice voices[kNumVoices];

    juce::AudioFormatManager formatManager;
    double currentSampleRate { 44100.0 };
    bool   anyVoiceLoaded    { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumSampler)
};
