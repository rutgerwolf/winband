#pragma once
#include <JuceHeader.h>
#include <memory>

struct EnergyAubioState;  // PIMPL — Aubio types only in .cpp

/**
 * EnergyAnalyzer — F5c
 *
 * Computes RMS energy from a guide-track buffer using Aubio's fvec pipeline.
 * Provides both per-block (smoothed) and per-bar energy.
 *
 * Usage:
 *   1. Call prepare() when the device starts (audioDeviceAboutToStart).
 *   2. Call setBpm() when the user changes tempo.
 *   3. Call computeRms() on every guide-track block.
 *   4. Read getSmoothedEnergy() for block-level decisions.
 *      Read getBarEnergy() + barJustCompleted() for bar-level decisions.
 */
class EnergyAnalyzer
{
public:
    EnergyAnalyzer();
    ~EnergyAnalyzer();

    /** (Re)initialise with device parameters.  Safe to call multiple times. */
    void prepare (double sampleRate, int blockSize, double bpm = 120.0);

    /** Update tempo (e.g. from TransportBar BPM input). */
    void setBpm (double bpm) noexcept;

    /**
     * Process one audio block.  Returns per-block RMS in [0, 1].
     * Also accumulates energy toward the next bar boundary.
     * @param buffer  Mono guide track (channel 0 is used).
     */
    double computeRms (const juce::AudioBuffer<float>& buffer) noexcept;

    /** IIR-smoothed block energy — use this for adaptive pattern switching. */
    double getSmoothedEnergy() const noexcept { return static_cast<double> (smoothed); }

    /** RMS averaged over the last complete bar — updated once per bar. */
    double getBarEnergy() const noexcept { return barEnergy; }

    /**
     * Returns true for exactly one call to computeRms() after a bar completes.
     * Use this to trigger fill decisions (F5e).
     */
    bool barJustCompleted() const noexcept { return barCompleted; }

    void reset() noexcept;

private:
    void updateSamplesPerBar() noexcept;

    std::unique_ptr<EnergyAubioState> aubio;

    double currentSampleRate { 44100.0 };
    int    currentBlockSize  { 512 };
    double currentBpm        { 120.0 };
    int    samplesPerBar     { 0 };

    // Bar-level accumulator
    double barSumSq    { 0.0 };
    int    barSamples  { 0 };
    double barEnergy   { 0.0 };
    bool   barCompleted { false };

    // IIR block-level energy
    float smoothed { 0.0f };
    static constexpr float kSmoothCoeff = 0.1f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnergyAnalyzer)
};
