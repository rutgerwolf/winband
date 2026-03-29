#pragma once
#include <JuceHeader.h>

/**
 * Central audio engine.
 *
 * Responsibilities:
 *   - Opens and manages the audio device (ASIO preferred, WASAPI fallback)
 *   - Drives the AudioProcessorGraph for all active tracks
 *   - Exposes transport state (play / stop / record)
 *
 * MVP scope: supports a single audio input track + the drummer track.
 */
class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    AudioEngine();
    ~AudioEngine() override;

    // Device management
    bool initialise();   ///< Open preferred device; returns false on failure
    void shutdown();

    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }

    // Transport
    void play();
    void stop();
    void record();

    bool isPlaying()  const { return playing; }
    bool isRecording() const { return recording; }

    // AudioIODeviceCallback
    void audioDeviceIOCallbackWithContext(const float* const* inputChannelData,
                                          int numInputChannels,
                                          float* const* outputChannelData,
                                          int numOutputChannels,
                                          int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    juce::AudioDeviceManager deviceManager;

    std::atomic<bool> playing   { false };
    std::atomic<bool> recording { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
