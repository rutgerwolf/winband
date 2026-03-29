#include "AudioTrack.h"

AudioTrack::AudioTrack (const juce::String& trackName)
    : TrackBase (trackName)
{}

void AudioTrack::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate     = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;

    const int maxSamples = static_cast<int> (sampleRate * kMaxRecordSeconds);
    recordedAudio.setSize (inputMode == InputMode::Stereo ? 2 : 1, maxSamples, false, true);
    recordWritePos = 0;
}

void AudioTrack::processBlock (juce::AudioBuffer<float>& buffer,
                               juce::MidiBuffer& /*midiMessages*/)
{
    if (muted) return;

    const int numSamples = buffer.getNumSamples();

    // Record incoming audio
    if (recording)
    {
        const int space = recordedAudio.getNumSamples() - recordWritePos;
        const int toCopy = juce::jmin (numSamples, space);
        if (toCopy > 0)
        {
            for (int ch = 0; ch < recordedAudio.getNumChannels(); ++ch)
            {
                const int srcCh = juce::jmin (ch, buffer.getNumChannels() - 1);
                recordedAudio.copyFrom (ch, recordWritePos,
                                        buffer, srcCh, 0, toCopy);
            }
            recordWritePos += toCopy;
        }
    }

    // Apply volume + pan
    buffer.applyGain (volume);

    if (buffer.getNumChannels() == 2)
    {
        const float leftGain  = (pan <= 0.0f) ? 1.0f : (1.0f - pan);
        const float rightGain = (pan >= 0.0f) ? 1.0f : (1.0f + pan);
        buffer.applyGain (0, 0, numSamples, leftGain);
        buffer.applyGain (1, 0, numSamples, rightGain);
    }
}

void AudioTrack::releaseResources()
{
    recording = false;
}

void AudioTrack::startRecording()
{
    recordWritePos = 0;
    recording      = true;
}

void AudioTrack::stopRecording()
{
    recording = false;
    // Trim buffer to actual recorded length
    recordedAudio.setSize (recordedAudio.getNumChannels(), recordWritePos, true);
}

void AudioTrack::clearRecording()
{
    recording      = false;
    recordWritePos = 0;
    recordedAudio.clear();
}

juce::ValueTree AudioTrack::toValueTree() const
{
    auto tree = TrackBase::toValueTree();
    tree.setProperty ("type",         "audio",                    nullptr);
    tree.setProperty ("inputMode",    (int) inputMode,            nullptr);
    tree.setProperty ("inputChannel", inputChannel,               nullptr);
    tree.setProperty ("monitoring",   monitoring,                 nullptr);
    return tree;
}

void AudioTrack::fromValueTree (const juce::ValueTree& tree)
{
    TrackBase::fromValueTree (tree);
    inputMode    = static_cast<InputMode> ((int) tree.getProperty ("inputMode",    0));
    inputChannel = tree.getProperty ("inputChannel", 0);
    monitoring   = tree.getProperty ("monitoring",   false);
}
