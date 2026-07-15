#pragma once

#include <JuceHeader.h>

namespace parametric_eq::theme
{

extern const juce::Colour MAIN_BACKGROUND;
extern const juce::Colour TEXT;
extern const juce::Colour TEXT_DISABLED;
extern const juce::Colour ANALYSER_TEXT;
extern const juce::Colour RESPONSE_CURVE_FILL;
extern const juce::Colour RESPONSE_CURVE_DISABLED;
extern const juce::Colour RESPONSE_CURVE_AVG;
extern const juce::Colour NODE_ON;
extern const juce::Colour NODE_OFF;
extern const juce::Colour ANALYSER_GRID;
extern const juce::Colour ANALYSER_WAVEFORM;
extern const juce::Colour ACTIVE_BAND_BACKGROUND;
extern const juce::Colour TEXT_BUTTON_BG_HOVER;

inline constexpr float ENABLED_ALPHA  = 1.f;
inline constexpr float DISABLED_ALPHA = 0.2f;
inline constexpr int   UI_PADDING     = 4;
inline constexpr float BORDER_RADIUS  = 4.f;

extern juce::Font getFont();

}  // namespace parametric_eq::theme
