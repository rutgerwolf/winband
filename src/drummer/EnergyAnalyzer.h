#pragma once
#include <JuceHeader.h>

// Forward-declare Aubio types to avoid including aubio.h in every TU.
extern "C" {
    typedef struct _aubio_pvoc_t   aubio_pvoc_t;
    typedef struct _fvec_t         fvec_t;
    fvec_t*  new_fvec  (unsigned int length);
    void     del_fvec  (fvec_t* s);
    float    fvec_mean (fvec_t* s);
}

/**
 * EnergyAnalyzer — F5c
 *
 * Computes per-block RMS energy from a mono guide-track buffer.
 * Uses Aubio's fvec utilities under the hood; the full Aubio onset/tempo
 * pipeline can be wired in here for beat-synchronised analysis.
 */
class EnergyAnalyzer
{
public:
    EnergyAnalyzer()  = default;
    ~EnergyAnalyzer() = default;

    /**
     * Returns normalised RMS energy in [0, 1] for the supplied buffer.
     * @param buffer  Mono (or first-channel) audio block.
     */
    double computeRms (const juce::AudioBuffer<float>& buffer) noexcept;

    /** Running average energy over the last N blocks (smoothing). */
    double getSmoothedEnergy() const noexcept { return smoothed; }

    void reset() noexcept;

private:
    static constexpr float kSmoothCoeff = 0.1f;  ///< IIR smoothing coefficient
    float smoothed { 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnergyAnalyzer)
};
