#pragma once

#include <JuceHeader.h>

namespace ParametricEq::Theme
{

extern const juce::Colour MAIN_BACKGROUND;
extern const juce::Colour TEXT;
extern const juce::Colour TEXT_DISABLED;
extern const juce::Colour ANALYSER_TEXT;
extern const juce::Colour ROTARY_FILL;
extern const juce::Colour ROTARY_THUMB;
extern const juce::Colour RESPONSE_CURVE_FILL;
extern const juce::Colour RESPONSE_CURVE_AVG;
extern const juce::Colour NODE_ON;
extern const juce::Colour NODE_OFF;
extern const juce::Colour ANALYSER_GRID;
extern const juce::Colour ANALYSER_PRE_EQ;
extern const juce::Colour ANALYSER_POST_EQ;
extern const juce::Colour BAND_BACKGROUND;
extern const juce::Colour ACTIVE_BAND_BACKGROUND;
extern const juce::Colour TEXT_BUTTON_BG_STD;
extern const juce::Colour TEXT_BUTTON_BG_HOVER;
extern const juce::Colour POWER_ICON;
extern const juce::Colour POWER_ORB_ON;
extern const juce::Colour POWER_ORB_OFF;
extern const juce::Colour WAVEFORM_BUTTON_ON;
extern const juce::Colour WAVEFORM_BUTTON_OFF;

extern const juce::Colour PLUGIN_DISABLED_ANALYSER_TEXT;
extern const juce::Colour PLUGIN_DISABLED_RESPONSE_CURVE_FILL;
extern const juce::Colour PLUGIN_DISABLED_RESPONSE_CURVE_AVG;
extern const juce::Colour PLUGIN_DISABLED_NODE;
extern const juce::Colour PLUGIN_DISABLED_BAND_BACKGROUND;

inline constexpr float ENABLED_ALPHA  = 1.f;
inline constexpr float DISABLED_ALPHA = 0.2f;
inline constexpr int   UI_PADDING     = 4;
inline constexpr float BORDER_RADIUS  = 4.f;

extern juce::Font getFont();

}  // namespace ParametricEq::Theme
