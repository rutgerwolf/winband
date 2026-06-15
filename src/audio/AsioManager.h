#pragma once
#include <JuceHeader.h>

/**
 * Thin wrapper around JUCE's ASIO device support.
 *
 * Provides:
 *   - Enumeration of installed ASIO drivers
 *   - Convenience method to open the control panel of the active driver
 *   - Buffer-size / sample-rate negotiation helpers
 *
 * Note: JUCE handles ASIO internally via AudioDeviceManager when
 * JUCE_ASIO=1 is defined.  This class provides WinBand-specific helpers
 * on top of that.
 */
class AsioManager
{
public:
    AsioManager()  = default;
    ~AsioManager() = default;

    /** Returns a list of ASIO driver names available on this system. */
    static juce::StringArray getAvailableDriverNames();

    /**
     * Opens the ASIO control panel for the device currently active in
     * the supplied AudioDeviceManager.
     */
    static void openControlPanel(juce::AudioDeviceManager& manager);

    /**
     * Returns the preferred buffer sizes for the given ASIO device name,
     * or an empty array if the device is not found.
     */
    static juce::Array<int> getSupportedBufferSizes(const juce::String& driverName);

private:
    JUCE_DECLARE_NON_COPYABLE(AsioManager)
};
