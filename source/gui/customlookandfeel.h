#pragma once

#include <JuceHeader.h>

struct CustomLookAndFeel : juce::LookAndFeel_V4 {
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
};
