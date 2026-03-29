#include "MainComponent.h"

MainComponent::MainComponent()
{
    // ── Wire transport listener ───────────────────────────────────────────────────
    transportBar.addListener (this);

    // ── Register tracks with the engine ──────────────────────────────────────────
    audioEngine.addTrack (&audioTrack);
    audioEngine.addTrack (&drummerTrack);

    // ── Load Salamander samples ────────────────────────────────────────────────
    const auto appDir     = juce::File::getSpecialLocation (juce::File::currentApplicationFile)
                                .getParentDirectory();
    const auto salamander = appDir.getChildFile ("assets/samples/salamander");
    if (! drummerTrack.loadSamples (salamander))
        DBG ("MainComponent: Salamander samples not found at " + salamander.getFullPathName()
             + " — drummer will be silent.");

    // ── Initialise audio device ─────────────────────────────────────────────────
    if (! audioEngine.initialise())
        juce::AlertWindow::showMessageBoxAsync (
            juce::MessageBoxIconType::WarningIcon,
            "Audio device error",
            "WinBand could not open an audio device.\n"
            "Check your audio settings and restart the application.");

    // ── UI layout ──────────────────────────────────────────────────────────────
    addAndMakeVisible (transportBar);
    addAndMakeVisible (audioTrackView);
    addAndMakeVisible (drummerTrackView);

    setSize (1280, kTransportHeight + kTrackHeight * 2 + 4);
}

MainComponent::~MainComponent()
{
    transportBar.removeListener (this);
    audioEngine.shutdown();
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff0d0d14));  // windowBackground
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    transportBar    .setBounds (area.removeFromTop (kTransportHeight));
    audioTrackView  .setBounds (area.removeFromTop (kTrackHeight));
    drummerTrackView.setBounds (area.removeFromTop (kTrackHeight));
}

// ── TransportBar::Listener ─────────────────────────────────────────────────────

void MainComponent::transportPlay()
{
    audioTrack.startPlayback();
    audioEngine.play();
    transportBar.setPlaying (true);
}

void MainComponent::transportStop()
{
    audioEngine.stop();
    audioTrack.stopPlayback();
    transportBar.setPlaying   (false);
    transportBar.setRecording (false);
}

void MainComponent::transportRecord()
{
    audioTrack.startRecording();
    audioEngine.record();
    transportBar.setRecording (true);
    transportBar.setPlaying   (true);
}

void MainComponent::transportRewind()
{
    audioEngine.stop();
    audioTrack.stopPlayback();
    audioTrack.stopRecording();
    transportBar.setPlaying   (false);
    transportBar.setRecording (false);
}

void MainComponent::bpmChanged (double newBpm)
{
    drummerTrack.setBpm (newBpm);
}

void MainComponent::tapTempo()
{
    drummerTrack.tapTempo();
    // Reflect derived BPM back to the transport bar
    // (DrummerTrack would need a getBpm() for this — noted for future)
}
