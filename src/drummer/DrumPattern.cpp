#include "DrumPattern.h"
#include "AdaptiveDrummer.h"   // for DrumStyle enum

DrumPattern::DrumPattern()
{
    buildRock();
}

void DrumPattern::loadStyle (DrumStyle style)
{
    std::memset (layers, 0, sizeof (layers));
    switch (style)
    {
        case DrumStyle::Rock:        buildRock();        break;
        case DrumStyle::Jazz:        buildJazz();        break;
        case DrumStyle::Funk:        buildFunk();        break;
        case DrumStyle::Electronic:  buildElectronic();  break;
        case DrumStyle::Brushes:     buildBrushes();     break;
    }
}

void DrumPattern::setDensity (Density density)
{
    currentDensity = density;
}

uint8_t DrumPattern::getStep (int stepIndex) const
{
    jassert (stepIndex >= 0 && stepIndex < kSteps);
    return layers[static_cast<int> (currentDensity)][stepIndex];
}

int DrumPattern::getLengthInSamples (double bpm, double sampleRate) const
{
    // 1 bar = 4 beats at the given BPM
    const double secondsPerBar = (60.0 / bpm) * 4.0;
    return static_cast<int> (secondsPerBar * sampleRate);
}

// ── Pattern builders ──────────────────────────────────────────────────────────
//  Steps: 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
//         1     e     +     a     2     e     +     a

void DrumPattern::buildRock()
{
    // Sparse: just kick and snare on the 2 and 4
    layers[0][ 0] = Kick;
    layers[0][ 8] = Kick;
    layers[0][ 4] = Snare;
    layers[0][12] = Snare;

    // Medium: add closed hi-hats on every beat
    std::memcpy (layers[1], layers[0], kSteps);
    for (int i = 0; i < kSteps; i += 2) layers[1][i] |= HiHat;

    // Full: hi-hats on every 16th, extra kicks
    std::memcpy (layers[2], layers[1], kSteps);
    for (int i = 0; i < kSteps; ++i)   layers[2][i] |= HiHat;
    layers[2][ 6] |= Kick;
    layers[2][14] |= Kick;
    layers[2][ 0] |= Crash;
}

void DrumPattern::buildJazz()
{
    // Sparse: ride on beats 1 and 3, hi-hat on 2 and 4
    layers[0][ 0] = Ride;
    layers[0][ 8] = Ride;
    layers[0][ 4] = HiHat;
    layers[0][12] = HiHat;

    // Medium: jazz ride cymbal pattern
    std::memcpy (layers[1], layers[0], kSteps);
    layers[1][ 0] |= Ride;
    layers[1][ 3] |= Ride;
    layers[1][ 6] |= Ride;
    layers[1][10] |= Ride;
    layers[1][ 4] |= Kick;

    // Full: add snare brushes and occasional tom
    std::memcpy (layers[2], layers[1], kSteps);
    layers[2][ 4] |= Snare;
    layers[2][12] |= Snare;
    layers[2][10] |= Tom;
}

void DrumPattern::buildFunk()
{
    // Sparse: syncopated kick on 1 and the "and" of 2
    layers[0][ 0] = Kick;
    layers[0][ 6] = Kick;
    layers[0][ 4] = Snare;
    layers[0][12] = Snare;

    // Medium: 16th-note hi-hats with ghost snare
    std::memcpy (layers[1], layers[0], kSteps);
    for (int i = 0; i < kSteps; ++i) layers[1][i] |= HiHat;
    layers[1][ 2] |= Snare;  // ghost
    layers[1][10] |= Snare;  // ghost

    // Full: extra kick, open hi-hat
    std::memcpy (layers[2], layers[1], kSteps);
    layers[2][11] |= Kick;
    layers[2][ 8] |= (HiHat | Kick);
}

void DrumPattern::buildElectronic()
{
    // Sparse: four-on-the-floor kick, clap on 2 and 4
    for (int i = 0; i < kSteps; i += 4) layers[0][i] = Kick;
    layers[0][ 4] |= Snare;
    layers[0][12] |= Snare;

    // Medium: add closed hi-hats
    std::memcpy (layers[1], layers[0], kSteps);
    for (int i = 2; i < kSteps; i += 4) layers[1][i] |= HiHat;

    // Full: every 16th note hi-hat, open hat, ride accent
    std::memcpy (layers[2], layers[1], kSteps);
    for (int i = 0; i < kSteps; ++i)   layers[2][i] |= HiHat;
    layers[2][ 2] |= (HiHat);   // open
    layers[2][10] |= (HiHat);
}

void DrumPattern::buildBrushes()
{
    // Brushes: soft snare swirl on every beat, ride for pulse
    for (int i = 0; i < kSteps; i += 4)
    {
        layers[0][i] = Ride;
        layers[1][i] = Ride | Snare;
        layers[2][i] = Ride | Snare | HiHat;
    }
    layers[0][4]  |= Kick;
    layers[1][4]  |= Kick;
    layers[2][4]  |= Kick;
    layers[2][12] |= Kick;
}
