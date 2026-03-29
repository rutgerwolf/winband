#pragma once
#include <JuceHeader.h>
#include "audio/AudioEngine.h"
#include "tracks/AudioTrack.h"
#include "tracks/DrummerTrack.h"
#include "ui/TransportBar.h"
#include "ui/TrackView.h"

/**
 * MainComponent — root UI component and application coordinator.
 *
 * Owns the AudioEngine, both MVP tracks, and the UI strips.
 * Implements TransportBar::Listener to route button presses to the engine.
 *
 * Layout (top → bottom):
 *   TransportBar  (fixed height)
 *   AudioTrack strip
 *   DrummerTrack strip
 */
class MainComponent : public juce::Component,
                       private TransportBar::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint   (juce::Graphics& g) override;
    void resized () override;

private:
    // ── TransportBar::Listener ─────────────────────────────────────────────────
    void transportPlay()            override;
    void transportStop()            override;
    void transportRecord()          override;
    void transportRewind()          override;
    void bpmChanged (double newBpm) override;
    void tapTempo()                 override;

    // ── Audio ─────────────────────────────────────────────────────────────────
    AudioEngine  audioEngine;
    AudioTrack   audioTrack   { "Audio 1" };
    DrummerTrack drummerTrack { "Drummer" };

    // ── UI ───────────────────────────────────────────────────────────────────
    TransportBar transportBar;
    TrackView    audioTrackView   { audioTrack,   false };
    TrackView    drummerTrackView { drummerTrack, true  };

    static constexpr int kTransportHeight = 52;
    static constexpr int kTrackHeight     = 72;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
