#pragma once

#include <JuceHeader.h>

class CustomLinearSlider final : public juce::Slider
{
public:
    explicit CustomLinearSlider(juce::RangedAudioParameter* rap);

    void paint(juce::Graphics& g) override;

private:
    juce::RangedAudioParameter* param;
};
