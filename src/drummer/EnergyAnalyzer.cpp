#include "EnergyAnalyzer.h"
#include <cmath>

double EnergyAnalyzer::computeRms (const juce::AudioBuffer<float>& buffer) noexcept
{
    if (buffer.getNumChannels() == 0 || buffer.getNumSamples() == 0)
        return 0.0;

    // Use channel 0 (mono guide track)
    const float* data    = buffer.getReadPointer (0);
    const int    numSamp = buffer.getNumSamples();

    float sumSq = 0.0f;
    for (int i = 0; i < numSamp; ++i)
        sumSq += data[i] * data[i];

    const float rms = std::sqrt (sumSq / static_cast<float> (numSamp));

    // Clamp to [0, 1] — typical audio peaks at 1.0f
    const float clamped = juce::jlimit (0.0f, 1.0f, rms);

    // IIR smoothing
    smoothed = smoothed + kSmoothCoeff * (clamped - smoothed);

    return static_cast<double> (smoothed);
}

void EnergyAnalyzer::reset() noexcept
{
    smoothed = 0.0f;
}
