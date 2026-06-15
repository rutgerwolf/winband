#pragma once
#include <JuceHeader.h>

/**
 * TrackBase — abstract base for all track types.
 *
 * Thread safety:
 *   muted / soloed are atomic so the UI and audio callback can access them
 *   without locks.  volume and pan are plain floats — the worst case of a
 *   torn read is a single block of slightly wrong gain, which is acceptable.
 */
class TrackBase
{
public:
    explicit TrackBase (juce::String name);
    virtual ~TrackBase() = default;

    // ── Identity ────────────────────────────────────────────────────────────
    const juce::String& getName() const noexcept { return name; }
    void                setName (const juce::String& n) { name = n; }

    // ── Mix controls ────────────────────────────────────────────────────────
    float getVolume() const noexcept { return volume; }
    float getPan()    const noexcept { return pan; }
    bool  isMuted()   const noexcept { return muted.load(); }
    bool  isSoloed()  const noexcept { return soloed.load(); }

    void setVolume (float v) noexcept { volume = juce::jlimit (0.0f, 1.0f, v); }
    void setPan    (float p) noexcept { pan    = juce::jlimit (-1.0f, 1.0f, p); }
    void setMuted  (bool m)  noexcept { muted  = m; }
    void setSoloed (bool s)  noexcept { soloed = s; }

    // ── Audio processing ────────────────────────────────────────────────────
    virtual void prepareToPlay (double sampleRate, int samplesPerBlock) = 0;
    virtual void processBlock  (juce::AudioBuffer<float>& buffer,
                                juce::MidiBuffer&         midiMessages) = 0;
    virtual void releaseResources() = 0;

    // ── Serialisation ───────────────────────────────────────────────────────
    virtual juce::ValueTree toValueTree() const;
    virtual void            fromValueTree (const juce::ValueTree& tree);

protected:
    juce::String name;
    float        volume { 0.8f };
    float        pan    { 0.0f };
    std::atomic<bool> muted  { false };
    std::atomic<bool> soloed { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackBase)
};
