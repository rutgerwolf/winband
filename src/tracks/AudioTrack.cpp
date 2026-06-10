#include "AudioTrack.h"

AudioTrack::AudioTrack (const juce::String& trackName)
    : TrackBase (trackName)
{}

// ── TrackBase ─────────────────────────────────────────────────────────────────

void AudioTrack::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate      = sampleRate;
    currentSamplesPerBlock = samplesPerBlock;

    const int numChannels = (inputMode == InputMode::Stereo) ? 2 : 1;
    const int maxSamples  = static_cast<int> (sampleRate * kMaxRecordSeconds);

    // Preserve any already-recorded audio when the device restarts
    recordedAudio.setSize (numChannels, maxSamples, /*keepExisting=*/true, /*clearExtra=*/true);
}

void AudioTrack::processBlock (juce::AudioBuffer<float>& buffer,
                               juce::MidiBuffer& /*midiMessages*/)
{
    if (isMuted())
    {
        buffer.clear();
        return;
    }

    const int numSamples = buffer.getNumSamples();

    // ── 1. Record from selected input channel ────────────────────────────────
    if (recording.load())
    {
        const int space  = recordedAudio.getNumSamples() - recordWritePos;
        const int toCopy = juce::jmin (numSamples, space);

        if (toCopy > 0)
        {
            const int numRecChans = recordedAudio.getNumChannels();
            for (int destCh = 0; destCh < numRecChans; ++destCh)
            {
                // Mono: always read from inputChannel.
                // Stereo: read left/right from consecutive device channels.
                const int srcCh = (inputMode == InputMode::Mono)
                    ? juce::jmin (inputChannel, buffer.getNumChannels() - 1)
                    : juce::jmin (destCh,       buffer.getNumChannels() - 1);

                recordedAudio.copyFrom (destCh, recordWritePos,
                                        buffer, srcCh, 0, toCopy);
            }
            recordWritePos += toCopy;
        }
        else
        {
            // Pre-allocated buffer is full — stop recording silently
            recording = false;
        }
    }

    // ── 2. Fill output ───────────────────────────────────────────────────────
    if (playingBack.load() && recordedSamples > 0)
    {
        const int available = recordedSamples - playbackPos;
        const int toPlay    = juce::jmin (numSamples, available);

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            const int srcCh = juce::jmin (ch, recordedAudio.getNumChannels() - 1);

            if (toPlay > 0)
                buffer.copyFrom (ch, 0, recordedAudio, srcCh, playbackPos, toPlay);

            // Silence any remainder after the recording ends
            if (toPlay < numSamples)
                buffer.clear (ch, toPlay, numSamples - toPlay);
        }

        playbackPos += toPlay;

        if (playbackPos >= recordedSamples)
        {
            playbackPos = 0;
            playingBack = false;   // one-shot: stop at end
        }
    }
    else if (! monitoring.load())
    {
        // Nothing to play back and monitoring off — silence the output
        buffer.clear();
    }
    // else: monitoring on — input data already in buffer, pass through

    // ── 3. Volume + pan ──────────────────────────────────────────────────────
    applyGainAndPan (buffer, numSamples);
}

void AudioTrack::releaseResources()
{
    recording   = false;
    playingBack = false;
}

// ── Transport ────────────────────────────────────────────────────────────────

void AudioTrack::startRecording()
{
    // Reset counters before setting the flag so the audio callback
    // never observes a stale position alongside recording=true.
    recordWritePos  = 0;
    recordedSamples = 0;
    recording       = true;
}

void AudioTrack::stopRecording()
{
    recording       = false;
    recordedSamples = recordWritePos;

    if (recordedSamples > 0)
        recordedAudio.setSize (recordedAudio.getNumChannels(), recordedSamples,
                               /*keepExisting=*/true);
}

void AudioTrack::startPlayback()
{
    playbackPos = 0;
    playingBack = true;
}

void AudioTrack::stopPlayback()
{
    playingBack = false;
    playbackPos = 0;
}

void AudioTrack::clearRecording()
{
    recording       = false;
    playingBack     = false;
    recordWritePos  = 0;
    recordedSamples = 0;
    playbackPos     = 0;
    recordedAudio.clear();
}

bool AudioTrack::loadFromFile (const juce::File& file)
{
    juce::AudioFormatManager fmt;
    fmt.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader (fmt.createReaderFor (file));
    if (reader == nullptr) return false;

    const int numCh  = static_cast<int> (reader->numChannels);
    const int numSmp = static_cast<int> (reader->lengthInSamples);
    if (numSmp <= 0) return false;

    // Stop any in-progress recording / playback before touching the buffer
    recording   = false;
    playingBack = false;

    recordedAudio.setSize (numCh, numSmp, false, true);
    reader->read (&recordedAudio, 0, numSmp, 0, true, true);

    recordedSamples = numSmp;
    recordWritePos  = numSmp;
    playbackPos     = 0;

    return true;
}

// ── Serialisation ────────────────────────────────────────────────────────────

juce::ValueTree AudioTrack::toValueTree() const
{
    auto tree = TrackBase::toValueTree();
    tree.setProperty ("type",         "audio",           nullptr);
    tree.setProperty ("inputMode",    (int) inputMode,   nullptr);
    tree.setProperty ("inputChannel", inputChannel,      nullptr);
    tree.setProperty ("monitoring",   monitoring.load(), nullptr);
    return tree;
}

void AudioTrack::fromValueTree (const juce::ValueTree& tree)
{
    TrackBase::fromValueTree (tree);
    inputMode    = static_cast<InputMode> ((int) tree.getProperty ("inputMode",    0));
    inputChannel = tree.getProperty ("inputChannel", 0);
    monitoring   = static_cast<bool> (tree.getProperty ("monitoring", false));
}

// ── Private ──────────────────────────────────────────────────────────────────

void AudioTrack::applyGainAndPan (juce::AudioBuffer<float>& buf,
                                  int numSamples) const noexcept
{
    buf.applyGain (volume);

    if (buf.getNumChannels() == 2 && pan != 0.0f)
    {
        const float leftGain  = (pan <= 0.0f) ? 1.0f : (1.0f - pan);
        const float rightGain = (pan >= 0.0f) ? 1.0f : (1.0f + pan);
        buf.applyGain (0, 0, numSamples, leftGain);
        buf.applyGain (1, 0, numSamples, rightGain);
    }
}
