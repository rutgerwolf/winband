#include "AdaptiveDrummer.h"

AdaptiveDrummer::AdaptiveDrummer()
{
    drumPattern.loadStyle (currentStyle);
}

void AdaptiveDrummer::prepare (double sampleRate, int blockSize)
{
    energyAnalyzer.prepare (sampleRate, blockSize, bpm);
    drumSampler.prepare (sampleRate);
    reset();
}

void AdaptiveDrummer::setStyle (DrumStyle style)
{
    currentStyle = style;
    drumPattern.loadStyle (style);
}

void AdaptiveDrummer::setBpm (double newBpm)
{
    bpm = juce::jlimit (40.0, 240.0, newBpm);
    energyAnalyzer.setBpm (bpm);
}

void AdaptiveDrummer::setTapTempo()
{
    const auto now = juce::Time::currentTimeMillis();
    if (lastTapTime > 0)
    {
        const double interval = static_cast<double> (now - lastTapTime);
        if (interval > 200.0 && interval < 3000.0)
        {
            tapAccumBpm += (60000.0 / interval);
            ++tapCount;
            setBpm (tapAccumBpm / tapCount);
        }
        else
        {
            // Long gap — start fresh
            tapCount    = 1;
            tapAccumBpm = 60000.0 / interval;
        }
    }
    lastTapTime = now;
}

bool AdaptiveDrummer::loadSamples (const juce::File& salamanderRoot)
{
    return drumSampler.loadSamples (salamanderRoot);
}

void AdaptiveDrummer::analyzeGuideBuffer (const juce::AudioBuffer<float>& buffer)
{
    currentEnergy = energyAnalyzer.computeRms (buffer);
    checkForFill();
}

void AdaptiveDrummer::processBlock (juce::AudioBuffer<float>& outBuffer,
                                    int numSamples, double sampleRate)
{
    selectPattern();
    drumSampler.processBlock (outBuffer, numSamples, sampleRate,
                              drumPattern, bpm, playheadSample);

    const int patternLen = drumPattern.getLengthInSamples (bpm, sampleRate);
    playheadSample = (playheadSample + numSamples) % juce::jmax (1, patternLen);
}

void AdaptiveDrummer::reset()
{
    playheadSample = 0;
    fillPending    = false;
    prevEnergy     = 0.0f;
    currentEnergy  = 0.0;
    energyAnalyzer.reset();
    drumSampler.reset();
}

// ── private ──────────────────────────────────────────────────────────────────

void AdaptiveDrummer::selectPattern()
{
    // Map smoothed energy to density tier
    if (currentEnergy < 0.2)
        drumPattern.setDensity (DrumPattern::Density::Sparse);
    else if (currentEnergy < 0.6)
        drumPattern.setDensity (DrumPattern::Density::Medium);
    else
        drumPattern.setDensity (DrumPattern::Density::Full);

    // Insert fill if pending and a bar boundary was just crossed (F5e)
    if (fillPending && energyAnalyzer.barJustCompleted())
    {
        fillPending = false;
        // TODO (F5e): swap drumPattern to a fill variant for one bar
    }
}

void AdaptiveDrummer::checkForFill()
{
    // Schedule a fill when energy spikes by more than 0.3 in one block
    const float delta = static_cast<float> (currentEnergy) - prevEnergy;
    if (delta > 0.3f)
        fillPending = true;
    prevEnergy = static_cast<float> (currentEnergy);
}
