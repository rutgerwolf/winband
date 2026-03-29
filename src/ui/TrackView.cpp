#include "TrackView.h"

TrackView::TrackView (TrackBase& t, bool drummer)
    : track (t), isDrummer (drummer)
{
    nameLabel.setText (track.getName(), juce::dontSendNotification);
    nameLabel.setColour (juce::Label::textColourId, juce::Colour (0xffe0e0e0));
    addAndMakeVisible (nameLabel);

    volumeSlider.setRange (0.0, 1.0, 0.01);
    volumeSlider.setValue (track.getVolume(), juce::dontSendNotification);
    volumeSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.addListener (this);
    addAndMakeVisible (volumeSlider);

    panSlider.setRange (-1.0, 1.0, 0.01);
    panSlider.setValue (track.getPan(), juce::dontSendNotification);
    panSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    panSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    panSlider.addListener (this);
    addAndMakeVisible (panSlider);

    muteButton.setToggleable (true);
    muteButton.setToggleState (track.isMuted(), juce::dontSendNotification);
    muteButton.addListener (this);
    addAndMakeVisible (muteButton);

    soloButton.setToggleable (true);
    soloButton.setToggleState (track.isSoloed(), juce::dontSendNotification);
    soloButton.addListener (this);
    addAndMakeVisible (soloButton);
}

TrackView::~TrackView()
{
    volumeSlider.removeListener (this);
    panSlider.removeListener (this);
    muteButton.removeListener (this);
    soloButton.removeListener (this);
}

void TrackView::paint (juce::Graphics& g)
{
    const auto bg = isDrummer
        ? juce::Colour (0xff1b1b3a)   // drummerTrack from dark.json
        : juce::Colour (0xff16213e);  // trackBackground

    g.fillAll (bg);
    g.setColour (juce::Colour (0xff2a2a4a));
    g.drawRect (getLocalBounds(), 1);
}

void TrackView::resized()
{
    auto area = getLocalBounds().reduced (6);

    nameLabel   .setBounds (area.removeFromLeft (120));
    muteButton  .setBounds (area.removeFromRight (28));
    soloButton  .setBounds (area.removeFromRight (28));
    panSlider   .setBounds (area.removeFromRight (48));
    volumeSlider.setBounds (area);
}

void TrackView::buttonClicked (juce::Button* button)
{
    if (button == &muteButton)
        track.setMuted (muteButton.getToggleState());
    else if (button == &soloButton)
        track.setSoloed (soloButton.getToggleState());
}

void TrackView::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &volumeSlider)
        track.setVolume (static_cast<float> (volumeSlider.getValue()));
    else if (slider == &panSlider)
        track.setPan (static_cast<float> (panSlider.getValue()));
}
