#pragma once
#include "TrackBase.h"
#include "../drummer/AdaptiveDrummer.h"

/**
 * DrummerTrack — wraps AdaptiveDrummer as a first-class track.
 *
 * Visually distinct from AudioTrack in the UI (see TrackView).
 * Output is mixed into the master bus via processBlock().
 */
class DrummerTrack : public TrackBase
{
public:
    explicit DrummerTrack (const juce::String& name = "Drummer");
    ~DrummerTrack() override = default;

    //==========================================================================
    void prepareToPlay  (double sampleRate, int samplesPerBlock) override;
    void processBlock   (juce::AudioBuffer<float>& buffer,
                         juce::MidiBuffer&         midiMessages) override;
    void releaseResources() override;

    //==========================================================================
    AdaptiveDrummer& getDrummer() noexcept { return drummer; }

    void setStyle   (DrumStyle style) { drummer.setStyle (style); }
    void setBpm     (double bpm)      { drummer.setBpm (bpm); }
    void tapTempo   ()                { drummer.setTapTempo(); }

    /** Feed a guide-track buffer for energy analysis. */
    void setGuideBuffer (const juce::AudioBuffer<float>& guideBuffer);

    juce::ValueTree toValueTree()                       const override;
    void            fromValueTree (const juce::ValueTree& tree) override;

private:
    AdaptiveDrummer drummer;
    double          sampleRate { 44100.0 };

    // Holds the most recent guide buffer reference (set from UI thread)
    juce::AudioBuffer<float> guideBlock;
    juce::CriticalSection    guideLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrummerTrack)
};
