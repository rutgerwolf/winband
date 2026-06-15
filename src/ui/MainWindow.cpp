#include "MainWindow.h"
#include "../MainComponent.h"

MainWindow::MainWindow (const juce::String& title)
    : DocumentWindow (title,
                      juce::Colour (0xff0d0d14),   // windowBackground from dark.json
                      DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (new MainComponent(), true);
    setResizable (true, true);
    setResizeLimits (960, 600, 3840, 2160);
    centreWithSize (1280, 768);
    setVisible (true);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
