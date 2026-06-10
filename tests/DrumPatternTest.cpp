#include <JuceHeader.h>
#include "../src/drummer/DrumPattern.h"
#include "../src/drummer/AdaptiveDrummer.h"  // DrumStyle enum

/**
 * DrumPatternTest
 *
 * JUCE unit tests for DrumPattern step logic.
 * Registered automatically; run via juce::UnitTestRunner in main() (debug).
 */
class DrumPatternTest : public juce::UnitTest
{
public:
    DrumPatternTest() : UnitTest ("DrumPattern", "WinBand") {}

    void runTest() override
    {
        // ── Step count ────────────────────────────────────────────────────────────
        beginTest ("kSteps == 16");
        expectEquals (DrumPattern::kSteps, 16);

        // ── getLengthInSamples ────────────────────────────────────────────────────
        beginTest ("getLengthInSamples: 120 BPM, 44100 Hz = 88200 samples");
        {
            DrumPattern p;
            // 1 bar = 4 beats; at 120 BPM = 0.5 s/beat → 2 s → 88200 samples
            const int expected = static_cast<int> ((60.0 / 120.0) * 4.0 * 44100.0);
            expectEquals (p.getLengthInSamples (120.0, 44100.0), expected);
        }

        beginTest ("getLengthInSamples: 90 BPM, 48000 Hz");
        {
            DrumPattern p;
            const int expected = static_cast<int> ((60.0 / 90.0) * 4.0 * 48000.0);
            expectEquals (p.getLengthInSamples (90.0, 48000.0), expected);
        }

        // ── Rock patterns ─────────────────────────────────────────────────────────
        beginTest ("Rock Sparse: kick on 0 & 8, snare on 4 & 12, no hihat");
        {
            DrumPattern p;
            p.loadStyle (DrumStyle::Rock);
            p.setDensity (DrumPattern::Density::Sparse);

            expect ((p.getStep (0)  & DrumPattern::Kick)  != 0);
            expect ((p.getStep (8)  & DrumPattern::Kick)  != 0);
            expect ((p.getStep (4)  & DrumPattern::Snare) != 0);
            expect ((p.getStep (12) & DrumPattern::Snare) != 0);
            // No hi-hat in sparse rock
            for (int i = 0; i < DrumPattern::kSteps; ++i)
                expect ((p.getStep (i) & DrumPattern::HiHat) == 0,
                        "Unexpected HiHat in Sparse Rock step " + juce::String (i));
        }

        beginTest ("Rock Medium: hihat on every even step");
        {
            DrumPattern p;
            p.loadStyle (DrumStyle::Rock);
            p.setDensity (DrumPattern::Density::Medium);
            for (int i = 0; i < DrumPattern::kSteps; i += 2)
                expect ((p.getStep (i) & DrumPattern::HiHat) != 0,
                        "HiHat missing at step " + juce::String (i));
        }

        beginTest ("Rock Full: hihat on every step");
        {
            DrumPattern p;
            p.loadStyle (DrumStyle::Rock);
            p.setDensity (DrumPattern::Density::Full);
            for (int i = 0; i < DrumPattern::kSteps; ++i)
                expect ((p.getStep (i) & DrumPattern::HiHat) != 0,
                        "HiHat missing at step " + juce::String (i));
            // Crash on beat 1
            expect ((p.getStep (0) & DrumPattern::Crash) != 0);
        }

        // ── Electronic patterns ───────────────────────────────────────────────────
        beginTest ("Electronic Sparse: four-on-the-floor kick");
        {
            DrumPattern p;
            p.loadStyle (DrumStyle::Electronic);
            p.setDensity (DrumPattern::Density::Sparse);
            for (int i = 0; i < DrumPattern::kSteps; i += 4)
                expect ((p.getStep (i) & DrumPattern::Kick) != 0,
                        "Kick missing on beat " + juce::String (i));
            // No kick off-beat in sparse
            expect ((p.getStep (2) & DrumPattern::Kick) == 0);
        }

        // ── All styles smoke test ──────────────────────────────────────────────────
        beginTest ("All styles and densities: no crash or assert");
        {
            const DrumStyle styles[] = { DrumStyle::Rock, DrumStyle::Jazz,
                                         DrumStyle::Funk, DrumStyle::Electronic,
                                         DrumStyle::Brushes };
            const DrumPattern::Density densities[] = { DrumPattern::Density::Sparse,
                                                        DrumPattern::Density::Medium,
                                                        DrumPattern::Density::Full };
            DrumPattern p;
            for (auto style : styles)
            {
                p.loadStyle (style);
                for (auto density : densities)
                {
                    p.setDensity (density);
                    for (int step = 0; step < DrumPattern::kSteps; ++step)
                        p.getStep (step);  // must not assert
                }
            }
            expect (true);  // reached here without crash
        }

        // ── Density switching ──────────────────────────────────────────────────────
        beginTest ("Density switch: Sparse kick unchanged after switching to Full");
        {
            DrumPattern p;
            p.loadStyle (DrumStyle::Rock);
            p.setDensity (DrumPattern::Density::Full);
            p.setDensity (DrumPattern::Density::Sparse);
            // After switching back, sparse layer still correct
            expect ((p.getStep (0) & DrumPattern::Kick) != 0);
            expect ((p.getStep (0) & DrumPattern::HiHat) == 0);
        }
    }
};

static DrumPatternTest drumPatternTestInstance;
