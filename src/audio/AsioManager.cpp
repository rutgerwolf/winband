#include "AsioManager.h"

juce::StringArray AsioManager::getAvailableDriverNames()
{
    juce::StringArray names;

#if JUCE_ASIO
    // Ask JUCE's device type registry for the ASIO device type
    juce::AudioDeviceManager tempManager;
    for (auto* type : tempManager.getAvailableDeviceTypes())
    {
        if (type->getTypeName() == "ASIO")
        {
            names = type->getDeviceNames();
            break;
        }
    }
#endif

    return names;
}

void AsioManager::openControlPanel(juce::AudioDeviceManager& manager)
{
#if JUCE_ASIO
    // showControlPanel() is a public virtual on juce::AudioIODevice — no cast needed.
    if (auto* device = manager.getCurrentAudioDevice())
        device->showControlPanel();
#else
    juce::ignoreUnused(manager);
#endif
}

juce::Array<int> AsioManager::getSupportedBufferSizes(const juce::String& driverName)
{
    juce::Array<int> sizes;

#if JUCE_ASIO
    juce::AudioDeviceManager tempManager;
    for (auto* type : tempManager.getAvailableDeviceTypes())
    {
        if (type->getTypeName() == "ASIO")
        {
            // Create a temporary instance to query buffer sizes
            std::unique_ptr<juce::AudioIODevice> dev(
                type->createDevice(juce::String(), driverName));
            if (dev != nullptr)
                sizes = dev->getAvailableBufferSizes();
            break;
        }
    }
#else
    juce::ignoreUnused(driverName);
#endif

    return sizes;
}
