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

    static juce::Font getFont();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Theme)
};
