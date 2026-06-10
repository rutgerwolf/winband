#include "EnergyAnalyzer.h"
#include <cmath>

// Include Aubio if the submodule is present.  If not, the class falls back
// to a pure C++ implementation so the project can be built immediately.
#if __has_include(<aubio/aubio.h>)
  #include <aubio/aubio.h>
  #define WINBAND_HAVE_AUBIO 1
#else
  #define WINBAND_HAVE_AUBIO 0
#endif

// ── Aubio PIMPL ────────────────────────────────────────────────────────────────

struct EnergyAubioState
{
#if WINBAND_HAVE_AUBIO
    fvec_t* input  { nullptr };
    fvec_t* output { nullptr };  // reserved for future onset detection

    explicit EnergyAubioState (unsigned int hopSize)
    {
        input  = new_fvec (hopSize);
        output = new_fvec (1u);
    }

    ~EnergyAubioState()
    {
        if (output) del_fvec (output);
        if (input)  del_fvec (input);
    }

    /** Copy JUCE buffer (ch 0) into Aubio fvec. */
    void fill (const juce::AudioBuffer<float>& buffer) noexcept
    {
        const int n = juce::jmin (static_cast<int> (input->length),
                                   buffer.getNumSamples());
        const float* src = buffer.getReadPointer (0);
        for (int i = 0; i < n; ++i)
            input->data[i] = src[i];
        // Zero-pad if JUCE block is shorter than hop size
        for (int i = n; i < static_cast<int> (input->length); ++i)
            input->data[i] = 0.0f;
    }

    /** Sum of squares of the filled fvec (used for RMS). */
    float sumOfSquares() const noexcept
    {
        float s = 0.0f;
        for (unsigned int i = 0; i < input->length; ++i)
            s += input->data[i] * input->data[i];
        return s;
    }
#else
    explicit EnergyAubioState (unsigned int) {}
    ~EnergyAubioState() = default;
    void  fill (const juce::AudioBuffer<float>&) noexcept {}
    float sumOfSquares() const noexcept { return 0.0f; }
#endif
};

// ── EnergyAnalyzer ─────────────────────────────────────────────────────────────

EnergyAnalyzer::EnergyAnalyzer()
    : aubio (std::make_unique<EnergyAubioState> (512u))
{
    updateSamplesPerBar();
}

EnergyAnalyzer::~EnergyAnalyzer() = default;

void EnergyAnalyzer::prepare (double sampleRate, int blockSize, double bpm)
{
    currentSampleRate = sampleRate;
    currentBlockSize  = blockSize;
    currentBpm        = bpm;
    updateSamplesPerBar();

    // Rebuild Aubio state with the correct hop size
    aubio = std::make_unique<EnergyAubioState> (static_cast<unsigned int> (blockSize));
    reset();
}

void EnergyAnalyzer::setBpm (double bpm) noexcept
{
    currentBpm = juce::jlimit (40.0, 240.0, bpm);
    updateSamplesPerBar();
}

double EnergyAnalyzer::computeRms (const juce::AudioBuffer<float>& buffer) noexcept
{
    barCompleted = false;

    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0)
        return static_cast<double> (smoothed);

    const int numSamples = buffer.getNumSamples();
    float sumSq = 0.0f;

#if WINBAND_HAVE_AUBIO
    // Route through Aubio fvec — allows future onset/tempo analysis here
    aubio->fill (buffer);
    sumSq = aubio->sumOfSquares();
#else
    // Pure C++ fallback (identical result)
    const float* data = buffer.getReadPointer (0);
    for (int i = 0; i < numSamples; ++i)
        sumSq += data[i] * data[i];
#endif

    // Per-block RMS → IIR smoothed energy
    const float rms     = std::sqrt (sumSq / static_cast<float> (juce::jmax (1, numSamples)));
    const float clamped = juce::jlimit (0.0f, 1.0f, rms);
    smoothed += kSmoothCoeff * (clamped - smoothed);

    // Accumulate toward the bar boundary
    barSumSq  += static_cast<double> (sumSq);
    barSamples += numSamples;

    if (samplesPerBar > 0 && barSamples >= samplesPerBar)
    {
        const double barRms = std::sqrt (barSumSq / static_cast<double> (juce::jmax (1, barSamples)));
        barEnergy    = juce::jlimit (0.0, 1.0, barRms);
        barCompleted = true;

        // Reset accumulator; keep any overflow into the next bar
        const int overflow = barSamples - samplesPerBar;
        barSumSq   = 0.0;
        barSamples = juce::jmax (0, overflow);
    }

    return static_cast<double> (smoothed);
}

void EnergyAnalyzer::reset() noexcept
{
    smoothed     = 0.0f;
    barSumSq     = 0.0;
    barSamples   = 0;
    barEnergy    = 0.0;
    barCompleted = false;
}

void EnergyAnalyzer::updateSamplesPerBar() noexcept
{
    if (currentBpm > 0.0 && currentSampleRate > 0.0)
        samplesPerBar = static_cast<int> ((60.0 / currentBpm) * 4.0 * currentSampleRate);
    else
        samplesPerBar = 0;
}
