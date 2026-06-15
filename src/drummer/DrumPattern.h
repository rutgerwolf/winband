#pragma once
#include <JuceHeader.h>

// Forward declaration from AdaptiveDrummer.h
enum class DrumStyle;

/**
 * DrumPattern — holds the current beat grid for a given style and density.
 *
 * A pattern is a fixed-length sequence of 16th-note slots (default: 1 bar,
 * 16 slots).  Each slot contains a bitmask of drum voices that should fire.
 */
class DrumPattern
{
public:
    /** Coarse energy levels that select a sub-pattern. */
    enum class Density { Sparse, Medium, Full };

    /** Drum voice indices (bitmask bits). */
    enum Voice : uint8_t
    {
        Kick   = 1 << 0,
        Snare  = 1 << 1,
        HiHat  = 1 << 2,
        Crash  = 1 << 3,
        Ride   = 1 << 4,
        Tom    = 1 << 5,
    };

    static constexpr int kSteps = 16;  ///< Steps per bar (16th-note resolution)

    DrumPattern();

    void loadStyle  (DrumStyle style);
    void setDensity (Density density);

    /** Returns the voice bitmask for the given 16th-note step (0–15). */
    uint8_t getStep (int stepIndex) const;

    /** Pattern length in samples for the current BPM and sample rate. */
    int getLengthInSamples (double bpm, double sampleRate) const;

private:
    void buildRock();
    void buildJazz();
    void buildFunk();
    void buildElectronic();
    void buildBrushes();

    // Three density layers per style [Sparse][Medium][Full]
    uint8_t layers[3][kSteps] {};
    Density currentDensity { Density::Medium };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrumPattern)
};
