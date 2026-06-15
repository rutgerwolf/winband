#include "AudioEngine.h"
#include "../tracks/TrackBase.h"
#include "../tracks/AudioTrack.h"

AudioEngine::AudioEngine() = default;

AudioEngine::~AudioEngine()
{
    shutdown();
}

// ── Lifecycle ──────────────────────────────────────────────────────────────

bool AudioEngine::initialise()
{
    // ASIO first (lowest latency), then Windows default (WASAPI / DirectSound)
    if (tryInitialiseAsio())
    {
        juce::Logger::writeToLog ("AudioEngine: opened ASIO device");
        deviceManager.addAudioCallback (this);
        return true;
    }

    if (tryInitialiseDefault())
    {
        juce::Logger::writeToLog ("AudioEngine: opened default device (WASAPI/DS)");
        deviceManager.addAudioCallback (this);
        return true;
    }

    juce::Logger::writeToLog ("AudioEngine: no audio device could be opened");
    return false;
}

void AudioEngine::shutdown()
{
    stop();
    deviceManager.removeAudioCallback (this);
    deviceManager.closeAudioDevice();

    juce::ScopedWriteLock sl (tracksLock);
    tracks.clear();
}

bool AudioEngine::tryInitialiseAsio()
{
#if JUCE_ASIO
    deviceManager.setCurrentAudioDeviceType ("ASIO", true);

    for (auto* type : deviceManager.getAvailableDeviceTypes())
    {
        if (type->getTypeName() != "ASIO") continue;

        const auto names = type->getDeviceNames (false);  // output side
        if (names.isEmpty()) return false;

        juce::AudioDeviceManager::AudioDeviceSetup setup;
        setup.outputDeviceName         = names[0];
        setup.inputDeviceName          = names[0];   // ASIO: same device for I/O
        setup.sampleRate               = 44100.0;
        setup.bufferSize               = 256;
        setup.useDefaultInputChannels  = true;
        setup.useDefaultOutputChannels = true;

        const auto err = deviceManager.setAudioDeviceSetup (setup, true);
        if (err.isEmpty())
            return true;

        juce::Logger::writeToLog ("AudioEngine: ASIO setup failed: " + err);
        return false;
    }
#endif
    return false;
}

bool AudioEngine::tryInitialiseDefault()
{
    const auto err = deviceManager.initialiseWithDefaultDevices (2, 2);
    if (err.isNotEmpty())
        juce::Logger::writeToLog ("AudioEngine: default device failed: " + err);
    return err.isEmpty();
}

// ── Track management ───────────────────────────────────────────────────────

void AudioEngine::addTrack (TrackBase* track)
{
    jassert (track != nullptr);
    juce::ScopedWriteLock sl (tracksLock);
    tracks.addIfNotAlreadyThere (track);
}

void AudioEngine::removeTrack (TrackBase* track)
{
    juce::ScopedWriteLock sl (tracksLock);
    tracks.removeFirstMatchingValue (track);
}

void AudioEngine::clearTracks()
{
    juce::ScopedWriteLock sl (tracksLock);
    tracks.clear();
}

// ── Transport ──────────────────────────────────────────────────────────────

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

// ── AudioIODeviceCallback ──────────────────────────────────────────────────

void AudioEngine::audioDeviceAboutToStart (juce::AudioIODevice* device)
{
    currentSampleRate = device->getCurrentSampleRate();
    currentBlockSize  = device->getCurrentBufferSizeSamples();

    // Pre-allocate stereo mix buffer
    mixBuffer.setSize (2, currentBlockSize, false, true);

    juce::ScopedReadLock sl (tracksLock);
    for (auto* track : tracks)
        track->prepareToPlay (currentSampleRate, currentBlockSize);
}

void AudioEngine::audioDeviceStopped()
{
    juce::ScopedReadLock sl (tracksLock);
    for (auto* track : tracks)
        track->releaseResources();
}

void AudioEngine::audioDeviceError (const juce::String& errorMessage)
{
    // Called on an arbitrary thread — only log, never touch UI here
    juce::Logger::writeToLog ("AudioEngine error: " + errorMessage);
}

void AudioEngine::audioDeviceIOCallbackWithContext (
    const float* const* inputChannelData,
    int                 numInputChannels,
    float* const*       outputChannelData,
    int                 numOutputChannels,
    int                 numSamples,
    const juce::AudioIODeviceCallbackContext& /*context*/)
{
    // Always silence the output first — avoids stale audio on stop
    for (int ch = 0; ch < numOutputChannels; ++ch)
        if (outputChannelData[ch] != nullptr)
            juce::FloatVectorOperations::clear (outputChannelData[ch], numSamples);

    if (! playing.load()) return;

    // Guard against late buffer size changes (should not normally happen)
    if (mixBuffer.getNumSamples() < numSamples)
        mixBuffer.setSize (2, numSamples, false, true);

    mixBuffer.clear();
    juce::MidiBuffer dummyMidi;

    {
        juce::ScopedReadLock sl (tracksLock);
        for (auto* track : tracks)
        {
            if (track->isMuted()) continue;

            juce::AudioBuffer<float> trackBuffer (2, numSamples);
            trackBuffer.clear();

            // AudioTracks need input data so processBlock can record it
            // and optionally monitor it.
            if (dynamic_cast<AudioTrack*> (track) != nullptr)
            {
                const int chsToCopy = juce::jmin (2, numInputChannels);
                for (int ch = 0; ch < chsToCopy; ++ch)
                    if (inputChannelData[ch] != nullptr)
                        trackBuffer.copyFrom (ch, 0, inputChannelData[ch], numSamples);
            }

            track->processBlock (trackBuffer, dummyMidi);

            // Accumulate into stereo mix
            for (int ch = 0; ch < 2; ++ch)
                mixBuffer.addFrom (ch, 0, trackBuffer, ch, 0, numSamples);
        }
    }

    // Copy mix to every output channel
    for (int ch = 0; ch < numOutputChannels; ++ch)
        if (outputChannelData[ch] != nullptr)
            juce::FloatVectorOperations::copy (
                outputChannelData[ch],
                mixBuffer.getReadPointer (ch % 2),
                numSamples);
}
