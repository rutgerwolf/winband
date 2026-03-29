#include "TrackBase.h"

TrackBase::TrackBase (juce::String trackName)
    : name (std::move (trackName))
{}

juce::ValueTree TrackBase::toValueTree() const
{
    juce::ValueTree tree ("Track");
    tree.setProperty ("name",   name,   nullptr);
    tree.setProperty ("volume", volume, nullptr);
    tree.setProperty ("pan",    pan,    nullptr);
    tree.setProperty ("muted",  muted,  nullptr);
    tree.setProperty ("soloed", soloed, nullptr);
    return tree;
}

void TrackBase::fromValueTree (const juce::ValueTree& tree)
{
    name   = tree.getProperty ("name",   name);
    volume = tree.getProperty ("volume", volume);
    pan    = tree.getProperty ("pan",    pan);
    muted  = tree.getProperty ("muted",  muted);
    soloed = tree.getProperty ("soloed", soloed);
}
