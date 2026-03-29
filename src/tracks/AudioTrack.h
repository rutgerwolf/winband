#pragma once
#include "TrackBase.h"

/**
 * AudioTrack — F1 (MVP: single track)
 *
 * State machine:
 *   Idle       — output silence (or input passthrough if monitoring)
 *   Recording  — capture input; optionally monitor to output
 *   PlayingBack— output recorded audio from playbackPos
 *
 * Thread safety:
 *   recording / playingBack / monitoring are atomic — safe to set from UI.
 *   recordWritePos / playbackPos are only written from the audio callback.
 */
class AudioTrack : public TrackBase
{
public:
    enum class InputMode { Mono, Stereo };

    explicit AudioTrack (const juce::String& name = "Audio 1");
    ~AudioTrack() override = default;

    // ── TrackBase ────────────────────────────────────────────────────────────
    void prepareToPlay   (double sampleRate, int samplesPerBlock) override;
    void processBlock    (juce::AudioBuffer<float>& buffer,
                          juce::MidiBuffer&         midiMessages) override;
    void releaseResources() override;

    // ── Input config ─────────────────────────────────────────────────────────
    void setInputMode      (InputMode mode)  noexcept { inputMode    = mode; }
    void setInputChannel   (int channel)     noexcept { inputChannel = channel; }
    void setMonitorEnabled (bool enable)     noexcept { monitoring   = enable; }
    bool isMonitorEnabled  ()          const noexcept { return monitoring.load(); }

    // ── Transport ────────────────────────────────────────────────────────────
    /** Begin capturing; resets write position to 0. */
    void startRecording();
    /** Stop capturing; trims the buffer to the actual recorded length. */
    void stopRecording();
    bool isRecording()   const noexcept { return recording.load(); }

    /** Start playback from the beginning of the recorded buffer. */
    void startPlayback();
    void stopPlayback();
    bool isPlayingBack() const noexcept { return playingBack.load(); }

    // ── Recorded data ────────────────────────────────────────────────────────
    /** Returns the recorded audio (valid after stopRecording). */
    const juce::AudioBuffer<float>& getRecordedAudio() const noexcept { return recordedAudio; }
    int  getRecordedSamples()                          const noexcept { return recordedSamples; }
    void clearRecording();

    // ── Serialisation ────────────────────────────────────────────────────────
    juce::ValueTree toValueTree()                        const override;
    void            fromValueTree (const juce::ValueTree& tree) override;

private:
    void applyGainAndPan (juce::AudioBuffer<float>& buf, int numSamples) const noexcept;

    InputMode inputMode    { InputMode::Mono };
    int       inputChannel { 0 };

    std::atomic<bool> recording    { false };
    std::atomic<bool> playingBack  { false };
    std::atomic<bool> monitoring   { false };

    double currentSampleRate     { 44100.0 };
    int    currentSamplesPerBlock { 512 };

    juce::AudioBuffer<float> recordedAudio;
    int recordWritePos  { 0 };   ///< written only from audio callback
    int playbackPos     { 0 };   ///< written only from audio callback
    int recordedSamples { 0 };   ///< set in stopRecording, read in processBlock

    static constexpr int kMaxRecordSeconds = 300;  ///< 5-minute pre-allocation

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioTrack)
};
