#include "TransportBar.h"

TransportBar::TransportBar()
{
    for (auto* btn : { &playButton, &stopButton, &recordButton, &rewindButton, &tapButton })
    {
        btn->addListener (this);
        addAndMakeVisible (btn);
    }

    recordButton.setColour (juce::TextButton::buttonColourId,    juce::Colour (0xff330000));
    recordButton.setColour (juce::TextButton::buttonOnColourId,  juce::Colour (0xffcc2200));
    recordButton.setToggleable (true);
    playButton  .setToggleable (true);

    bpmLabel.setText ("BPM", juce::dontSendNotification);
    bpmLabel.setColour (juce::Label::textColourId, juce::Colour (0xff888899));
    addAndMakeVisible (bpmLabel);

    bpmDisplay.setText (juce::String (currentBpm, 1), juce::dontSendNotification);
    bpmDisplay.setEditable (true);
    bpmDisplay.setColour (juce::Label::textColourId,       juce::Colour (0xffe0e0e0));
    bpmDisplay.setColour (juce::Label::backgroundColourId, juce::Colour (0xff1a1a2e));
    bpmDisplay.setJustificationType (juce::Justification::centred);
    bpmDisplay.addListener (this);
    addAndMakeVisible (bpmDisplay);
}

TransportBar::~TransportBar()
{
    for (auto* btn : { &playButton, &stopButton, &recordButton, &rewindButton, &tapButton })
        btn->removeListener (this);
    bpmDisplay.removeListener (this);
}

void TransportBar::setPlaying (bool playing)
{
    playButton.setToggleState (playing, juce::dontSendNotification);
}

void TransportBar::setRecording (bool recording)
{
    recordButton.setToggleState (recording, juce::dontSendNotification);
}

void TransportBar::setBpm (double bpm)
{
    currentBpm = bpm;
    bpmDisplay.setText (juce::String (bpm, 1), juce::dontSendNotification);
}

void TransportBar::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff111122));  // transportBackground
    g.setColour (juce::Colour (0xff2a2a4a));
    g.drawRect (getLocalBounds(), 1);
}

void TransportBar::resized()
{
    auto area = getLocalBounds().reduced (6);

    rewindButton.setBounds (area.removeFromLeft (40));
    area.removeFromLeft (4);
    playButton  .setBounds (area.removeFromLeft (40));
    area.removeFromLeft (4);
    stopButton  .setBounds (area.removeFromLeft (40));
    area.removeFromLeft (4);
    recordButton.setBounds (area.removeFromLeft (40));
    area.removeFromLeft (16);
    tapButton   .setBounds (area.removeFromLeft (48));
    area.removeFromLeft (16);
    bpmLabel    .setBounds (area.removeFromLeft (32));
    bpmDisplay  .setBounds (area.removeFromLeft (64));
}

void TransportBar::buttonClicked (juce::Button* button)
{
    if (button == &playButton)    listeners.call ([] (Listener& l) { l.transportPlay(); });
    if (button == &stopButton)    listeners.call ([] (Listener& l) { l.transportStop(); });
    if (button == &recordButton)  listeners.call ([] (Listener& l) { l.transportRecord(); });
    if (button == &rewindButton)  listeners.call ([] (Listener& l) { l.transportRewind(); });
    if (button == &tapButton)     listeners.call ([] (Listener& l) { l.tapTempo(); });
}

void TransportBar::labelTextChanged (juce::Label* label)
{
    if (label == &bpmDisplay)
    {
        const double newBpm = juce::jlimit (40.0, 240.0,
            bpmDisplay.getText().getDoubleValue());
        currentBpm = newBpm;
        const double bpmCopy = newBpm;
        listeners.call ([bpmCopy] (Listener& l) { l.bpmChanged (bpmCopy); });
    }
}
