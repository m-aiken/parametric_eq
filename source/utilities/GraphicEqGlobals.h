#pragma once

#include <JuceHeader.h>

namespace GraphicEq
{

constexpr float MIN_HZ         = 20.f;
constexpr float MAX_HZ         = 20000.f;
constexpr float MIN_DB         = -72.f;
constexpr float MAX_DB         = 24.f;
constexpr int   MAX_NUM_BANDS  = 8;
constexpr int   MIN_BAND_INDEX = 0;
constexpr int   MAX_BAND_INDEX = 7;
constexpr int   NUM_PEAK_BANDS = 6;

constexpr float BORDER_CORNER_RADIUS = 5.f;
constexpr float BORDER_THICKNESS     = 2.f;

const juce::Font FONT = juce::FontOptions(juce::Font::getDefaultMonospacedFontName(), 10.f, juce::Font::plain);

enum Channel {
    Left,
    Right,
};

enum FFTOrder {
    order2048 = 11,
    order4096 = 12,
    order8192 = 13,
};

}  // namespace GraphicEq
