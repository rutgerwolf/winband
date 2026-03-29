#include "AdaptiveDrummer.h"

AdaptiveDrummer::AdaptiveDrummer()
{
    drumPattern.loadStyle (currentStyle);
}

void AdaptiveDrummer::setStyle (DrumStyle style)
{
    currentStyle = style;
    drumPattern.loadStyle (style);
}

void AdaptiveDrummer::setBpm (double newBpm)
{
    bpm = juce::jlimit (40.0, 240.0, newBpm);
}

void AdaptiveDrummer::setTapTempo()
{
    const auto now = juce::Time::currentTimeMillis();
    if (lastTapTime > 0)
    {
        const double interval = static_cast<double> (now - lastTapTime);
        if (interval > 200.0 && interval < 3000.0)   // ignore outliers
        {
            tapAccumBpm += (60000.0 / interval);
            ++tapCount;
            setBpm (tapAccumBpm / tapCount);
        }
        else
        {
            tapCount    = 0;
            tapAccumBpm = 0.0;
        }
    }
    lastTapTime = now;
}

void AdaptiveDrummer::analyzeGuideBuffer (const juce::AudioBuffer<float>& buffer,
                                          double sampleRate)
{
    currentEnergy = energyAnalyzer.computeRms (buffer);
    scheduleFill();
}

void AdaptiveDrummer::processBlock (juce::AudioBuffer<float>& outBuffer,
                                    int numSamples,
                                    double sampleRate)
{
    selectPattern();
    drumSampler.processBlock (outBuffer, numSamples, sampleRate,
                              drumPattern, bpm, playheadSample);

    const int patternLengthSamples = drumPattern.getLengthInSamples (bpm, sampleRate);
    playheadSample = (playheadSample + numSamples) % juce::jmax (1, patternLengthSamples);
}

void AdaptiveDrummer::reset()
{
    playheadSample = 0;
    fillPending    = false;
    energyAnalyzer.reset();
    drumSampler.reset();
}

// ── private ──────────────────────────────────────────────────────────────────

void AdaptiveDrummer::selectPattern()
{
    // Energy thresholds: 0.0 – quiet, 1.0 – very loud
    if (currentEnergy < 0.2f)
        drumPattern.setDensity (DrumPattern::Density::Sparse);
    else if (currentEnergy < 0.6f)
        drumPattern.setDensity (DrumPattern::Density::Medium);
    else
        drumPattern.setDensity (DrumPattern::Density::Full);
}

void AdaptiveDrummer::scheduleFill()
{
    // Simple heuristic: trigger fill when energy jumps by > 0.3 in one block
    static float prevEnergy = 0.0f;
    if ((currentEnergy - prevEnergy) > 0.3)
        fillPending = true;
    prevEnergy = static_cast<float> (currentEnergy);
}
