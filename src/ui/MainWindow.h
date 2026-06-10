#pragma once
#include <JuceHeader.h>

class MainComponent;

/**
 * MainWindow — top-level application window.
 *
 * Hosts MainComponent as its content, applies the dark background colour,
 * and handles close/quit.
 */
class MainWindow : public juce::DocumentWindow
{
public:
    explicit MainWindow (const juce::String& title);
    ~MainWindow() override = default;

    void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
