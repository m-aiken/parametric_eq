#pragma once

#include <JuceHeader.h>

#include "TailwindColours.h"

namespace ColourPalette
{

const juce::Colour MainBackground   = TailwindColours::Amber50;
const juce::Colour Text             = TailwindColours::Gray800;
const juce::Colour RotaryFill       = TailwindColours::Red400;
const juce::Colour RotaryText       = TailwindColours::Gray800;
const juce::Colour SliderTooltip    = TailwindColours::Red400;
const juce::Colour SliderFill       = TailwindColours::Red50;
const juce::Colour PowerButtonOn    = TailwindColours::Green400;
const juce::Colour PowerButtonOff   = TailwindColours::Gray800;
const juce::Colour ResponseCurve    = TailwindColours::Red400;
const juce::Colour ResponseCurveAvg = TailwindColours::Gray800;
const juce::Colour NodeOn           = TailwindColours::Red400;
const juce::Colour NodeOff          = TailwindColours::Gray800;
const juce::Colour AnalyzerPreEq    = TailwindColours::Red400;
const juce::Colour AnalyzerPostEq   = TailwindColours::Teal400;
const juce::Colour PeakBandFocus    = TailwindColours::Red300.withAlpha(0.15f);

}  // namespace ColourPalette
