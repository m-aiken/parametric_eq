#pragma once

#include <JuceHeader.h>

class CustomLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel() = default;

    void drawRotarySlider(juce::Graphics& g,
                          int             x,
                          int             y,
                          int             width,
                          int             height,
                          float           sliderPosProportional,
                          float           rotaryStartAngle,
                          float           rotaryEndAngle,
                          juce::Slider&   slider) override;

    void drawLinearSlider(juce::Graphics&                 g,
                          int                             x,
                          int                             y,
                          int                             width,
                          int                             height,
                          float                           sliderPos,
                          float                           minSliderPos,
                          float                           maxSliderPos,
                          const juce::Slider::SliderStyle sliderStyle,
                          juce::Slider&                   slider) override;
    /*
    void drawToggleButton(juce::Graphics& g,
                          juce::ToggleButton& toggleButton,
                          bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override;
    */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CustomLookAndFeel)
};
