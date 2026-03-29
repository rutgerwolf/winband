#pragma once
#include "TrackBase.h"
#include "../drummer/AdaptiveDrummer.h"

/**
 * DrummerTrack — wraps AdaptiveDrummer as a first-class track.
 *
 * Visually distinct in the UI (isDrummerTrack = true in TrackView).
 * Output is mixed into the master bus via processBlock().
 */
class DrummerTrack : public TrackBase
{
public:
    explicit DrummerTrack (const juce::String& name = "Drummer");
    ~DrummerTrack() override = default;

    // ── TrackBase ────────────────────────────────────────────────────────────
    void prepareToPlay   (double sampleRate, int samplesPerBlock) override;
    void processBlock    (juce::AudioBuffer<float>& buffer,
                          juce::MidiBuffer&         midiMessages) override;
    void releaseResources() override;

    // ── Controls ────────────────────────────────────────────────────────────
    AdaptiveDrummer& getDrummer() noexcept { return drummer; }

    void   setStyle (DrumStyle style)  { drummer.setStyle (style); }
    void   setBpm   (double bpm)       { drummer.setBpm (bpm); }
    double getBpm   () const noexcept  { return drummer.getBpm(); }
    void   tapTempo ()                 { drummer.setTapTempo(); }

    bool loadSamples (const juce::File& salamanderRoot)
        { return drummer.loadSamples (salamanderRoot); }

    /**
     * Provide a guide track block for energy analysis.
     * Safe to call from the audio thread immediately before processBlock().
     */
    void feedGuideBuffer (const juce::AudioBuffer<float>& guide);

    // ── Serialisation ────────────────────────────────────────────────────────
    juce::ValueTree toValueTree()                        const override;
    void            fromValueTree (const juce::ValueTree& tree) override;

private:
    AdaptiveDrummer drummer;
    double          currentSampleRate { 44100.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrummerTrack)
};
