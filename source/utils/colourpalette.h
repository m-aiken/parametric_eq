#pragma once

#include <JuceHeader.h>

#include "TailwindColours.h"

namespace ColourPalette
{

static bool darkMode = false;

static void
toggleDarkMode()
{
    darkMode = !darkMode;
}
/*
enum Palette {
    Eggshell,
    Salmon,
    Blue,
    Green
};
*/
typedef std::pair< juce::Colour, juce::Colour > DarkLightPair;

static const DarkLightPair MainBackground   = DarkLightPair(TailwindColours::Gray800, TailwindColours::Amber50);
static const DarkLightPair Text             = DarkLightPair(TailwindColours::Amber50, TailwindColours::Gray800);
static const DarkLightPair RotaryFill       = DarkLightPair(TailwindColours::Teal400, TailwindColours::Red400);
static const DarkLightPair RotaryText       = DarkLightPair(TailwindColours::Gray800, TailwindColours::Gray800);
static const DarkLightPair SliderTooltip    = DarkLightPair(TailwindColours::Teal400, TailwindColours::Red400);
static const DarkLightPair SliderFill       = DarkLightPair(TailwindColours::Gray700, TailwindColours::Red50);
static const DarkLightPair PowerButtonOn    = DarkLightPair(TailwindColours::Teal400, TailwindColours::Green400);
static const DarkLightPair PowerButtonOff   = DarkLightPair(TailwindColours::Amber50, TailwindColours::Gray800);
static const DarkLightPair ResponseCurve    = DarkLightPair(TailwindColours::Red400, TailwindColours::Red400);
static const DarkLightPair ResponseCurveAvg = DarkLightPair(TailwindColours::Amber50, TailwindColours::Gray800);
static const DarkLightPair NodeOn           = DarkLightPair(TailwindColours::Red400, TailwindColours::Red400);
static const DarkLightPair NodeOff          = DarkLightPair(TailwindColours::Amber50, TailwindColours::Gray800);
static const DarkLightPair AnalyzerPreEq    = DarkLightPair(TailwindColours::Red400, TailwindColours::Red400);
static const DarkLightPair AnalyzerPostEq   = DarkLightPair(TailwindColours::Teal400, TailwindColours::Teal400);
static const DarkLightPair PeakBandFocus    = DarkLightPair(TailwindColours::Teal300.withAlpha(0.05f),
                                                         TailwindColours::Red300.withAlpha(0.15f));

/*
inline std::map<Palette, juce::Colour> colourMap = {
    { Eggshell, juce::Colour(244u, 241u, 222u) },
    { Salmon, juce::Colour(224u, 122u, 95u) },
    { Blue, juce::Colour(61u, 64u, 91u) },
    { Green, juce::Colour(129u, 178u, 154u) }
};
*/
// inline juce::Colour getColour(Palette c) { return colourMap[c]; }

inline juce::Colour
getColourV2(DarkLightPair c)
{
    return darkMode ? c.first : c.second;
}

}  // namespace ColourPalette
