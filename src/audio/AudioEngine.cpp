#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
    // Initialisation deferred to initialise() so the UI can start first
}

AudioEngine::~AudioEngine()
{
    shutdown();
}

bool AudioEngine::initialise()
{
    // Request stereo output, stereo input, default sample rate + buffer size.
    // JUCE will prefer ASIO if JUCE_ASIO=1 is set and a driver is available.
    auto result = deviceManager.initialiseWithDefaultDevices(2, 2);
    if (result.isNotEmpty())
    {
        // Initialisation failed; caller should show an error or fall back
        juce::Logger::writeToLog("AudioEngine: device init failed: " + result);
        return false;
    }

    deviceManager.addAudioCallback(this);
    return true;
}

void AudioEngine::shutdown()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
}

void AudioEngine::play()
{
    recording = false;
    playing   = true;
}

void AudioEngine::stop()
{
    playing   = false;
    recording = false;
}

void AudioEngine::record()
{
    playing   = true;
    recording = true;
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* /*device*/)
{
    // TODO: prepare all tracks with current sample rate + block size
}

void AudioEngine::audioDeviceStopped()
{
    // TODO: flush record buffers
}

void AudioEngine::audioDeviceIOCallbackWithContext(
    const float* const* inputChannelData,
    int /*numInputChannels*/,
    float* const* outputChannelData,
    int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext& /*context*/)
{
    // Silence output — mixing will be implemented in later phases
    for (int ch = 0; ch < numOutputChannels; ++ch)
        if (outputChannelData[ch] != nullptr)
            juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);

    juce::ignoreUnused(inputChannelData);
}
