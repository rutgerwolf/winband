#pragma once
#include <JuceHeader.h>

/**
 * TrackBase — abstract base for all track types.
 *
 * Subclasses: AudioTrack, DrummerTrack
 * (future: MidiTrack, InstrumentTrack)
 */
class TrackBase
{
public:
    explicit TrackBase (juce::String name);
    virtual ~TrackBase() = default;

    //==========================================================================
    // Identity
    const juce::String& getName()  const noexcept { return name; }
    void                setName  (const juce::String& n) { name = n; }

    //==========================================================================
    // Mix controls
    float getVolume()   const noexcept { return volume; }
    float getPan()      const noexcept { return pan; }
    bool  isMuted()     const noexcept { return muted; }
    bool  isSoloed()    const noexcept { return soloed; }

    void setVolume  (float v) { volume = juce::jlimit (0.0f, 1.0f, v); }
    void setPan     (float p) { pan    = juce::jlimit (-1.0f, 1.0f, p); }
    void setMuted   (bool m)  { muted  = m; }
    void setSoloed  (bool s)  { soloed = s; }

    //==========================================================================
    // Audio processing — implemented by each subclass
    virtual void prepareToPlay (double sampleRate, int samplesPerBlock) = 0;
    virtual void processBlock  (juce::AudioBuffer<float>& buffer,
                                juce::MidiBuffer&         midiMessages) = 0;
    virtual void releaseResources() = 0;

    //==========================================================================
    // Serialisation
    virtual juce::ValueTree toValueTree() const;
    virtual void            fromValueTree (const juce::ValueTree& tree);

protected:
    juce::String name;
    float        volume { 0.8f };
    float        pan    { 0.0f };
    bool         muted  { false };
    bool         soloed { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackBase)
};
