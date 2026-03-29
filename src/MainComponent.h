#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "ui/TransportBar.h"
#include "ui/TrackView.h"

/**
 * Top-level component that owns the AudioEngine and lays out the main UI.
 *
 * Layout (top → bottom):
 *   TransportBar  — fixed height, play/stop/record/BPM
 *   TrackView     — fills remaining space, scrollable track list
 */
class MainComponent : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioEngine   audioEngine;
    TransportBar  transportBar;
    TrackView     trackView;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
