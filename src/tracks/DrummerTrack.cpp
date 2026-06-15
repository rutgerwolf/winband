#include "DrummerTrack.h"

DrummerTrack::DrummerTrack (const juce::String& trackName)
    : TrackBase (trackName)
{}

void DrummerTrack::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    drummer.prepare (sampleRate, samplesPerBlock);
}

void DrummerTrack::processBlock (juce::AudioBuffer<float>& buffer,
                                 juce::MidiBuffer& /*midiMessages*/)
{
    if (isMuted()) return;

    drummer.processBlock (buffer, buffer.getNumSamples(), currentSampleRate);
    buffer.applyGain (volume);
}

void DrummerTrack::releaseResources()
{
    drummer.reset();
}

void DrummerTrack::feedGuideBuffer (const juce::AudioBuffer<float>& guide)
{
    // Called from the audio thread — no lock needed; drummer is audio-thread-only
    drummer.analyzeGuideBuffer (guide);
}

juce::ValueTree DrummerTrack::toValueTree() const
{
    auto tree = TrackBase::toValueTree();
    tree.setProperty ("type", "drummer", nullptr);
    return tree;
}

void DrummerTrack::fromValueTree (const juce::ValueTree& tree)
{
    TrackBase::fromValueTree (tree);
}
