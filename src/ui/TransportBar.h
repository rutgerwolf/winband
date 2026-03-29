#pragma once
#include <JuceHeader.h>

/**
 * TransportBar — play, stop, record buttons + BPM display.
 *
 * Broadcasts transport state changes via a simple Listener interface so
 * AudioEngine and DrummerTrack can react without coupling to the UI.
 */
class TransportBar : public juce::Component,
                     private juce::Button::Listener,
                     private juce::Label::Listener
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void transportPlay()   = 0;
        virtual void transportStop()   = 0;
        virtual void transportRecord() = 0;
        virtual void transportRewind() = 0;
        virtual void bpmChanged (double newBpm) = 0;
        virtual void tapTempo()        = 0;
    };

    TransportBar();
    ~TransportBar() override;

    void addListener    (Listener* l) { listeners.add (l); }
    void removeListener (Listener* l) { listeners.remove (l); }

    void setPlaying   (bool playing);
    void setRecording (bool recording);
    void setBpm       (double bpm);

    void paint   (juce::Graphics& g) override;
    void resized () override;

private:
    void buttonClicked      (juce::Button* button) override;
    void labelTextChanged   (juce::Label*  label)  override;

    juce::TextButton playButton   { "\u25B6" };   // ▶
    juce::TextButton stopButton   { "\u25A0" };   // ■
    juce::TextButton recordButton { "\u25CF" };   // ●
    juce::TextButton rewindButton { "\u23EE" };   // ⏮
    juce::TextButton tapButton    { "TAP" };
    juce::Label      bpmLabel;
    juce::Label      bpmDisplay;

    double currentBpm { 120.0 };

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TransportBar)
};
