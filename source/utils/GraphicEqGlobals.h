#pragma once

#include <JuceHeader.h>

//==============================================================================
namespace GraphicEqGlobals {

constexpr float getMaxDecibels() { return 24.f; }
constexpr float getNegativeInf() { return -72.f; }

constexpr int getMinBandNum() { return 0; }
constexpr int getMaxBandNum() { return 7; }

constexpr int getNumMaxBands() { return 8; }
constexpr int getNumPeakBands() { return 6; }

constexpr float getMinFrequency() { return 20.f; }
constexpr float getMaxFrequency() { return 20000.f; }

constexpr float getBorderCornerRadius() { return 5.f; }
constexpr float getBorderThickness() { return 2.f; }

static const juce::Font getFont() { return juce::Font(juce::Font::getDefaultMonospacedFontName(), 10.f, 0); }

enum Channel {
    Left,
    Right
};

enum FFTOrder {
    order2048 = 11,
    order4096 = 12,
    order8192 = 13
};

}
