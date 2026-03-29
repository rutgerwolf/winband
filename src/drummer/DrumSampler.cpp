#include "DrumSampler.h"
#include <array>

DrumSampler::DrumSampler()
{
    formatManager.registerBasicFormats();
}

bool DrumSampler::loadSamples (const juce::File& root)
{
    // Expected filenames per voice
    const std::array<const char*, kNumVoices> names {
        "kick/kick.wav",
        "snare/snare.wav",
        "hihat/hihat-closed.wav",
        "crash/crash.wav",
        "ride/ride.wav",
        "tom/tom-mid.wav"
    };

    samplesLoaded = true;
    for (int i = 0; i < kNumVoices; ++i)
    {
        const auto file = root.getChildFile (names[i]);
        if (file.existsAsFile())
        {
            if (! loadWav (file, voices[i].sample))
            {
                DBG ("DrumSampler: failed to load " + file.getFullPathName());
                samplesLoaded = false;
            }
        }
        else
        {
            DBG ("DrumSampler: sample not found: " + file.getFullPathName());
            // Non-fatal — voice will simply be silent
        }
    }
    return samplesLoaded;
}

void DrumSampler::processBlock (juce::AudioBuffer<float>& outBuffer,
                                int                       numSamples,
                                double                    sampleRate,
                                const DrumPattern&        pattern,
                                double                    bpm,
                                int                       playheadSample)
{
    const int patternLen = pattern.getLengthInSamples (bpm, sampleRate);
    if (patternLen <= 0) return;

    // Samples per 16th note
    const int stepLen = patternLen / DrumPattern::kSteps;
    if (stepLen <= 0) return;

    // Trigger voices that land within this block
    for (int offset = 0; offset < numSamples; ++offset)
    {
        const int absPos  = playheadSample + offset;
        const int stepPos = absPos % patternLen;

        if (stepPos % stepLen == 0)  // start of a 16th-note step
        {
            const int   step = stepPos / stepLen;
            const uint8_t mask = pattern.getStep (step);

            if (mask & DrumPattern::Kick)  triggerVoice (0);
            if (mask & DrumPattern::Snare) triggerVoice (1);
            if (mask & DrumPattern::HiHat) triggerVoice (2);
            if (mask & DrumPattern::Crash) triggerVoice (3);
            if (mask & DrumPattern::Ride)  triggerVoice (4);
            if (mask & DrumPattern::Tom)   triggerVoice (5);
        }
    }

    mixVoices (outBuffer, numSamples);
}

void DrumSampler::reset()
{
    for (auto& v : voices)
        v.playPos = -1;
}

// ── private ──────────────────────────────────────────────────────────────────

bool DrumSampler::loadWav (const juce::File& file, juce::AudioBuffer<float>& dest)
{
    std::unique_ptr<juce::AudioFormatReader> reader (
        formatManager.createReaderFor (file));

    if (reader == nullptr) return false;

    dest.setSize (static_cast<int> (reader->numChannels),
                  static_cast<int> (reader->lengthInSamples));
    reader->read (&dest, 0, static_cast<int> (reader->lengthInSamples), 0, true, true);
    return true;
}

void DrumSampler::triggerVoice (int voiceIndex)
{
    voices[voiceIndex].playPos = 0;
}

void DrumSampler::mixVoices (juce::AudioBuffer<float>& outBuffer, int numSamples)
{
    for (auto& v : voices)
    {
        if (v.playPos < 0 || v.sample.getNumSamples() == 0) continue;

        const int available = v.sample.getNumSamples() - v.playPos;
        const int toCopy    = juce::jmin (numSamples, available);

        for (int ch = 0; ch < outBuffer.getNumChannels(); ++ch)
        {
            const int srcCh = juce::jmin (ch, v.sample.getNumChannels() - 1);
            outBuffer.addFrom (ch, 0,
                               v.sample, srcCh, v.playPos, toCopy);
        }

        v.playPos += toCopy;
        if (v.playPos >= v.sample.getNumSamples())
            v.playPos = -1;  // voice finished
    }
}
