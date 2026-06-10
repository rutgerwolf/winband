#pragma once
#include <JuceHeader.h>

class TrackBase;

/**
 * AudioEngine
 *
 * Opens and manages the audio device (ASIO preferred, WASAPI fallback).
 * Routes device input to registered tracks and mixes their output to the
 * device output.
 *
 * Thread safety:
 *   addTrack / removeTrack / clearTracks may be called from any thread.
 *   The audio callback holds a read lock; track mutations hold a write lock.
 */
class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();
    ~AudioEngine() override;

    // ── Lifecycle ─────────────────────────────────────────────────────────
    /** Open preferred device. Returns false and logs on failure. */
    bool initialise();
    void shutdown();

    juce::AudioDeviceManager& getDeviceManager() noexcept { return deviceManager; }

    // ── Track management (non-owning) ─────────────────────────────────────
    void addTrack    (TrackBase* track);
    void removeTrack (TrackBase* track);
    void clearTracks ();

    // ── Transport ─────────────────────────────────────────────────────────
    void play();
    void stop();
    void record();

    bool isPlaying()   const noexcept { return playing.load(); }
    bool isRecording() const noexcept { return recording.load(); }

    double getSampleRate() const noexcept { return currentSampleRate; }
    int    getBlockSize()  const noexcept { return currentBlockSize; }

    // ── AudioIODeviceCallback ─────────────────────────────────────────────
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                           int   numInputChannels,
                                           float* const* outputChannelData,
                                           int   numOutputChannels,
                                           int   numSamples,
                                           const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart (juce::AudioIODevice* device) override;
    void audioDeviceStopped      () override;
    void audioDeviceError        (const juce::String& errorMessage) override;

private:
    bool tryInitialiseAsio    ();
    bool tryInitialiseDefault ();

    juce::AudioDeviceManager deviceManager;

    juce::Array<TrackBase*> tracks;
    juce::ReadWriteLock     tracksLock;

    std::atomic<bool> playing   { false };
    std::atomic<bool> recording { false };

    double currentSampleRate { 44100.0 };
    int    currentBlockSize  { 512 };

    juce::AudioBuffer<float> mixBuffer;  ///< Stereo scratch buffer, allocated once

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioEngine)
};
