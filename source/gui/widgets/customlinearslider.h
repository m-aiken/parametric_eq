#pragma once

#include <JuceHeader.h>

struct CustomLinearSlider : juce::Slider {
    explicit CustomLinearSlider(juce::RangedAudioParameter& rap);

    void paint(juce::Graphics& g) override;

private:
    juce::RangedAudioParameter* param;
};
