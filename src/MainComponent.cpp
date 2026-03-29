#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(transportBar);
    addAndMakeVisible(trackView);

    setSize(1280, 768);
}

MainComponent::~MainComponent() = default;

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a1a2e));
}

void MainComponent::resized()
{
    constexpr int kTransportHeight = 52;
    auto area = getLocalBounds();
    transportBar.setBounds(area.removeFromTop(kTransportHeight));
    trackView.setBounds(area);
}
