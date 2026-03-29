#include "DrummerTrack.h"

DrummerTrack::DrummerTrack (const juce::String& trackName)
    : TrackBase (trackName)
{}

void DrummerTrack::prepareToPlay (double newSampleRate, int /*samplesPerBlock*/)
{
    sampleRate = newSampleRate;
    drummer.reset();
}

void DrummerTrack::processBlock (juce::AudioBuffer<float>& buffer,
                                 juce::MidiBuffer& /*midiMessages*/)
{
    if (muted) return;

    // Analyze the latest guide buffer (if any)
    {
        juce::ScopedLock sl (guideLock);
        if (guideBlock.getNumSamples() > 0)
            drummer.analyzeGuideBuffer (guideBlock, sampleRate);
    }

    drummer.processBlock (buffer, buffer.getNumSamples(), sampleRate);
    buffer.applyGain (volume);
}

void DrummerTrack::releaseResources()
{
    drummer.reset();
}

void DrummerTrack::setGuideBuffer (const juce::AudioBuffer<float>& guide)
{
    juce::ScopedLock sl (guideLock);
    guideBlock = guide;
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
