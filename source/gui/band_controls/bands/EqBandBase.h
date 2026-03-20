#pragma once

#include <JuceHeader.h>

#include "CustomRotarySlider.h"

class EqBandBase : public juce::Component
{
public:
    EqBandBase() = default;

protected:
    static void         initialiseLabel(juce::Label& label);
    static juce::String getFrequencyString(const CustomRotarySlider& slider);

    enum SUB_WIDGET_PROPORTIONS {
        ENABLEMENT_BUTTON = 20,
        CONTROL_SECTION   = 50,
        LABEL_1           = 15,
        LABEL_2           = 15,
    };

    juce::Rectangle< int > getEnablementButtonBounds() const;
    juce::Rectangle< int > getControlSectionBounds() const;
    juce::Rectangle< int > getLabel1Bounds() const;
    juce::Rectangle< int > getLabel2Bounds() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqBandBase)
};
