#pragma once
#include <JuceHeader.h>
#include "../tracks/TrackBase.h"
#include "../tracks/AudioTrack.h"

/**
 * TrackView — horizontal UI strip for a single track.
 *
 * Displays: track name, volume fader, pan knob, mute button, solo button.
 * Audio tracks also show an Import button to load a file into the buffer.
 * DrummerTrack instances are rendered with a distinct background colour.
 */
class TrackView : public juce::Component,
                  private juce::Button::Listener,
                  private juce::Slider::Listener
{
public:
    explicit TrackView (TrackBase& track, bool isDrummerTrack = false);
    ~TrackView() override;

    void paint   (juce::Graphics& g) override;
    void resized () override;

private:
    void buttonClicked (juce::Button* button) override;
    void sliderValueChanged (juce::Slider* slider) override;

    void openImportDialog();

    TrackBase& track;
    bool       isDrummer;

    juce::Label       nameLabel;
    juce::Slider      volumeSlider;
    juce::Slider      panSlider;
    juce::TextButton  muteButton    { "M" };
    juce::TextButton  soloButton    { "S" };
    juce::TextButton  importButton  { "Import" };  // audio tracks only

    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackView)
};
