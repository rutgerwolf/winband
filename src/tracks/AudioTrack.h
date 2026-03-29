#pragma once
#include "TrackBase.h"

/**
 * AudioTrack — F1 (MVP: single track)
 *
 * Captures audio from an input device and plays back recorded audio.
 * Mono or stereo input selectable per instance.
 */
class AudioTrack : public TrackBase
{
public:
    enum class InputMode { Mono, Stereo };

    explicit AudioTrack (const juce::String& name = "Audio 1");
    ~AudioTrack() override = default;

    //==========================================================================
    void prepareToPlay  (double sampleRate, int samplesPerBlock) override;
    void processBlock   (juce::AudioBuffer<float>& buffer,
                         juce::MidiBuffer&         midiMessages) override;
    void releaseResources() override;

    //==========================================================================
    void setInputMode      (InputMode mode)    { inputMode = mode; }
    void setInputChannel   (int channel)       { inputChannel = channel; }
    void setMonitorEnabled (bool enable)       { monitoring = enable; }

    bool isRecording()  const noexcept { return recording; }
    void startRecording();
    void stopRecording();

    /** Recorded audio (all captured blocks concatenated). */
    const juce::AudioBuffer<float>& getRecordedAudio() const { return recordedAudio; }
    void clearRecording();

    juce::ValueTree toValueTree()                       const override;
    void            fromValueTree (const juce::ValueTree& tree) override;

private:
    InputMode inputMode    { InputMode::Mono };
    int       inputChannel { 0 };
    bool      monitoring   { false };
    bool      recording    { false };

    double currentSampleRate    { 44100.0 };
    int    currentSamplesPerBlock { 512 };

    juce::AudioBuffer<float> recordedAudio;
    int                      recordWritePos { 0 };

    static constexpr int kMaxRecordSeconds = 300;  ///< 5-minute pre-allocation

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioTrack)
};
