#include "TrackBase.h"

TrackBase::TrackBase (juce::String trackName)
    : name (std::move (trackName))
{}

juce::ValueTree TrackBase::toValueTree() const
{
    juce::ValueTree tree ("Track");
    tree.setProperty ("name",   name,          nullptr);
    tree.setProperty ("volume", volume,        nullptr);
    tree.setProperty ("pan",    pan,           nullptr);
    tree.setProperty ("muted",  muted.load(),  nullptr);
    tree.setProperty ("soloed", soloed.load(), nullptr);
    return tree;
}

void TrackBase::fromValueTree (const juce::ValueTree& tree)
{
    name   = tree.getProperty ("name",   name).toString();
    volume = tree.getProperty ("volume", volume);
    pan    = tree.getProperty ("pan",    pan);
    muted  = static_cast<bool> (tree.getProperty ("muted",  false));
    soloed = static_cast<bool> (tree.getProperty ("soloed", false));
}
