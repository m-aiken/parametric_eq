#pragma once

#include <JuceHeader.h>

class Theme
{
private:
    static const juce::Colour DARK_GREY;
    static const juce::Colour PARCHMENT;
    static const juce::Colour RED;
    static const juce::Colour TEAL;
    static const juce::Colour PURPLE;
    static const juce::Colour GREEN;

public:
    static const juce::Colour MAIN_BACKGROUND;
    static const juce::Colour TEXT;
    static const juce::Colour ROTARY_FILL;
    static const juce::Colour ROTARY_THUMB;
    static const juce::Colour RESPONSE_CURVE;
    static const juce::Colour RESPONSE_CURVE_AVG;
    static const juce::Colour NODE_ON;
    static const juce::Colour NODE_OFF;
    static const juce::Colour ANALYSER_GRID;
    static const juce::Colour ANALYSER_PRE_EQ;
    static const juce::Colour ANALYSER_POST_EQ;
    static const juce::Colour PEAK_BAND_FOCUS;
    static const juce::Colour TEXT_BUTTON_BG_STD;
    static const juce::Colour TEXT_BUTTON_BG_HOVER;

    static constexpr float ENABLED_ALPHA  = 1.f;
    static constexpr float DISABLED_ALPHA = 0.2f;
    static constexpr int   UI_PADDING     = 4;
    static constexpr float BORDER_RADIUS  = 4.f;

    static juce::Font getFont();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Theme)
};
